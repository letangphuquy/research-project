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
- Random heur. : make "clusters" then make_span

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

vector<Solution> init_population(void) {
    vector<Solution> pop;
    auto set = heuristics_random_set();
    pop.insert(end(pop), all_of(set));
    set = heuristics_mst_set();
    pop.insert(end(pop), all_of(set));
    set = heuristics_sp_set();
    pop.insert(end(pop), all_of(set));
    while (pop.size() < POP_SIZE)
        pop.push_back(heuristics_random());
    pop.resize(POP_SIZE);
    return pop;
}

vector<Solution> roulette_wheel_selection(vector<Solution>& population) {
    vector<Solution> pool;
    // a big pie with many sectors
    sort(all_of(population));
    // min problem --> take inversion
    int popsize = size(population);

    vector<Real> fitness(popsize);
    for (int i = 0; i < popsize; i++)
        fitness[i] = 1 / population[i].get_objval();
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

void main_algorithm(void) {
    auto population = init_population();
    for (int igen = 0; igen < NUM_GEN; igen++) {
        auto mating_pool = roulette_wheel_selection(population);
        // Crossover & mutation phase
        vector<Solution> offspring;
        // (\mu + 2\times\mu)-ES
        Real range_of_objval = population.back().get_objval() - population.front().get_objval();
        int trial_count = 0;
        while (offspring.size() < 2*POP_SIZE && (trial_count++) <= 2*POP_SIZE) {
            auto father = random_element(mating_pool);
            auto mother = random_element(mating_pool);
            Real P_CROSS = abs(father.get_objval() - mother.get_objval()) / range_of_objval;
            if (random_num(0,1) < P_CROSS) {
                auto children = father.crossover(mother);
                offspring.push_back(children.first);
                offspring.push_back(children.second);
            }

        }
        // Survival phase: Elitism + Longest Distance
        population.insert(end(population), all_of(offspring));
        sort(all_of(population));
        for (int i = 0, it = N_ELITE; i < N_ELITE; i++) {
            int idx = -1, max_dist = -1;
            for (int j = it; j < int(size(population)); j++) {
                if (umax(max_dist, population[i].distance_to(population[j])))
                    idx = j;
            }
            std::swap(population[idx], population[it++]);
        }
        sort(begin(population) + 2 * N_ELITE, end(population));
        population.resize(POP_SIZE);
        if (igen % 50 == 0)
            cout << "Generation " << igen << ": " << population[0] << " with " << population[0].get_objval() << '\n';
    }
}

int main()
{
    const string TESTSETS[] = {
        "SP"//, "MC"
    };
    for (auto testset : TESTSETS) {
        string path = "..\\tests\\" + testset;
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".stp") {
                read_input(entry.path().string());
                unit_test();
                main_algorithm();
                break;
            }
        }
    }
}