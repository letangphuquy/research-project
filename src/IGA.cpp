#include "solver.hpp"
#include "testrun.hpp"

// PROPOSE: slightly modified Mutation (pick random index instead of permutation) is NOT WORSE

/*
Observation
    It will get to a point where mutation really doesn't change anything
    LS is not good enough for late-phase but is necessary for KLD & Wild Migrants.
    optimize LS instead of abandon it
    It will (may) get to a point where Wild Migration introduces too "raw" solutions that doesn't bring any benefit,
    and so, couldn't get out of the local facet
    In case of initial solutions trapped inside a large space of Local Plateu, like in bipartie instance,
        then would need very large energy to get out,
    etc..
    ==> To solve this specific problem better
        --> Investigate on Đặc tính bài toán, thus need more research and knowledge from exact solver's approach?
    For now:
        Apply suggested technique in literature (GECCO)
        Modulize main and develop 3 versions: naive, current & suggested

Reminders and potential to-do:
    - Refer to HLS3 and older works for better neighborhood structure
    - LS must be different from mutate? Hill climbing?
*/

const int BSTEP = STEP * 1.5;

int migrate_counter = 0;
int last_optimal = 0;
int stuck_counter = 0;

const Real MIN_REDUCE_RATE = 1.05; // for adjusting of Distance Sampling measurement
const Real RATIO_REDUCE_RATE = 0.97; // as above
const Real SCALE = EULER; // for Migrant's Local Search
Real diff_avg;
Real dist_avg;
Real dist_avg_space;
Real dist_avg_last_period;
Real DIST_REDUCE_RATE;
Real R_CHANGE_SCALE;
Real R_CHANGE_ADAPT;

Social population;
#define the_best population[0].get_objval()
void reset_parameters() {
    dist_avg_last_period = dist_avg_space = distance_sampling(population);
    DIST_REDUCE_RATE = exp(3);
    last_optimal = the_best;
    migrate_counter = stuck_counter = 0;
}

void calculate_stat() {
    ::dist_avg = distance_sampling(population);
    ::diff_avg = dist_avg / num_edges;
    ::R_CHANGE_SCALE = R_CHANGE * exp(-SCALE + (dist_avg / dist_avg_space) * SCALE);
    ::R_CHANGE_ADAPT = R_CHANGE * exp(dist_avg / dist_avg_space - 1);
}

// Attempt to diversify that actually showed good results
void wild_migration() {
    bool is_stuck = stuck_counter >= BSTEP;
    bool is_stuck_for_long = stuck_counter >= MILESTONE;
    if (is_stuck_for_long) 
        return reset_parameters();

    #define got_too_narrow (dist_avg_last_period / dist_avg > DIST_REDUCE_RATE)
    if (migrate_counter >= BSTEP and !got_too_narrow) {
        DIST_REDUCE_RATE *= RATIO_REDUCE_RATE; 
        umax(DIST_REDUCE_RATE, MIN_REDUCE_RATE);
    }
    if ((migrate_counter >= 25 and got_too_narrow)
    or (migrate_counter >= 10 and is_stuck)) {
        migrate_counter = 0;
        elitism(population, diff_avg);
        int n_replace = R_REPLACE * popsize;
        if (is_stuck) n_replace *= 2;
        for (int _ = 0; _ < n_replace; _++) {
            int idx = random_int(N_ELITE, popsize - 1); // shifted
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

void analysis_post(int igen) {
    if (igen % STEP == 0) dist_avg_last_period = dist_avg;
    if (the_best == last_optimal) ++stuck_counter;
    else {
        last_optimal = the_best;
        stuck_counter = 0;
    }
}

int main_algorithm(std::ofstream& out) {
    cout << "Running algorithm...\n";
    auto& population = ::population;
    population = init_population();
    cout << "\tInit population: Done heuristics\n";
    cout.flush();
    reset_parameters();
    for (int igen = 1; igen <= NUM_GEN; igen++) {
        // Diversification
        analysis_post(igen-1); //emphasize: must go together
        calculate_stat();
        wild_migration();
        // elitism(population, diff_avg);
        elitism(population);
        kld_seed(population);
        auto mating_pool = roulette_wheel_selection(population);
        std::copy_backward(begin(population), begin(population) + N_ELITE + N_SEED, end(mating_pool));
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
        // Mutation
        for (auto &child : offspring)
            possibly(P_MUTATION, [&] { child.mutate(R_CHANGE_ADAPT); });
        for (auto &child : offspring) // there maybe a genius?
            possibly(P_MUTATION, [&] { child.local_search(R_CHANGE_ADAPT, 30); });

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
    for (auto& citizen : population)
        citizen.local_search(R_CHANGE, 100, true);
    sort(all_of(population));
    out << "Final " << population[0] << " with " << the_best;
    cout << "Final = " << the_best << '\n';
    return the_best;
}

int main()
{
    MapType testset_start;
    SetType excluded_tests;
    SetType included_tests;
    run_tests("IGA", main_algorithm, false, testset_start, excluded_tests, included_tests);
}
