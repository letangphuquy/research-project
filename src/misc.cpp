#include "solver.hpp"
#include "testrun.hpp"

/*
TRY:
    Dynamic Mutation Ratio based on
        |E| / |V|
*/

Social population;
#define best_value (population[0].get_objval() + added_cost)

const Real P_CROSS_MIN = 0.25;
const Real P_CROSS_MAX = 0.95;

const Real DIFF_MIN = 0.005;
Real RATIO_LOW;
Real RATIO_BIG;
Real diff_avg, diff_threshold;
Real dist_avg;
Real dist_avg_space;
#define R_CHANGE 0.03
Real R_CHANGE_ADAPT;

#define N_KEEP (N_ELITE + N_SEED)

void CONSTANTS() {
    RATIO_LOW = pow((Real) num_edges / num_nodes, 1 / pow(EULER, PI / 2));
    RATIO_BIG = pow((Real) num_edges / num_nodes, 1 / EULER);
    N_ELITE = 2;
    N_SEED_PER_ELITE = 2;
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

void calculate_stat() {
    dist_avg = distance_measure(population);
    diff_avg = dist_avg / num_edges;
    diff_threshold = std::max(diff_avg / 2, DIFF_MIN);
    R_CHANGE_ADAPT = R_CHANGE * exp(dist_avg / dist_avg_space - 1);
}

// repeated local search until rendered ineffective
int enhance(Solution& sol, Real rate, int MAX_ITER) {
    const int BATCH_SIZE = 10;
    int recall = 0, num_calls = 0;
    do {
        recall = sol.augment(RATIO_LOW * R_CHANGE, BATCH_SIZE, true);
        num_calls += BATCH_SIZE;
    } while (recall >= rate * BATCH_SIZE && num_calls < MAX_ITER);
    return num_calls;
}

void enhance_seeds() {
    const int QTY = 100;
    for (int i = 0; i < N_ELITE + N_SEED; i++) {
        int rem = QTY;
        rem -= enhance(population[i], 0.5, QTY);
        population[i].augment(RATIO_LOW * R_CHANGE_ADAPT, rem);
    }
}

int main_algorithm(std::ofstream& out) {
    CONSTANTS();
    cout << "Running algorithm...\n";
    PRINTLN(cout, RATIO_LOW);
    auto& population = ::population;
    population = init_population();
    cout << "\tInit population: Done heuristics\n";
    cout.flush();
    dist_avg_space = distance_measure(population);
    for (int igen = 1; igen <= NUM_GEN; igen++) {
        calculate_stat();
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
            possibly(P_MUTATION, [&] { 
                child.mutate(RATIO_BIG * R_CHANGE); 
                enhance(child, 0.7, 150);
            });

        // Survival & Diversification
        population.insert(end(population), all_of(offspring));
        elitism(population, diff_threshold);
        if (!kld_seed(population, 0.95)) 
            kld_seed(population, 0.8);
        enhance_seeds();
        sort(begin(population) + N_KEEP, end(population));
        remove_duplication(population);
        if (size(population) > POP_SIZE)
            population.resize(POP_SIZE);
        // Report
        if (DEBUG_MODE) {
            if (igen % STEP == 0)
                out << "Generation " << igen << "(" << popsize << "): " << population[0] << " with " << best_value << '\n';
        }
        if (igen % MILESTONE == 0) {
            cout << "At " << igen << " got " << best_value << '\n';
            cout.flush();
        }
    }
    for (int i = 0; i < N_KEEP; i++) 
        enhance(population[i], 0.1, 500);
    sort(all_of(population));
    out << "Final " << population[0] << " with " << best_value;
    cout << "Final = " << best_value << '\n';
    cout << "LS success rate: " << CNT_LS_SUCC << " / " << CNT_LS_CALL 
        << ": " << ((Real) CNT_LS_SUCC / CNT_LS_CALL) << '\n';
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
        run_tests("IGA", main_algorithm, false, testset_start, 
            included_sets, excluded_sets, included_tests, excluded_tests,
            true);
    }
}
/// IGA_F: FULL CIRCLE
#include "solver.hpp"
#include "testrun.hpp"

/*
IGA with Fitness Sharing
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

// Fitness sharing
Real f1(Real x);
const Real CUTPOINT = 0.12;
const Real Y_CUT = f1(CUTPOINT);
const Real ALPHA = log(1 - Y_CUT) / log(CUTPOINT);
// Fast Decrease
Real f1(Real x) { return 1 - pow(5*x, 1 / (EULER * PHI)); }
// Slow Decrease
Real sharing_function(Real x) { // diff as x
    if (x-EPS <= CUTPOINT) return f1(x);
    return 1 - pow(std::min((Real) 1, x), ALPHA);
}
// Dynamic P_CROSS, Mutation's R_CHANGE and imposed DIFF on Elitism
const Real DIFF_MIN = PI / 1000;
#define DEFAULT_COEF (2.0 * num_nodes / num_edges * EULER)
Real COEF = DEFAULT_COEF; // |V| / |E| then e for fun (for extra space)
Real R_CHANGE_ADAPT;

int dist[POP_SIZE][POP_SIZE];
Real diff[POP_SIZE][POP_SIZE];
Real diff_avg, diff_threshold;
Real dist_avg;
Real dist_avg_space;
int diameter;
Real difference(int i, int j);
void distance_measure(Social& pop) {
    int num_pairs = popsize * (popsize - 1) / 2;
    umax(num_pairs, 1);
    Int sum_distance = 0;
    Real sum_diff = 0;
    for (int i = 0; i < popsize; i++)
        for (int j = i+1; j < popsize; j++) {
            dist[i][j] = dist[j][i] = pop[i].distance_to(pop[j]);
            diff[i][j] = diff[j][i] = difference(i,j);
            umax(diameter, dist[i][j]);
            sum_distance += dist[i][j];
            sum_diff += diff[i][j];
        }
    diff_avg = sum_diff / num_pairs;
    dist_avg = sum_distance / num_pairs;
}

// inspired by ML's set difference
Real difference(Solution& a, Solution& b) {
    // dA + comm = A; dB + comm = B
    // dist = dA + dB;
    // dist + 2 * comm = A + B
    int common = (a.count_edges() + b.count_edges() - a.distance_to(b)) / 2;
    return (Real) a.distance_to(b) / (a.count_edges() + b.count_edges() - common);
}
Real difference(int i, int j) {
    int a = population[i].count_edges();
    int b = population[j].count_edges();
    int comm = (a + b - dist[i][j]) / 2;
    return (Real) dist[i][j] / (a + b - comm);
}

void calculate_stat() {
    distance_measure(population);
    diff_threshold = std::max(diff_avg / 2, DIFF_MIN);
    // diff_threshold = std::max(diff_avg, R_CHANGE);
    R_CHANGE_ADAPT = R_CHANGE * exp(dist_avg / dist_avg_space - 1);
}

// repeated local search until rendered ineffective
const int BATCH_SIZE = 10;
int enhance(Solution& sol, Real rate, int MAX_ITER) {
    int recall = 0, num_calls = 0;
    do {
        recall = sol.augment(COEF * R_CHANGE, BATCH_SIZE, true);
        num_calls += BATCH_SIZE;
    } while (recall >= rate * BATCH_SIZE && num_calls < MAX_ITER);
    return num_calls;
}

void training(int num_iter) {
    const int BUCKET_SIZE = 20;
    const Real R_TOP = 0.1;
    for (int it = 0; it < num_iter; it += BUCKET_SIZE) {
        int n_top = popsize * R_TOP;
        for (int i = 0; i < n_top; i++) {
            int rem = BUCKET_SIZE - enhance(population[i], 0.3, BUCKET_SIZE);
            population[i].augment(COEF * R_CHANGE_ADAPT, rem);
        }
        for (int i = n_top; i < popsize; i++)
            enhance(population[i], 0.5, BUCKET_SIZE);
        sort(all_of(population));
    }
}

void elitism_v2(Social& pop, Real min_diff, Real min_quality = 0.95) {
    sort(all_of(pop));
    for (int i = 1, it = 1; i < N_ELITE; i++) {
        int idx = -1, furthest = -1;
        Real max_sum = 0;
        for (int j = it; j < int(size(pop)); j++) {
            if (pop[j].get_objval() < min_quality * pop[i].get_objval()) break;
            bool different_enough = true;
            Real sum_diff = 0;
            for (int k = 0; k < i; k++) {
                Real diff_kj = difference(pop[k], pop[j]);
                different_enough &= (diff_kj - EPS >= min_diff);
                sum_diff += diff_kj;
            }
            if (furthest == -1 or umax(max_sum, sum_diff)) furthest = j;
            if (different_enough) {
                idx = j; break;
            }
        }
        if (idx == -1) idx = furthest;
        if (idx == -1) break;
        std::swap(pop[idx], pop[it++]);
        sort(begin(pop) + it, end(pop)); // for line 111 to be correct
    }
}

void kld_seed_v2(Social& pop) {
    for (int i = 0, it = N_ELITE; i < N_ELITE; i++) {
        for (int _ = 0; _ < N_SEED_PER_ELITE; _++) {
            int idx = -1;
            Real max_diff = -1;
            for (int j = it; j < int(size(pop)); j++) {
                // maximizes difference to elite AND Previous Seed
                Real sum_diff = difference(pop[i], pop[j]);
                for (int k = it-1; k >= it - _; k--) 
                    sum_diff += difference(pop[j], pop[k]);
                if (umax(max_diff, sum_diff)) idx = j;
            }
            std::swap(pop[idx], pop[it++]);
        }
    }
}

Social& algorithm_initialization() {
    auto& pop = ::population;
    pop = init_population();
    cout << "\tInit population: Done heuristics\n";
    distance_measure(pop);
    dist_avg_space = dist_avg;
    return pop;
}


int main_algorithm(std::ofstream& out) {
    CONSTANTS();
    cout << "Running algorithm...\n";
    auto population = algorithm_initialization(); 
    ::COEF = DEFAULT_COEF;
    cout.flush();
    
    // Convergence Check - Soft Restart(s)
    int DIVERGE_GAP = 15;
    int TRAINING_GAP = 4;
    const Real DIVERGE_RATE = 0.5;
    int no_improve_count = 0;
    int diverge_count = 0;
    int last_diverge = 0;
    int last_optimal = best_value;
    bool RESET_STRATEGY = true;

    auto hard_reset = [&] (int igen) {
        cout << " || Hard reset at " << igen << '\n';
        Social keeps;
        for (int i = 0; i < N_ELITE; i++) keeps.push_back(population[i]);
        sp_handler.calc_for(graph); // new SP order
        population = algorithm_initialization();
        for (int i = 0; i < N_ELITE; i++)
            std::swap(population[popsize-i-1], keeps[i]);
        elitism_v2(population, diff_avg); // without this line, there's reset bug
    };

    for (int igen = 1; igen <= NUM_GEN; igen++) {
        calculate_stat();
        // Fitness sharing
        vector<Real> fitness;
        for (auto p_i : population) fitness.push_back(p_i.get_objval());
        for (int i = 0; i < popsize; i++) {
            Real coef = 0;
            for (int j = 0; j < popsize; j++)
                coef += sharing_function(difference(i,j));
            fitness[i] /= coef;
        }
        auto mating_pool = roulette_wheel_selection(population, fitness); // considers increasing pool size
        // std::copy_backward(begin(population), begin(population) + N_KEEP, end(mating_pool));
        // CHC Conservatism
        for (int i = 0, last = popsize; i < N_ELITE; i++) {
            pool_index[--last] = i;
            mating_pool[last] = population[i];
        }
        Real max_diff = 0;
        for (auto i : pool_index) for (auto j : pool_index) umax(max_diff, diff[i][j]);
        // Crossover
        Social offspring;
        while (offspring.size() < 2 * popsize) {
            int pa = random_int(0, popsize-1);
            int ma = random_int(0, popsize-1);
            pa = pool_index[pa]; ma = pool_index[ma];
            #define father population[pa]
            #define mother population[ma]
            Real P_CROSS = equals(dist_avg, 0) ? 
                0 : std::min((Real) 1, pow(diff[pa][ma] / max_diff, 1 / EULER)) * P_CROSS_MAX;
            umax(P_CROSS, P_CROSS_MIN);
            possibly(P_CROSS, [&] {
                auto children = father.crossover(mother);
                offspring.push_back(children.first);
                offspring.push_back(children.second);
            });
        }
        // Mutation
        for (auto &child : offspring)
            possibly(P_MUTATION, [&] { child.mutate(COEF * R_CHANGE); });
        for (auto &child : offspring) // there maybe a genius?
            possibly(P_MUTATION, [&] { child.augment(COEF * R_CHANGE_ADAPT, 30); });

        // Survival & Diversification
        population.insert(end(population), all_of(offspring));
        elitism_v2(population, diff_threshold);
        kld_seed_v2(population);
        sort(begin(population) + N_KEEP, end(population)); // CHC Adaptive
        remove_duplication(population);
        if (size(population) > POP_SIZE)
            population.resize(POP_SIZE);
        
        // "Soft" Restarts 
        bool unchanged = best_value >= last_optimal;
        if (unchanged) ++no_improve_count;
        else { last_optimal = best_value; no_improve_count = 0; }
        if (RESET_STRATEGY) {
            if (no_improve_count >= 24) {
                hard_reset(igen);
                RESET_STRATEGY = false;
            }
            #define diverge_gap (igen - last_diverge)
            if (unchanged && diverge_gap >= DIVERGE_GAP) {
                last_diverge = igen;
                if ((++diverge_count) % 3 == 0) {
                    hard_reset(igen);
                }
                else {
                    // Phase 1: Diverge
                    cout << "\tDiverged at " << igen << '\n';
                    Real V_E_RATIO = (Real) 2.0 * num_nodes / num_edges;
                    for (int i = N_KEEP; i < popsize; i++)
                        population[i].mutate(V_E_RATIO * DIVERGE_RATE);
                }
                if (diverge_count >= 4) {
                    DIVERGE_GAP = 18;
                    TRAINING_GAP = 7;
                }
            }
            if (unchanged && diverge_gap > 0 && diverge_gap % TRAINING_GAP == 0) {
                CNT_LS_CALL = CNT_LS_SUCC = 0;
                training(80); // Chronical Training
                report_local_search();
            }
        }
            
        // Report
        if (DEBUG_MODE) {
            if (igen % STEP == 0)
                out << "Generation " << igen << "(" << popsize << "): " << population[0] << " with " << best_value << '\n';
        }
        if (igen % MILESTONE == 0)
            cout << "At " << igen << " got " << best_value << std::endl;
    }
    CNT_LS_CALL = CNT_LS_SUCC = 0;
    training(500);
    out << "Final " << population[0] << " with " << best_value;
    cout << "Final = " << best_value << '\n';
    report_local_search();
    return best_value;
}

int main()
{
    MapType testset_start;
    SetType included_sets;
    SetType excluded_sets;
    SetType included_tests(TESTS_DEBUG);
    SetType excluded_tests;
    for (int i = 0; i < 10; i++) {
        run_tests("IGA_F", 
            main_algorithm, 
            false, 
            testset_start, 
            included_sets, 
            excluded_sets, 
            TESTS_DEBUG, 
            excluded_tests,
            true);
    }
    for (int i = 0; i < 10; i++) {
        run_tests("IGA_F", 
            main_algorithm, 
            false, 
            testset_start, 
            included_sets, 
            excluded_sets, 
            TESTS_STRONG, 
            excluded_tests,
            true);
    }
}
/// IGA_F: HEY JUDE
#include "solver.hpp"
#include "testrun.hpp"

/*
IGA with Fitness Sharing
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

// Fitness sharing
Real f1(Real x);
const Real CUTPOINT = 0.12;
const Real Y_CUT = f1(CUTPOINT);
const Real ALPHA = log(1 - Y_CUT) / log(CUTPOINT);
// Fast Decrease
Real f1(Real x) { return 1 - pow(5*x, 1 / (EULER * PHI)); }
// Slow Decrease
Real sharing_function(Real x) { // diff as x
    if (x-EPS <= CUTPOINT) return f1(x);
    return 1 - pow(std::min((Real) 1, x), ALPHA);
}
// Dynamic P_CROSS, Mutation's R_CHANGE and imposed DIFF on Elitism
const Real DIFF_MIN = PI / 1000;
#define DEFAULT_COEF (2.0 * num_nodes / num_edges * EULER)
Real COEF = DEFAULT_COEF; // |V| / |E| then e for fun (for extra space)
Real R_CHANGE_ADAPT;

int dist[POP_SIZE][POP_SIZE];
Real diff[POP_SIZE][POP_SIZE];
Real diff_avg, diff_threshold;
Real dist_avg;
Real dist_avg_space;
int diameter;
Real difference(int i, int j);
void distance_measure(Social& pop) {
    int num_pairs = popsize * (popsize - 1) / 2;
    umax(num_pairs, 1);
    Int sum_distance = 0;
    Real sum_diff = 0;
    for (int i = 0; i < popsize; i++)
        for (int j = i+1; j < popsize; j++) {
            dist[i][j] = dist[j][i] = pop[i].distance_to(pop[j]);
            diff[i][j] = diff[j][i] = difference(i,j);
            umax(diameter, dist[i][j]);
            sum_distance += dist[i][j];
            sum_diff += diff[i][j];
        }
    diff_avg = sum_diff / num_pairs;
    dist_avg = sum_distance / num_pairs;
}

// inspired by ML's set difference
Real difference(Solution& a, Solution& b) {
    // dA + comm = A; dB + comm = B
    // dist = dA + dB;
    // dist + 2 * comm = A + B
    int common = (a.count_edges() + b.count_edges() - a.distance_to(b)) / 2;
    return (Real) a.distance_to(b) / (a.count_edges() + b.count_edges() - common);
}
Real difference(int i, int j) {
    int a = population[i].count_edges();
    int b = population[j].count_edges();
    int comm = (a + b - dist[i][j]) / 2;
    return (Real) dist[i][j] / (a + b - comm);
}

void calculate_stat() {
    distance_measure(population);
    diff_threshold = std::max(diff_avg / 2, DIFF_MIN);
    // diff_threshold = std::max(diff_avg, R_CHANGE);
    R_CHANGE_ADAPT = R_CHANGE * exp(dist_avg / dist_avg_space - 1);
}

// repeated local search until rendered ineffective
const int BATCH_SIZE = 10;
int enhance(Solution& sol, Real rate, int MAX_ITER) {
    int recall = 0, num_calls = 0;
    do {
        recall = sol.augment(COEF * R_CHANGE, BATCH_SIZE, true);
        num_calls += BATCH_SIZE;
    } while (recall >= rate * BATCH_SIZE && num_calls < MAX_ITER);
    return num_calls;
}

void training(int num_iter) {
    const int BUCKET_SIZE = 20;
    const Real R_TOP = 0.1;
    for (int it = 0; it < num_iter; it += BUCKET_SIZE) {
        int n_top = popsize * R_TOP;
        for (int i = 0; i < n_top; i++) {
            int rem = BUCKET_SIZE - enhance(population[i], 0.3, BUCKET_SIZE);
            population[i].augment(COEF * R_CHANGE_ADAPT, rem);
        }
        for (int i = n_top; i < popsize; i++)
            enhance(population[i], 0.5, BUCKET_SIZE);
        sort(all_of(population));
    }
}

void elitism_v2(Social& pop, Real min_diff, Real min_quality = 0.95) {
    sort(all_of(pop));
    for (int i = 1, it = 1; i < N_ELITE; i++) {
        int idx = -1, furthest = -1;
        Real max_sum = 0;
        for (int j = it; j < int(size(pop)); j++) {
            if (pop[j].get_objval() < min_quality * pop[i].get_objval()) break;
            bool different_enough = true;
            Real sum_diff = 0;
            for (int k = 0; k < i; k++) {
                Real diff_kj = difference(pop[k], pop[j]);
                different_enough &= (diff_kj - EPS >= min_diff);
                sum_diff += diff_kj;
            }
            if (furthest == -1 or umax(max_sum, sum_diff)) furthest = j;
            if (different_enough) {
                idx = j; break;
            }
        }
        if (idx == -1) idx = furthest;
        if (idx == -1) break;
        std::swap(pop[idx], pop[it++]);
        sort(begin(pop) + it, end(pop)); // for line 111 to be correct
    }
}

void kld_seed_v2(Social& pop) {
    for (int i = 0, it = N_ELITE; i < N_ELITE; i++) {
        for (int _ = 0; _ < N_SEED_PER_ELITE; _++) {
            int idx = -1;
            Real max_diff = -1;
            for (int j = it; j < int(size(pop)); j++) {
                // maximizes difference to elite AND Previous Seed
                Real sum_diff = difference(pop[i], pop[j]);
                for (int k = it-1; k >= it - _; k--) 
                    sum_diff += difference(pop[j], pop[k]);
                if (umax(max_diff, sum_diff)) idx = j;
            }
            std::swap(pop[idx], pop[it++]);
        }
    }
}

Social& algorithm_initialization() {
    auto& pop = ::population;
    pop = init_population();
    cout << "\tInit population: Done heuristics\n";
    distance_measure(pop);
    dist_avg_space = dist_avg;
    return pop;
}


int main_algorithm(std::ofstream& out) {
    CONSTANTS();
    cout << "Running algorithm...\n";
    auto population = algorithm_initialization(); 
    ::COEF = DEFAULT_COEF;
    cout.flush();
    
    // Convergence Check - Soft Restart(s)
    int DIVERGE_GAP = 15;
    int TRAINING_GAP = 4;
    const Real DIVERGE_RATE = 0.5;
    int no_improve_count = 0;
    int diverge_count = 0;
    int reset_count = 0;
    int last_diverge = 0;
    int last_optimal = best_value;
    bool RESET_STRATEGY = true;
    bool TRAINING_STRATEGY = true;

    auto hard_reset = [&] (int igen) {
        last_diverge = igen;
        if (++reset_count >= 2) {
            TRAINING_STRATEGY = false;
            RESET_STRATEGY = false;
        } 
        cout << " || Hard reset at " << igen << '\n';
        Social keeps;
        for (int i = 0; i < N_ELITE; i++) keeps.push_back(population[i]);
        sp_handler.calc_for(graph); // new SP order
        population = algorithm_initialization();
        for (int i = 0; i < N_ELITE; i++)
            std::swap(population[popsize-i-1], keeps[i]);
        elitism_v2(population, diff_avg); // without this line, there's reset bug
    };

    for (int igen = 1; igen <= NUM_GEN; igen++) {
        calculate_stat();
        // Fitness sharing
        vector<Real> fitness;
        for (auto p_i : population) fitness.push_back(p_i.get_objval());
        for (int i = 0; i < popsize; i++) {
            Real coef = 0;
            for (int j = 0; j < popsize; j++)
                coef += sharing_function(difference(i,j));
            fitness[i] /= coef;
        }
        auto mating_pool = roulette_wheel_selection(population, fitness); // considers increasing pool size
        // std::copy_backward(begin(population), begin(population) + N_KEEP, end(mating_pool));
        // CHC Conservatism
        for (int i = 0, last = popsize; i < N_ELITE; i++) {
            pool_index[--last] = i;
            mating_pool[last] = population[i];
        }
        Real max_diff = 0;
        for (auto i : pool_index) for (auto j : pool_index) umax(max_diff, diff[i][j]);
        // Crossover
        Social offspring;
        while (offspring.size() < 2 * popsize) {
            int pa = random_int(0, popsize-1);
            int ma = random_int(0, popsize-1);
            pa = pool_index[pa]; ma = pool_index[ma];
            #define father population[pa]
            #define mother population[ma]
            Real P_CROSS = equals(dist_avg, 0) ? 
                0 : std::min((Real) 1, pow(diff[pa][ma] / max_diff, 1 / EULER)) * P_CROSS_MAX;
            umax(P_CROSS, P_CROSS_MIN);
            possibly(P_CROSS, [&] {
                auto children = father.crossover(mother);
                offspring.push_back(children.first);
                offspring.push_back(children.second);
            });
        }
        // Mutation
        for (auto &child : offspring)
            possibly(P_MUTATION, [&] { child.mutate(COEF * R_CHANGE); });
        for (auto &child : offspring) // there maybe a genius?
            possibly(P_MUTATION, [&] { child.augment(COEF * R_CHANGE_ADAPT, 30); });

        // Survival & Diversification
        population.insert(end(population), all_of(offspring));
        elitism_v2(population, diff_threshold);
        kld_seed_v2(population);
        sort(begin(population) + N_KEEP, end(population)); // CHC Adaptive
        remove_duplication(population);
        if (size(population) > POP_SIZE)
            population.resize(POP_SIZE);
        
        // "Soft" Restarts 
        bool unchanged = best_value >= last_optimal;
        if (unchanged) ++no_improve_count;
        else { last_optimal = best_value; no_improve_count = 0; }
        if (RESET_STRATEGY && no_improve_count >= 1.6 * DIVERGE_GAP) {
            hard_reset(igen);
            DIVERGE_GAP *= 1.2;
            // RESET_STRATEGY = false;
        }
        #define diverge_gap (igen - last_diverge)
        if (igen <= NUM_GEN * 0.9) {
            if (unchanged && diverge_gap >= DIVERGE_GAP) {
                last_diverge = igen;
                ++diverge_count;
                if (((igen <= NUM_GEN * 0.7 && diverge_count % 3 == 0) or
                    (igen > NUM_GEN * 0.7 && diverge_count % 4 == 0)) and RESET_STRATEGY) {
                    hard_reset(igen);
                }
                else {
                    // Phase 1: Diverge
                    cout << "\tDiverged at " << igen << '\n';
                    Real V_E_RATIO = (Real) 2.0 * num_nodes / num_edges;
                    for (int i = N_ELITE; i < popsize; i++) {
                        population[i] = population[random_int(0, N_ELITE-1)]; // template
                        population[i].mutate_hard(V_E_RATIO * DIVERGE_RATE);
                    }
                }
                if (diverge_count >= 4) {
                    DIVERGE_GAP = 18;
                    TRAINING_GAP = 7;
                }
            }
        }
        if (TRAINING_STRATEGY) {
            if (unchanged && diverge_gap > 0 && diverge_gap % TRAINING_GAP == 0) {
                CNT_LS_CALL = CNT_LS_SUCC = 0;
                training(80); // Chronical Training
                report_local_search();
            }
        }
            
        // Report
        if (DEBUG_MODE) {
            if (igen % STEP == 0)
                out << "Generation " << igen << "(" << popsize << "): " << population[0] << " with " << best_value << '\n';
        }
        if (igen % MILESTONE == 0)
            cout << "At " << igen << " got " << best_value << std::endl;
    }
    CNT_LS_CALL = CNT_LS_SUCC = 0;
    training(500);
    out << "Final " << population[0] << " with " << best_value;
    cout << "Final = " << best_value << '\n';
    report_local_search();
    return best_value;
}

int main()
{
    MapType testset_start;
    SetType included_sets;
    SetType excluded_sets;
    SetType included_tests(TESTS_DEBUG);
    SetType excluded_tests;
    for (int i = 0; i < 0; i++) {
        run_tests("IGA_F", 
            main_algorithm, 
            false, 
            testset_start, 
            included_sets, 
            excluded_sets, 
            TESTS_DEBUG, 
            excluded_tests,
            true);
    }
    for (int i = 0; i < 10; i++) {
        run_tests("IGA_F", 
            main_algorithm, 
            false, 
            testset_start, 
            included_sets, 
            excluded_sets, 
            TESTS_STRONG, 
            excluded_tests,
            true);
    }
}
//IGA_F: With Diverge only
#include "solver.hpp"
#include "testrun.hpp"

/*
IGA with Fitness Sharing
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

// Fitness sharing
Real f1(Real x);
const Real CUTPOINT = 0.12;
const Real Y_CUT = f1(CUTPOINT);
const Real ALPHA = log(1 - Y_CUT) / log(CUTPOINT);
// Fast Decrease
Real f1(Real x) { return 1 - pow(5*x, 1 / (EULER * PHI)); }
// Slow Decrease
Real sharing_function(Real x) { // diff as x
    if (x-EPS <= CUTPOINT) return f1(x);
    return 1 - pow(std::min((Real) 1, x), ALPHA);
}
// Dynamic P_CROSS, Mutation's R_CHANGE and imposed DIFF on Elitism
const Real DIFF_MIN = PI / 1000;
#define DEFAULT_COEF (2.0 * num_nodes / num_edges * EULER)
Real COEF = DEFAULT_COEF; // |V| / |E| then e for fun (for extra space)
Real R_CHANGE_ADAPT;

int dist[POP_SIZE][POP_SIZE];
Real diff[POP_SIZE][POP_SIZE];
Real diff_avg, diff_threshold;
Real dist_avg;
Real dist_avg_space;
int diameter;
Real difference(int i, int j);
void distance_measure(Social& pop) {
    int num_pairs = popsize * (popsize - 1) / 2;
    umax(num_pairs, 1);
    Int sum_distance = 0;
    Real sum_diff = 0;
    for (int i = 0; i < popsize; i++)
        for (int j = i+1; j < popsize; j++) {
            dist[i][j] = dist[j][i] = pop[i].distance_to(pop[j]);
            diff[i][j] = diff[j][i] = difference(i,j);
            umax(diameter, dist[i][j]);
            sum_distance += dist[i][j];
            sum_diff += diff[i][j];
        }
    diff_avg = sum_diff / num_pairs;
    dist_avg = sum_distance / num_pairs;
}

// inspired by ML's set difference
Real difference(Solution& a, Solution& b) {
    // dA + comm = A; dB + comm = B
    // dist = dA + dB;
    // dist + 2 * comm = A + B
    int common = (a.count_edges() + b.count_edges() - a.distance_to(b)) / 2;
    return (Real) a.distance_to(b) / (a.count_edges() + b.count_edges() - common);
}
Real difference(int i, int j) {
    int a = population[i].count_edges();
    int b = population[j].count_edges();
    int comm = (a + b - dist[i][j]) / 2;
    return (Real) dist[i][j] / (a + b - comm);
}

void calculate_stat() {
    distance_measure(population);
    diff_threshold = std::max(diff_avg / 2, DIFF_MIN);
    // diff_threshold = std::max(diff_avg, R_CHANGE);
    R_CHANGE_ADAPT = R_CHANGE * exp(dist_avg / dist_avg_space - 1);
}

// repeated local search until rendered ineffective
const int BATCH_SIZE = 10;
int enhance(Solution& sol, Real rate, int MAX_ITER) {
    int recall = 0, num_calls = 0;
    do {
        recall = sol.augment(COEF * R_CHANGE, BATCH_SIZE, true);
        num_calls += BATCH_SIZE;
    } while (recall >= rate * BATCH_SIZE && num_calls < MAX_ITER);
    return num_calls;
}

void training(int num_iter) {
    const int BUCKET_SIZE = 20;
    const Real R_TOP = 0.1;
    for (int it = 0; it < num_iter; it += BUCKET_SIZE) {
        int n_top = popsize * R_TOP;
        for (int i = 0; i < n_top; i++) {
            int rem = BUCKET_SIZE - enhance(population[i], 0.3, BUCKET_SIZE);
            population[i].augment(COEF * R_CHANGE_ADAPT, rem);
        }
        for (int i = n_top; i < popsize; i++)
            enhance(population[i], 0.5, BUCKET_SIZE);
        sort(all_of(population));
    }
}

void elitism_v2(Social& pop, Real min_diff, Real min_quality = 0.95) {
    sort(all_of(pop));
    for (int i = 1, it = 1; i < N_ELITE; i++) {
        int idx = -1, furthest = -1;
        Real max_sum = 0;
        for (int j = it; j < int(size(pop)); j++) {
            if (pop[j].get_objval() < min_quality * pop[i].get_objval()) break;
            bool different_enough = true;
            Real sum_diff = 0;
            for (int k = 0; k < i; k++) {
                Real diff_kj = difference(pop[k], pop[j]);
                different_enough &= (diff_kj - EPS >= min_diff);
                sum_diff += diff_kj;
            }
            if (furthest == -1 or umax(max_sum, sum_diff)) furthest = j;
            if (different_enough) {
                idx = j; break;
            }
        }
        if (idx == -1) idx = furthest;
        if (idx == -1) break;
        std::swap(pop[idx], pop[it++]);
        sort(begin(pop) + it, end(pop)); // for line 111 to be correct
    }
}

void kld_seed_v2(Social& pop) {
    for (int i = 0, it = N_ELITE; i < N_ELITE; i++) {
        for (int _ = 0; _ < N_SEED_PER_ELITE; _++) {
            int idx = -1;
            Real max_diff = -1;
            for (int j = it; j < int(size(pop)); j++) {
                // maximizes difference to elite AND Previous Seed
                Real sum_diff = difference(pop[i], pop[j]);
                for (int k = it-1; k >= it - _; k--) 
                    sum_diff += difference(pop[j], pop[k]);
                if (umax(max_diff, sum_diff)) idx = j;
            }
            std::swap(pop[idx], pop[it++]);
        }
    }
}

Social& algorithm_initialization() {
    auto& pop = ::population;
    pop = init_population();
    cout << "\tInit population: Done heuristics\n";
    distance_measure(pop);
    dist_avg_space = dist_avg;
    return pop;
}

int main_algorithm(std::ofstream& out) {
    CONSTANTS();
    cout << "Running algorithm...\n";
    auto population = algorithm_initialization(); 
    ::COEF = DEFAULT_COEF;
    cout.flush();
    
    // Additional Policies
    int DIVERGE_GAP = 15;
    int TRAINING_GAP = 4;
    const Real DIVERGE_RATE = 0.4;
    int no_improve_count = 0;
    int diverge_count = 0;
    int reset_count = 0;
    int last_diverge = 0; 
    #define diverge_gap (igen - last_diverge)
    int last_training = 0;
    #define training_gap (igen - last_training)
    
    int last_optimal = best_value;
    bool DO_DIVERGE = false;
    bool DO_RESET = false;
    bool DO_TRAINING = false;
    vector<int> record(NUM_GEN + 5, INF);
    record[0] = best_value;

    auto hard_reset = [&] (int igen) {
        DIVERGE_GAP = 15;
        TRAINING_GAP = 4;
        last_diverge = igen;
        if (++reset_count >= 3) DO_RESET = false;
        cout << " || Hard reset at " << igen << '\n';
        Social keeps;
        for (int i = 0; i < N_ELITE; i++) keeps.push_back(population[i]);
        sp_handler.calc_for(graph); // new SP order
        population = algorithm_initialization();
        for (int i = 0; i < N_ELITE; i++)
            std::swap(population[popsize-i-1], keeps[i]);
        elitism_v2(population, diff_avg); // without this line, there's reset bug
    };

    auto periodic_training = [&] (int igen) {
        if (diverge_gap > 0 && training_gap >= TRAINING_GAP 
            && diverge_gap % TRAINING_GAP == 0) {
            cout << "\tTrained at " << igen << '\n';
            CNT_LS_CALL = CNT_LS_SUCC = 0;
            training(80);
            report_local_search();
            elitism(population, diff_avg);
            if (best_value >= record[last_training]) TRAINING_GAP += 2;
            last_training = igen;
        }
    };

    auto diverge = [&] (int igen) {
        cout << "\tDiverged at " << igen << '\n';
        Real V_E_RATIO = (Real) num_nodes / num_edges;
        for (int i = N_ELITE; i < popsize; i++) {
            population[i] = population[random_int(0, N_ELITE-1)]; // template
            population[i].mutate_hard(V_E_RATIO * DIVERGE_RATE);
        }
        sort(all_of(population));
        if (best_value >= record[last_diverge]) DIVERGE_GAP *= 1.2;
        last_diverge = igen;
    };

    for (int igen = 1; igen <= NUM_GEN; igen++) {
        calculate_stat();
        // Fitness sharing
        vector<Real> fitness;
        for (auto p_i : population) fitness.push_back(p_i.get_objval());
        for (int i = 0; i < popsize; i++) {
            Real coef = 0;
            for (int j = 0; j < popsize; j++)
                coef += sharing_function(difference(i,j));
            fitness[i] /= coef;
        }
        auto mating_pool = roulette_wheel_selection(population, fitness); // considers increasing pool size
        // std::copy_backward(begin(population), begin(population) + N_KEEP, end(mating_pool));
        // CHC Conservatism
        for (int i = 0, last = popsize; i < N_ELITE; i++) {
            pool_index[--last] = i;
            mating_pool[last] = population[i];
        }
        Real max_diff = 0;
        for (auto i : pool_index) for (auto j : pool_index) umax(max_diff, diff[i][j]);
        // Crossover
        Social offspring;
        while (offspring.size() < 2 * popsize) {
            int pa = random_int(0, popsize-1);
            int ma = random_int(0, popsize-1);
            pa = pool_index[pa]; ma = pool_index[ma];
            #define father population[pa]
            #define mother population[ma]
            Real P_CROSS = equals(dist_avg, 0) ? 
                0 : std::min((Real) 1, pow(diff[pa][ma] / max_diff, 1 / EULER)) * P_CROSS_MAX;
            umax(P_CROSS, P_CROSS_MIN);
            possibly(P_CROSS, [&] {
                auto children = father.crossover(mother);
                offspring.push_back(children.first);
                offspring.push_back(children.second);
            });
        }
        // Mutation
        for (auto &child : offspring)
            possibly(P_MUTATION, [&] { child.mutate(COEF * R_CHANGE); });
        for (auto &child : offspring) // there maybe a genius?
            possibly(P_MUTATION, [&] { child.augment(COEF * R_CHANGE_ADAPT, 30); });

        // Survival & Diversification
        population.insert(end(population), all_of(offspring));
        elitism_v2(population, diff_threshold);
        kld_seed_v2(population);
        sort(begin(population) + N_KEEP, end(population)); // CHC Adaptive
        remove_duplication(population);
        if (size(population) > POP_SIZE)
            population.resize(POP_SIZE);
        
        // "Soft" Restarts 
        bool unchanged = best_value >= last_optimal;
        if (unchanged) ++no_improve_count;
        else { last_optimal = best_value; no_improve_count = 0; }
        if (igen <= NUM_GEN * 0.9) {
            if (DO_RESET && no_improve_count >= 25 
            && diverge_gap >= 15) {
                hard_reset(igen);
            } else
            if (unchanged && diverge_gap >= DIVERGE_GAP) diverge(igen);
        }
        if (DO_TRAINING) periodic_training(igen);
        record[igen] = best_value;
            
        // Report
        if (DEBUG_MODE) {
            if (igen % STEP == 0)
                out << "Generation " << igen << "(" << popsize << "): " << population[0] << " with " << best_value << '\n';
        }
        if (igen % MILESTONE == 0)
            cout << "At " << igen << " got " << best_value << std::endl;
    }
    CNT_LS_CALL = CNT_LS_SUCC = 0;
    training(500);
    out << "Final " << population[0] << " with " << best_value;
    cout << "Final = " << best_value << '\n';
    report_local_search();
    return best_value;
}

int main()
{
    MapType testset_start;
    SetType included_sets;
    SetType excluded_sets;
    SetType included_tests(TESTS_DEBUG);
    SetType excluded_tests;
    for (int i = 0; i < 0; i++) {
        run_tests("IGA_F", 
            main_algorithm, 
            false, 
            testset_start, 
            included_sets, 
            excluded_sets, 
            TESTS_DEBUG, 
            excluded_tests,
            true);
    }
    for (int i = 0; i < 10; i++) {
        run_tests("IGA_F", 
            main_algorithm, 
            false, 
            testset_start, 
            included_sets, 
            excluded_sets, 
            TESTS_STRONG, 
            excluded_tests,
            true);
    }
}
