#include "solver.hpp"
#include "testrun.hpp"

/*
PROPOSE: Number of Childs affects greatly! (true)
Propose: Simpler pop reset method for wild is better? (true) AT 2024-02-04 16:48:30 (Inversion)

PROP.: Thus, impose diff on elites and (random) Local Search them is okay? AT 2024-02-04 22:18:08
    Changes: Elite constant, and only Elitism at end of phase (boost run time !!!)

DOUBT: With this great search capability, it can "drill" more given more time to evolution (lifetime factor)

DOUBT: Heuritics for initial population omitted the large search space specially crafted in artificial tests

PROPOSE:
    - Simpler Narrow Detection
    - More flexible & sensible population reset
    
TO-DO: Reset to fixed P_MUT and P_CROSS to showcase and prove Wild Migrant
Traceback: https://github.com/letangphuquy/research-project/blob/4b0bed1570f550719aeb567a91fbaf1cfc3c6474/src/main.cpp

*/ 
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
const Real THRESHOLD = 0.005;
const Real P_CROSS_MIN = 0.25;
const Real P_CROSS_MAX = 0.95;

const int GAP = BSTEP;
int migrate_counter = 0;
int reset_counter = 0;
int last_optimal = 0;
int stuck_counter = 0;

const Real R_REPLACE = 0.2; // should lower if more "seeds" are passed into pool
const Real POLICY_ADAPT = pow(10, 1.0 / NUM_GEN);
Real DIST_POLICY = EULER;
const Real SCALE = EULER; // for Migrant's Local Search
Real diff_avg, diff_threshold;
Real dist_avg;
Real dist_avg_space;
Real dist_avg_last_period;
Real R_CHANGE_SCALE;
Real R_CHANGE_ADAPT;

Social population;
#define the_best population[0].get_objval()
void reset_parameters() {
    dist_avg_last_period = dist_avg_space = distance_sampling(population);
    DIST_POLICY = EULER;
    last_optimal = the_best;
    reset_counter = migrate_counter = stuck_counter = 0;
}

void calculate_stat() {
    ::dist_avg = distance_sampling(population);
    ::diff_avg = dist_avg / num_edges;
    ::diff_threshold = std::max(diff_avg / 2, R_CHANGE);
    ::R_CHANGE_SCALE = R_CHANGE * exp(-SCALE + (dist_avg / dist_avg_space) * SCALE);
    ::R_CHANGE_ADAPT = R_CHANGE * exp(dist_avg / dist_avg_space - 1);
}

void analysis_post(int igen) {
    migrate_counter++;
    DIST_POLICY *= POLICY_ADAPT;
    if (igen % STEP == 0) dist_avg_last_period = dist_avg;
    if (the_best == last_optimal) ++stuck_counter;
    else {
        last_optimal = the_best;
        stuck_counter = 0;
    }
}

void enhance(Solution& sol) {
    possibly(P_MUTATION,
        [&] { sol.local_search(R_CHANGE, 100, true); },
        [&] { sol.local_search(R_CHANGE, 50, true); });
}
void enhance_seeds() {
    for (int i = 0; i < N_ELITE + N_SEED; i++)
        enhance(population[i]);
}

// Attempt to diversify that actually showed good results
bool wildfire(int igen = 0) {
    bool is_stuck = stuck_counter >= MILESTONE;
    bool is_stuck_for_long = stuck_counter >= 3 * STEP;

    bool got_too_narrow = (dist_avg < THRESHOLD);
    bool narrow_too_fast = (dist_avg_space / dist_avg > DIST_POLICY);

    if (got_too_narrow or (migrate_counter >= GAP and (is_stuck or narrow_too_fast))) {
        if (igen) {
            cout << "Migrate at " << igen 
                << ", too narrow? " << got_too_narrow 
                << ", progress = " << migrate_counter << " / " << GAP 
                << ", stuck? " << is_stuck
                << ", too fast?" << narrow_too_fast << '\n';
        }
        migrate_counter = 0;
        for (int i = N_ELITE + N_SEED; i < popsize; i++) {
            possibly(P_MUTATION,
                [&] {
                    Solution outsider;
                    outsider.set_gene(population[i].inversion());
                    population[i] = outsider.crossover(population[i]).first;
                },
                [&] {
                    possibly(0.5, 
                        [&] { population[i].mutate_hard(); },
                        [&] { population[i].mutate(); }
                    );
                }
            );
        }
        calculate_stat();
        return true;
    }
    return false;
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
        // if (wildfire()) {
        //     elitism(population, diff_threshold);
        //     kld_seed(population);
        //     enhance_seeds();
        // }
        auto mating_pool = roulette_wheel_selection(population);
        std::copy_backward(begin(population), begin(population) + N_ELITE + N_SEED, end(mating_pool));
        // Crossover
        Social offspring;
        while (offspring.size() < 2 * popsize) {
            auto& father = random_element(mating_pool);
            auto& mother = random_element(mating_pool);
            Real P_CROSS = equals(dist_avg, 0) ? 
                0 : std::min((Real) 1, pow(father.distance_to(mother) / dist_avg, 1 / EULER));
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

        // Survival & Diversification
        population.insert(end(population), all_of(offspring));
        elitism(population, diff_threshold);
        kld_seed(population);
        enhance_seeds();
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
    SetType included_sets;
    SetType excluded_sets;
    SetType included_tests(TESTS_DEBUG);
    SetType excluded_tests;
    for (int i = 0; i < 5; i++)
    run_tests("IGA", main_algorithm, false, testset_start, 
        included_sets, excluded_sets, included_tests, excluded_tests,
        true);
}
