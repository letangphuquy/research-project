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

Social population;
#define the_best population[0].get_objval()

const int BSTEP = STEP * 1.5;
const Real THRESHOLD_DIST = 0.002;
const Real P_CROSS_MIN = 0.25;
const Real P_CROSS_MAX = 0.95;

const int GAP = 3 * STEP;
int migrate_counter = 0;
int last_optimal = 0;
int stuck_counter = 0;

Real diff_avg, diff_threshold;
Real dist_avg;
Real dist_avg_space;
Real R_CHANGE_ADAPT;

Real THRESHOLD_QUAL = 0.95;
const Real QUAL_START = 0.85;
const Real QUAL_END = 0.99;
const Real QUAL_RATE = pow(QUAL_END / QUAL_START, 1.0 / NUM_GEN);

void reset_parameters() {
    last_optimal = the_best;
    migrate_counter = stuck_counter = 0;
    THRESHOLD_QUAL = QUAL_START;
}

#define N_KEEP (N_ELITE + N_SEED)

void CONSTANTS() {
    N_ELITE = 1;
    N_SEED_PER_ELITE = 4;
}

int dist[POP_SIZE][POP_SIZE];
int dist_max;
Real distance_measure(Social& pop) {
    int num_tries = popsize * (popsize - 1) / 2;
    umax(num_tries, 1);
    Int sum_distance = 0;
    dist_max = 0;
    for (int i = 0; i < popsize; i++)
        for (int j = i+1; j < popsize; j++) {
            dist[i][j] = dist[j][i] = pop[i].distance_to(pop[j]);
            umax(dist_max, dist[i][j]);
            sum_distance += dist[i][j];
        }
    return (Real) sum_distance / num_tries;
}

// Dynamic Elitist: As far as possible while maintaining desired quality
// Another Disaster idea :(. Not easy to invent new thing
// currently correct for only N_ELITES = 2
// utilize cached distance matrix
bool elitism_tailored(Social& pop, Real min_diff, Real min_quality, bool commit = false) {
    vector<int> indices(popsize);
    iota(all_of(indices), 0);
    sort(all_of(indices), [&] (int i, int j) {
        return pop[i] < pop[j];
    });
    #define id(x) indices[x]
    for (int i = 1, it = 1; i < N_ELITE; i++) {
        int idx = -1;
        for (int j = it; j < int(size(pop)); j++) {
            if (pop[id(0)].get_objval() < min_quality * pop[id(j)].get_objval()) break;
            bool different_enough = true;
            for (int k = 0; k < i; k++)
                different_enough &= dist[id(k)][id(j)] >= min_diff * num_edges;
            if (different_enough) {
                idx = j; break;
            }
        }
        if (idx == -1) return false;
        std::swap(indices[idx], indices[it++]);
    }
    #undef id
    if (commit) {
        Social new_pop;
        for (auto idx : indices) new_pop.push_back(pop[idx]);
        swap(pop, new_pop);
    }
    return true;
}

void dynamic_elitism() {
    Real low = 0, high = 1, diff_max = R_CHANGE;
    const int N_BINARY_ITER = 10;
    for (int it = 0; it < N_BINARY_ITER; it++) {
        Real mid = 0.5 * (low + high);
        if (elitism_tailored(population, mid, THRESHOLD_QUAL))
            diff_max = low = mid;
        else high = mid;
    }
    elitism_tailored(population, diff_max, THRESHOLD_QUAL, true);
}

void calculate_stat() {
    ::dist_avg = distance_measure(population);
    ::diff_avg = dist_avg / num_edges;
    ::diff_threshold = std::max(diff_avg / EULER, R_CHANGE);
    ::R_CHANGE_ADAPT = R_CHANGE * exp(dist_avg / dist_avg_space - 1);
}

void analysis_post(int igen) {
    migrate_counter++;
    THRESHOLD_QUAL *= QUAL_RATE;
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

// Fuel diversity, just in case
bool wildfire(int igen = 0) {
    if (migrate_counter >= GAP && (diff_avg <= THRESHOLD_DIST or dist_max <= R_CHANGE)) {
        migrate_counter = 0;
        for (int i = N_ELITE + N_SEED; i < popsize; i++) {
            possibly(0.5, 
                [&] { population[i].mutate_hard(); },
                [&] { population[i].mutate(); }
            );
        }
        calculate_stat();
        return true;
    }
    return false;
}

int main_algorithm(std::ofstream& out) {
    // CONSTANTS();
    cout << "Running algorithm...\n";
    auto& population = ::population;
    population = init_population();
    cout << "\tInit population: Done heuristics\n";
    cout.flush();
    reset_parameters();
    for (int igen = 1; igen <= NUM_GEN; igen++) {
        analysis_post(igen-1); //emphasize: must go together
        calculate_stat();
        if (igen == 1) dist_avg_space = dist_avg;
        // if (wildfire()) calculate_stat();
        auto mating_pool = roulette_wheel_selection(population);
        // std::copy_backward(begin(population), begin(population) + N_KEEP, end(mating_pool));
        for (int i = popsize-1, j = 0; i >= popsize - N_KEEP; i--, j++) {
            mating_pool[i] = population[j];
            pool_index[i] = j;
        }
        dist_max = 0;
        for (auto i : pool_index) for (auto j : pool_index) umax(dist_max, dist[i][j]);
        // Crossover
        Social offspring;
        while (offspring.size() < 2 * popsize) {
            int pa = random_int(0, popsize-1);
            int ma = random_int(0, popsize-1);
            pa = pool_index[pa]; ma = pool_index[ma];
            #define father population[pa]
            #define mother population[ma]
            Real P_CROSS = equals(dist_avg, 0) ? 
                0 : std::min((Real) 1, pow(dist[pa][ma] / dist_max, 1 / EULER)) * P_CROSS_MAX;
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
        // dynamic_elitism();
        elitism(population, diff_threshold);
        // elitism(population);
        kld_seed(population);
        enhance_seeds();
        sort(begin(population) + N_KEEP, end(population));
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
    SetType included_sets(SETS_BENCHMARK);
    SetType excluded_sets;
    SetType included_tests;
    SetType excluded_tests;
    for (int i = 0; i < 5; i++) {
        std::cerr << "Hello " << i << '\n';
        run_tests("IGA", main_algorithm, false, testset_start, 
            included_sets, excluded_sets, included_tests, excluded_tests,
            false);
    }
}
