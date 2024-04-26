#include "solver.hpp"
#include "testrun.hpp"

/*
HGA, but nerfed more to MATCH THE STATED IN PAER
SGA:
+ Jaccard distance
+ Dynamic Crossover
+ 2 Longest Distance
*/
Social population;
#define best_value (population[0].get_objval() + added_cost)
#define N_KEEP (N_ELITE + N_SEED)

void CONSTANTS() {
    N_ELITE = 3;
    N_SEED_PER_ELITE = 3;
}

const Real P_CROSS_MIN = 0.25;
const Real P_CROSS_MAX = 0.95;

Social& algorithm_initialization() {
    auto& pop = ::population;
    pop = init_pop_mixed();
    if (VERBOSE_LOG) cout << "\tInit population: Done heuristics\n";
    return pop;
}

int main_algorithm(std::ofstream& out) {
    CONSTANTS();
    if (VERBOSE_LOG) cout << "Running algorithm...\n";
    auto population = algorithm_initialization(); 
    cout.flush();
    
    // Mild Divergence
    vector<int> record(NUM_GEN + 5, INF);
    record[0] = best_value;

    for (int igen = 1; igen <= NUM_GEN; igen++) {
        // RGA & IGA 's simpler scheme
        roulette_wheel_selection(population);
        for (int i = 0; i < N_KEEP; i++) 
            pool_index[size(pool_index)-1 - i] = i;
        // std::copy_backward(begin(population), begin(population) + N_KEEP, end(mating_pool));
        // Crossover
        auto mating_pool = roulette_wheel_selection(population);
        Social offspring;
        while (offspring.size() < 2 * POP_SIZE) {
            auto& father = random_element(mating_pool);
            auto& mother = random_element(mating_pool);
            Real P_CROSS = 0.95; 
            possibly(P_CROSS, [&] {
                auto children = father.crossover(mother);
                offspring.push_back(children.first);
                offspring.push_back(children.second);
            });
        }
        // Mutation
        for (auto &child : offspring)
            possibly(P_MUTATION, [&] { child.mutate(R_CHANGE); });
        // Survival & Diversification
        population.insert(end(population), all_of(offspring));
        elitism(population);
        kld_seed(population);
        sort(begin(population) + N_KEEP, end(population)); // CHC Adaptive
        remove_duplication(population);
        if (size(population) > POP_SIZE)
            population.resize(POP_SIZE);
        
        // Post-analysis
        record[igen] = best_value;
            
        // Report
        if (DEBUG_MODE) {
            if (igen % STEP == 0)
                out << "Generation " << igen << "(" << popsize << "): " << population[0] << " with " << best_value << '\n';
        }
        if (VERBOSE_LOG) {
            if (igen % MILESTONE == 0)
                cout << "At " << igen << " got " << best_value << std::endl;
        }
    }
    report_local_search();
    out << "Final " << population[0] << " with " << best_value;
    
    cout << "Summary: ";
    for (int i = 1; i <= NUM_GEN; i++) cout << record[i] << ' ';
    cout << '\n';
    
    cout << "Final = " << best_value << '\n';
    return best_value;
}

int main()
{
    MapType testset_start;
    SetType included_sets(SETS_MODEST);
    SetType excluded_sets;
    SetType included_tests;
    SetType excluded_tests;
    for (int i = 0; i < 10; i++) {
        run_tests("HGA-0", 
            main_algorithm, 
            false, 
            testset_start, 
            // i % 2 == 0 ? SETS_BENCHMARK_ADDITIONAL : SETS_BENCHMARK, 
            included_sets,
            excluded_sets, 
            included_tests, 
            excluded_tests,
            true,
            false
        );
    }
}
