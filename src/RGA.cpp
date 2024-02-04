#include "solver.hpp"
#include "testrun.hpp"

// Suggested: Add KLD and Elitism at various places

Social population;
#define the_best population[0].get_objval()

int main_algorithm(std::ofstream& out) {
    cout << "Running algorithm...\n";
    auto& population = ::population;
    population = init_population();
    cout << "\tInit population: Done heuristics\n";
    cout.flush();
    for (int igen = 1; igen <= NUM_GEN; igen++) {
        elitism(population);
        kld_seed(population);
        auto mating_pool = roulette_wheel_selection(population);
        std::copy_backward(begin(population), begin(population) + N_ELITE + N_SEED, end(mating_pool));
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
        elitism(population);
        kld_seed(population);
        sort(begin(population) + N_ELITE + N_SEED, end(population));
        remove_duplication(population);
        if (size(population) > POP_SIZE)
            population.resize(POP_SIZE);
        // Report
        if (igen % STEP == 0)
            out << "Generation " << igen << "(" << popsize << "): " << population[0] << " with " << the_best << '\n';
        if (igen % MILESTONE == 0) {
            cout << "At " << igen << " got " << the_best << '\n';
            cout.flush();
        }
    }
    out << "Final " << population[0] << " with " << the_best;
    cout << "Final = " << the_best << '\n';
    return the_best;
}

int main()
{
    MapType testset_start;
    SetType included_sets;
    SetType excluded_sets;
    SetType included_tests(GOOD_TESTS);
    SetType excluded_tests;
    run_tests("RGA", main_algorithm, false, testset_start, 
        included_sets, excluded_sets, included_tests, excluded_tests);
}
