#include "template.hpp"
#include "input.hpp"
#include "solution.hpp"
#include "unit_test.hpp"
#include <filesystem>
namespace fs = std::filesystem;

// PROBLEM_H
/*
Generic DS (boilerplate) // Singleton?
    dynamic array
    Disjoint Set Union

GRAPH_H
    read graph.
    construct graph from INPUT
    construct graph from REPR
    check connectivity <- DSU
    cut leaves

PROBLEM_H
    num_nodes, num_edges, num_terminals,
    Edge set (edge list), Terminal set
    properties

"Almost" Minimum Spanning Tree (allow biased towards edges or "fluctuation")
    add biased or fluctuation

calculate "properties": 
    distance matroid, path tracer // Floyd - Warshall all-pair shortest path

\\ Solution - Chromosome representation
    // (most later steps "abuse the notation" of the repr. in impl.)
    - Binary GA: Performant bitarray
        + Reduce: (make each solution a candidate Steiner tree) MST then leaves cutter
        + Make span: shortest paths connector
    - Mutation (also Neighborhood definition for distance) // add random "bias" edges then make_span
    - Crossover: uniform & make_span

"Deterministics" Heuristic(s):
- MST heuristic: mutates MST based heur
- Shortest path heuristic: add "minimal" number of edges (path tracer)
- Stem heur. : make "clusters" then make_span
- Random heur. : uniform edges then connect

GA and search - related:
    POP_SIZE
    P_CROSSOVER
    P_MUTATION
    R_CHANGE
    N_ELITES
    temperature schedule and timing.

*/

// NOTE: In this implementation, parallelism is discouraged as only one individual, one graph,
// one operation is calculated at a time (singleton && global helpers)

void debug_social(vector<Solution> pop, string title = "") {
    if (title.size()) cout << title << '\n';
    for (auto pi : pop)
        cout << '\t' << pi << ": " << pi.get_objval() << '\n';
}

Social init_population(void) {
    Social pop;
    auto join = [&] (Social group) { pop.insert(end(pop), all_of(group)); };
    join(heuristics_random_set());
    join(heuristics_stem_set());
    join(heuristics_mst_set());
    join(heuristics_sp_set());
    while (pop.size() < POP_SIZE)
        pop.push_back(heuristics_random());
    pop.resize(POP_SIZE);
    return pop;
}

Social roulette_wheel_selection(Social& population) {
    Social pool;
    // a big pie with many sectors
    sort(all_of(population));
    // min problem --> take inversion
    int popsize = size(population);

    vector<Real> fitness(popsize);
    for (int i = 0; i < popsize; i++)
        fitness[i] = (Real) 1 / population[i].get_objval();
    Real sum = 0;
    for (int i = 0; i < popsize; i++)
        sum += fitness[i];
    for (auto &p_i : fitness) p_i /= sum;

    vector<Real> spins;
    for (int i = 0; i < popsize; i++)
        spins.push_back(random_num(0,1));
    sort(all_of(spins));

    sum = 0;
    for (int it = -1, i = 0; i < int(size(spins)); i++) {
        for (; sum-EPS < spins[i] && it+1 < popsize;) 
            sum += fitness[++it];
        pool.push_back(population[it]);
    }
    return pool;
}

void elitism(Social& pop) {
    sort(all_of(pop));
    for (int i = 0, it = N_ELITE; i < N_ELITE; i++) {
        int idx = -1, max_dist = -1;
        for (int j = it; j < int(size(pop)); j++) {
            if (umax(max_dist, pop[i].distance_to(pop[j])))
                idx = j;
        }
        std::swap(pop[idx], pop[it++]);
    }
}

// Newest change: Move Elitism before Crossover and promote them directly to mating_pool
void main_algorithm(std::ofstream& out) {
    cout << "Running algorithm...\n";
    auto population = init_population();
    cout << "\tInit population: Done heuristics\n";
    cout.flush();
    for (int igen = 1; igen <= NUM_GEN; igen++) {
        // cout << "G " << igen << '\n';
        // debug_social(population, "Population");
        elitism(population);
        auto mating_pool = roulette_wheel_selection(population);
        std::copy_backward(begin(population), begin(population) + 2 * N_ELITE, end(mating_pool));
        // debug_social(mating_pool, "Pool");
        // Crossover & mutation phase
        Social offspring;
        // (\mu + 2\times\mu)-ES
        Real range_of_objval = mating_pool.back().get_objval() - mating_pool.front().get_objval();

        while (offspring.size() < 2*POP_SIZE) {
            auto father = random_element(mating_pool);
            auto mother = random_element(mating_pool);
            Real P_CROSS = equals(range_of_objval, 0) ? 
                P_CROSS_MIN :
                abs(father.get_objval() - mother.get_objval()) / range_of_objval;
            P_CROSS = 0.8;
            possibly(P_CROSS, [&] {
                auto children = father.crossover(mother);
                offspring.push_back(children.first);
                offspring.push_back(children.second);
            });
        }
        // debug_social(offspring, "Offspring");
        // Survival phase: Elitism + Longest Distance
        population.insert(end(population), all_of(offspring));
        sort(begin(population) + 2 * N_ELITE, end(population));
        population.resize(POP_SIZE);
        // remove duplication?
        if (igen % 50 == 0)
            out << "Generation " << igen << ": " << population[0] << " with " << population[0].get_objval() << '\n';
        if (igen % (NUM_GEN / 10) == 0) {
            cout << "At " << igen << " got " << population[0].get_objval() << '\n';
            cout.flush();
        }
    }
}

int main()
{
    const string TESTSETS[] = {
        "SP"//, "MC"
    };
    freopen("record.log", "a", stdout);
    cout << "\n_____________________________________________\n";
    cout << "NEW BENCHMARK AT: " << get_date_time() << '\n';
    for (auto testset : TESTSETS) {
        string dirpath = "..\\tests\\" + testset;
        for (const auto& entry : fs::directory_iterator(dirpath)) {
            auto path = entry.path();

            if (path.extension() == ".stp") {
                benchmark([&] { read_input(path.string()); }, "Input Reading");
                bool can_do;
                benchmark([&] { can_do = input_preprocessing(); }, "Input Preprocessing");
                if (!can_do) {
                    cout << "Couldn't get all-pair shortest paths. STP instance skipped\n";
                } else {
                    // unit_test();
                    std::ofstream outf("..\\tests_results\\" + testset + "\\" + path.filename().replace_extension(".stp-result").string());
                    benchmark([&] { main_algorithm(outf); }, "Main algorithm");
                    outf.close();
                }
            }
        }
    }
    cout << "=============================================\n";
}