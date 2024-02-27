#include "solver.hpp"
#include "testrun.hpp"

// Naive Algorithm: Simple GA without any new technique,

Social population;
#define best_value (population[0].get_objval() + added_cost)

int main_algorithm(std::ofstream& out) {
    cout << "Running algorithm...\n";
    auto& population = ::population;
    population = init_population();
    cout << "\tInit population: Done heuristics\n";
    cout.flush();
    for (int igen = 1; igen <= NUM_GEN; igen++) {
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

        // Survival
        population.insert(end(population), all_of(offspring));
        sort(all_of(population));
        remove_duplication(population);
        if (size(population) > POP_SIZE)
            population.resize(POP_SIZE);
        // Report
        if (DEBUG_MODE) {
            if (igen % STEP == 0)
                out << "Generation " << igen << "(" << popsize << "): " << population[0] << " with " << best_value << '\n';
        }
        if (igen % MILESTONE == 0)
            cout << "At " << igen << " got " << best_value << '\n';
    }
    out << "Final " << population[0] << " with " << best_value;
    cout << "Final = " << best_value << '\n';
    return best_value;
}

int main()
{
    MapType testset_start;
    SetType included_sets;
    SetType excluded_sets;
    SetType included_tests(TESTS_DEBUG);
    SetType excluded_tests;
    for (int i = 0; i < 5; i++) {
        run_tests("SGA", main_algorithm, false, testset_start, 
            included_sets, excluded_sets, included_tests, excluded_tests,
            false);
    }
}
