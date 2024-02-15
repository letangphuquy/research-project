#include "solver.hpp"
#include "testrun.hpp"

/*
IGA with Fitness Sharing
*/
Social population;
#define best_value population[0].get_objval()
#define N_KEEP (N_ELITE + N_SEED)

const Real P_CROSS_MIN = 0.25;
const Real P_CROSS_MAX = 0.95;

// Fitness sharing
#define MAX_DIFF (2 * num_nodes)
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
Real COEF = DEFAULT_COEF; // then e for fun (for extra space)
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
        recall = sol.local_search(COEF * R_CHANGE, BATCH_SIZE, true);
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
            int rem = BUCKET_SIZE - enhance(population[i], 0.1, BUCKET_SIZE);
            population[i].local_search(R_CHANGE_ADAPT, rem);
        }
        for (int i = n_top; i < popsize; i++)
            enhance(population[i], 0.5, BUCKET_SIZE);
        sort(all_of(population));
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
    cout << "Running algorithm...\n";
    auto population = algorithm_initialization(); 
    ::COEF = DEFAULT_COEF;
    cout.flush();
    
    // Convergence Check - Soft Restart(s)
    int CONVERGE_GAP = 12;
    int TRAINING_GAP = 4;
    const Real DIVERGE_RATE = 0.5;
    int training_count = 0;
    int no_improve_count = 0;
    int diverge_count = 0;
    int last_optimal = best_value;
    std::unordered_map<int,int> first_gen_with_opt;
    first_gen_with_opt[last_optimal] = -1;
    auto account_for_new_record = [&] (int igen) {
        auto &gen = first_gen_with_opt[best_value];
        if (!gen) gen = igen;
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
            possibly(P_MUTATION, [&] { child.local_search(COEF * R_CHANGE_ADAPT, 30); });

        // Survival & Diversification
        population.insert(end(population), all_of(offspring));
        elitism(population, diff_threshold);
        kld_seed(population);
        // enhance_seeds();
        sort(begin(population) + N_KEEP, end(population)); // CHC Adaptive
        remove_duplication(population);
        if (size(population) > POP_SIZE)
            population.resize(POP_SIZE);
        
        // "Soft" Restarts
        ++training_count; 
        bool unchanged = best_value >= last_optimal;
        if (unchanged) ++no_improve_count;
        else { last_optimal = best_value; no_improve_count = 0; }
        account_for_new_record(igen);
        if (igen - first_gen_with_opt[best_value] < 3.5 * CONVERGE_GAP) // restart ineffective?, skip
        {
            if (unchanged && no_improve_count >= CONVERGE_GAP && training_count >= TRAINING_GAP / 2.5) {
                no_improve_count = 0;
                if ((++diverge_count) % 3 == 0) {
                    // Population reset
                    cout << " || Hard reset at " << igen << '\n';
                    Solution keep = population[0];
                    sp_handler.calc_for(graph); // new SP order
                    population = algorithm_initialization();
                    std::swap(population.back(), keep);
                    elitism(population, diff_avg); // without this line, there'll be reset bug
                }
                else {
                    // Phase 1: Diverge
                    if (diverge_count % 3 == 2) CONVERGE_GAP += 2;
                    cout << "\tDiverged at " << igen << '\n';
                    Real V_E_RATIO = (Real) 2.0 * num_nodes / num_edges;
                    for (int i = N_ELITE; i < popsize; i++)
                        population[i].mutate(V_E_RATIO * DIVERGE_RATE);
                    training(40);
                    // Phase 2: Finer approximation
                    if (COEF);
                    COEF = std::max(0.1l, COEF * 0.75);
                }
            }
        }
        else COEF = DEFAULT_COEF;
        if (no_improve_count >= CONVERGE_GAP / 3 && 
        training_count > 0 &&
        training_count % TRAINING_GAP == 0) {
            cout << "\tTrained with COEF = " << COEF << " at " << igen << '\n';
            int before = best_value;
            CNT_LS_CALL = CNT_LS_SUCC = 0;
            training(80); // Chronical Training
            report_local_search();
            if (best_value >= before) TRAINING_GAP += 1;
            training_count = 0;
        }
        account_for_new_record(igen);
            
        // Report
        if (DEBUG_MODE) {
            if (igen % STEP == 0)
                out << "Generation " << igen << "(" << popsize << "): " << population[0] << " with " << best_value << '\n';
        }
        if (igen % MILESTONE == 0)
            cout << "At " << igen << " got " << best_value << '\n';
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
    for (int i = 0; i < 3; i++) {
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
    for (int i = 0; i < 5; i++) {
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
