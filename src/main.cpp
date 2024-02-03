#include "template.hpp"
#include "input.hpp"
#include "solution.hpp"
#include "unit_test.hpp"
#include <filesystem>
#include <map>
namespace fs = std::filesystem;

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

// Assumes all objective value are positive
vector<int> pool_index;
Social roulette_wheel_selection(Social& population, bool is_minimization = true) {
    pool_index.clear();
    Social pool;
    // a big pie with many sectors
    sort(all_of(population));
    // min problem --> take inversion
    int popsize = size(population);

    vector<Real> fitness(popsize);
    for (int i = 0; i < popsize; i++)
        fitness[i] = is_minimization ?
            ((Real) 1 / population[i].get_objval()) : population[i].get_objval();
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
        pool_index.push_back(it);
    }
    return pool;
}

void elitism(Social& pop) {
    sort(all_of(pop));
    for (int i = 1, it = 1; i < N_ELITE; i++) {
        int idx = -1;
        for (int j = it; j < int(size(pop)); j++) {
            bool different_enough = true;
            for (int k = 0; k < i; k++)
                different_enough &= (pop[k].difference(pop[j]) > R_CHANGE);
            if (different_enough) {
                idx = j; break;
            }
        }
        if (idx == -1) break;
        std::swap(pop[idx], pop[it++]);
    }
}

void kld_seed(Social& pop) {
    elitism(pop);
    for (int i = 0, it = N_ELITE; i < N_ELITE; i++) {
        for (int _ = 0; _ < N_SEED_PER_ELITE; _++) {
            int idx = -1, max_dist = -1;
            for (int j = it; j < int(size(pop)); j++) {
                // maximizes distance to elite AND Previous Seed
                int sum = pop[j].distance_to(pop[i]);
                for (int k = it-1; k >= it - _; k--) 
                    sum += pop[j].distance_to(pop[k]);
                if (umax(max_dist, sum)) idx = j;
            }
            std::swap(pop[idx], pop[it++]);
        }
    }
}

void remove_duplication(Social& pop) {
    sort(all_of(pop));
    pop.erase(std::unique(all_of(pop)), end(pop));
}

Real distance_sampling(Social& pop) {
    int num_tries = pop.size() * log2(pop.size());
    umax(num_tries, 1);
    Int sum_distance = 0;
    for (int _ = 0; _ < num_tries; _++) {
        auto u = random_element(pop);
        auto v = random_element(pop);
        sum_distance += u.distance_to(v);
    }
    return sum_distance / num_tries;
}

void main_algorithm(std::ofstream& out) {
    cout << "Running algorithm...\n";
    auto population = init_population();
    cout << "\tInit population: Done heuristics\n";
    cout.flush();

    const Real MIN_REDUCE_RATE = 1.05; // for adjusting of Distance Sampling measurement
    const Real RATIO_REDUCE_RATE = 0.97; // as above
    const Real SCALE = EULER; // for Migrant's Local Search

    // bool N_SMALL = num_nodes < 50;
    // NUM_GEN = (N_SMALL) ? 100 : 1000;
    const int STEP = std::min(50, NUM_GEN / 20);
    const int BSTEP = STEP * 1.5;
    const int MILESTONE = NUM_GEN / 10; // for observing

    Real dist_avg_space = distance_sampling(population);
    Real dist_avg_last_period = dist_avg_space;
    Real DIST_REDUCE_RATE = exp(3);
    int migrate_counter = 0;
    int last_optimal = 0;
    int stuck_counter = 0;

    auto reset_parameters = [&] {
        dist_avg_last_period = dist_avg_space = distance_sampling(population);
        DIST_REDUCE_RATE = exp(3);
        last_optimal = population[0].get_objval();
        migrate_counter = stuck_counter = 0;
    };
    int lifetime = 1;
    if (!IS_MEDIUM_INSTANCE(num_edges)) lifetime = 2;
    for (int igen = 1; igen <= NUM_GEN * lifetime; igen++) {
        // if (N_SMALL && igen > 100) break;
        #define popsize (population.size())
        // cout << "G " << igen << '\n';
        // debug_social(population, "Population");
        Real dist_avg = distance_sampling(population);
        Real R_CHANGE_SCALE = R_CHANGE * exp(-SCALE + (dist_avg / dist_avg_space) * SCALE);

        bool is_stuck = stuck_counter >= BSTEP;
        bool is_stuck_for_long = stuck_counter >= MILESTONE;
        // bool is_stucked_for_too_long = stuck_counter >= 2 * MILESTONE;
        // if (is_stucked_for_too_long) {
        //     elitism(population);
        //     for (int i = N_ELITE + N_SEED; i < popsize; i++)
        //         population[i].mutate_hard();
        // }
        if (is_stuck_for_long) {
            reset_parameters();
        } else
        // "Wild Migration": random outsiders
        // if (0)
        {
            // Idea: Maintaining average distance?
            // currently: only throttling down, needs to increase up back?
            #define got_too_narrow (dist_avg_last_period / dist_avg > DIST_REDUCE_RATE)
            if (migrate_counter >= BSTEP and !got_too_narrow) {
                DIST_REDUCE_RATE *= RATIO_REDUCE_RATE; 
                umax(DIST_REDUCE_RATE, MIN_REDUCE_RATE);
            }
            if ((migrate_counter >= 25 and got_too_narrow)
            or (migrate_counter >= 10 and is_stuck)) {
                // cout << "\tReplace: " << igen << ", distance " << DIST_REDUCE_RATE << " ";
                // PRINT(cout, got_too_narrow)
                // PRINTLN(cout, is_stucked_for_long)
                migrate_counter = 0;
                elitism(population);
                int n_replace = R_REPLACE * popsize;
                if (is_stuck) n_replace *= 2;
                for (int _ = 0; _ < n_replace; _++) {
                    int idx = random_num(N_ELITE, popsize - 1); // shifted
                    auto& individual = population[idx];
                    Solution outsider = heuristics_random();
                    auto get = individual.crossover(outsider);
                    possibly(0.5, 
                        [&] { individual = random_element(Social({get.first, get.second}));},
                        [&] {
                            for (auto child : {get.first, get.second})
                                if (child.get_objval() < individual.get_objval()) individual = child;
                        }
                    );
                    possibly(P_MUTATION,
                        [&] { individual.local_search(R_CHANGE_SCALE, 100); },
                        [&] { individual.local_search(R_CHANGE_SCALE, 10); }
                    );
                }
            }
            ++migrate_counter;
        }
        kld_seed(population);
        auto mating_pool = roulette_wheel_selection(population);
        std::copy_backward(begin(population), begin(population) + N_ELITE + N_SEED, end(mating_pool));
        // debug_social(mating_pool, "Pool");
        // Crossover
        Social offspring;
        while (population.size() + offspring.size() < 2 * POP_SIZE) {
            auto& father = random_element(mating_pool);
            auto& mother = random_element(mating_pool);
            Real P_CROSS = equals(dist_avg, 0) ? 
                0 : std::min((Real) 1, pow(father.distance_to(mother) / dist_avg, 0.3)) * P_CROSS_MAX;
            umax(P_CROSS, P_CROSS_MIN);
            possibly(P_CROSS, [&] {
                auto children = father.crossover(mother);
                offspring.push_back(children.first);
                offspring.push_back(children.second);
            });
        }
        // debug_social(offspring, "Offspring");
        // Mutation
        Real R_CHANGE_ADAPT = R_CHANGE * exp(dist_avg / dist_avg_space - 1);
        for (auto &child : offspring)
            possibly(P_MUTATION, [&] { child.mutate(R_CHANGE_ADAPT); });
        for (auto &child : offspring) // there maybe a genius?
            possibly(P_MUTATION, [&] { child.local_search(R_CHANGE_ADAPT, 30); });

        // Survival phase: Fittest
        population.insert(end(population), all_of(offspring));
        kld_seed(population);
        sort(begin(population) + N_ELITE + N_SEED, end(population));
        remove_duplication(population); // considers removal of "too similar" elements
        if (size(population) > POP_SIZE)
            population.resize(POP_SIZE);
        // Post- Anaylysis
        if (igen % STEP == 0) {
            dist_avg_last_period = dist_avg;
            out << "Generation " << igen << "(" << popsize << "): " << population[0] << " with " << population[0].get_objval() << '\n';
        }
        if (igen % MILESTONE == 0) {
            cout << "At " << igen << " got " << population[0].get_objval() << '\n';
            cout.flush();
        }
        if (population[0].get_objval() == last_optimal) ++stuck_counter;
        else {
            last_optimal = population[0].get_objval();
            stuck_counter = 0;
        }
    }
}

int main()
{
    const string TESTSETS[] = {
        "B",
        "C", 
        "D", 
        "SP", 
        "MC",
        "X",
        "E", 
        "PUC",
        "notestset"
    };
    bool RUN_NEW_TEST_ONLY = false;
    std::map<string, string> startsFromTest;
    // startsFromTest["C"] = "c19";
    // startsFromTest["E"] = "e16";

    freopen("record.log", "a", stdout);

    cout << "\n_____________________________________________\n";
    cout << "NEW BENCHMARK AT: " << get_date_time() << '\n';
    for (auto testset : TESTSETS) {
        string dirpath = "..\\tests\\" + testset;
        string startTest = startsFromTest[testset];
        bool skipped = !startTest.empty();
        for (const auto& entry : fs::directory_iterator(dirpath)) {
            auto path = entry.path();

            if (path.extension() == ".stp") {
                string outf_path = "..\\tests_results\\" + testset 
                    + "\\" + path.filename().replace_extension(".stp-result").string();
                if (RUN_NEW_TEST_ONLY && std::filesystem::exists(outf_path)) {
                    cout << "Already have result. Skipping test\n";
                    continue;
                }
                if (path.filename().replace_extension() == startTest) skipped = false;
                if (skipped) {
                    cout << path.filename() << " skipped\n";
                    continue;
                }
                benchmark([&] { read_input(path.string()); }, "Input Reading");
                bool can_do;
                benchmark([&] { can_do = input_preprocessing(); }, "Input Preprocessing");
                if (!can_do) {
                    cout << "Couldn't get all-pair shortest paths. STP instance skipped\n";
                } else {
                    // unit_test();
                    std::ofstream outf(outf_path);
                    benchmark([&] { main_algorithm(outf); }, "Main algorithm");
                    outf.close();
                }
            }
        }
    }
    cout << "=============================================\n";
}
