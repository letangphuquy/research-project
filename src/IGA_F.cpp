#include "solver.hpp"
#include "testrun.hpp"

/*
IGA with Fitness Sharing
*/

Social population;
#define the_best population[0].get_objval()

const Real P_CROSS_MIN = 0.25;
const Real P_CROSS_MAX = 0.95;

// Fitness sharing
#define DELTA_SHARE (2 * num_nodes)
// Fast Decrease
Real f1(Real x) { return 1 - pow(5*x, 1 / (EULER * PHI)); }
// Slow Decrease
Real sharing_function(int distance) {
    Real x = (Real) distance / DELTA_SHARE;
    Real cutting_point = 0.12;
    if (x-EPS <= cutting_point) return f1(x);
    Real y1 = f1(cutting_point);
    Real alpha = log(1 - y1) / log(cutting_point);
    return 1 - pow(std::min((Real) 1, x), alpha);
}
// ML's set difference
// Real difference(Solution& a, Solution& b) {
//     // dA + comm = A; dB + comm = B
//     // dist = dA + dB;
//     // dist + 2 * comm = A + B
//     int common = (a.count_edges() + b.count_edges() - a.distance_to(b)) / 2;
//     return (Real) a.distance_to(b) / (a.count_edges() + b.count_edges() - common);
// }
// Dynamic P_CROSS, Mutation's R_CHANGE and imposed DIFF on Elitism
const Real DIFF_MIN = 0.005;
Real diff_avg, diff_threshold;
Real dist_avg;
Real dist_avg_space;
Real R_CHANGE_ADAPT;

#define N_KEEP (N_ELITE + N_SEED)

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
    diff_avg = dist_avg / (2 * num_nodes);
    diff_threshold = std::max(diff_avg / 2, DIFF_MIN);
    // diff_threshold = std::max(diff_avg, R_CHANGE);
    Real COEF = 2.0 * num_nodes / num_edges; // 2|V|, then e for fun (for extra space)
    R_CHANGE_ADAPT = R_CHANGE * exp(dist_avg / dist_avg_space - 1) * COEF;
}

// repeated local search until rendered ineffective
const int BATCH_SIZE = 10;
int enhance(Solution& sol, Real rate, int MAX_ITER) {
    int recall = 0, num_calls = 0;
    do {
        recall = sol.local_search(R_CHANGE, BATCH_SIZE, true);
        num_calls += BATCH_SIZE;
    } while (recall >= rate * BATCH_SIZE && num_calls < MAX_ITER);
    return num_calls;
}
void enhance_seeds() {
    const int QTY = 100;
    for (int i = 0; i < N_KEEP; i++) {
        int rem = QTY;
        rem -= enhance(population[i], 0.5, QTY);
        population[i].local_search(R_CHANGE_ADAPT, rem);
    }
}

void training(int num_iter) {
    const int BUCKET_SIZE = 50;
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

int main_algorithm(std::ofstream& out) {
    cout << "Running algorithm...\n";
    auto& population = ::population;
    population = init_population();
    cout << "\tInit population: Done heuristics\n";
    cout.flush();
    dist_avg_space = distance_measure(population);
    // Convergence Check
    const int CONVERGE_GAP = 15;
    const Real DIVERGE_RATE = 0.35;
    int converge_count = 0;
    int reset_count = 0;
    Social last_gen(population);

    for (int igen = 1; igen <= NUM_GEN; igen++) {
        calculate_stat();
        vector<Real> fitness;
        for (auto p_i : population) fitness.push_back(p_i.get_objval());
        for (int i = 0; i < popsize; i++) {
            Real coef = 0;
            for (int j = 0; j < popsize; j++)
                coef += sharing_function(dist[i][j]);
            fitness[i] /= coef;
        }
        auto mating_pool = roulette_wheel_selection(population, fitness);
        // std::copy_backward(begin(population), begin(population) + N_KEEP, end(mating_pool));
        for (int i = 0, last = popsize; i < N_ELITE; i++) {
            pool_index[--last] = i;
            mating_pool[last] = population[i];
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
            possibly(P_MUTATION, [&] { child.mutate(R_CHANGE); });
        for (auto &child : offspring) // there maybe a genius?
            possibly(P_MUTATION, [&] { child.local_search(R_CHANGE_ADAPT, 30); });

        // Survival & Diversification
        population.insert(end(population), all_of(offspring));
        elitism(population, diff_threshold);
        kld_seed(population);
        // enhance_seeds();
        sort(begin(population) + N_KEEP, end(population)); // CHC Adaptive
        remove_duplication(population);
        if (size(population) > POP_SIZE)
            population.resize(POP_SIZE);
        //
        
        bool unchanged = true;
        for (int i = 0; i < popsize && unchanged; i++)
            unchanged &= population[i] == last_gen[i];
        if (unchanged) last_gen = population;
        ++converge_count;
        if (unchanged && converge_count >= CONVERGE_GAP) {
            cout << "\tDiverged at " << igen << '\n';
            Real ratio = (Real) num_nodes / num_edges;
            for (int i = N_KEEP; i < popsize; i++) 
                population[i].mutate(ratio);
            converge_count = 0;
            training(100);
            if ((++reset_count) % 3 == 0) {
                cout << "\tHard reset at " << igen << '\n';
                Solution keep = population[0];
                sp_handler.calc_for(graph); // new SP order
                population = init_population();
                std::swap(population.back(), keep);
            }
        }
            
        // Report
        if (DEBUG_MODE) {
            if (igen % STEP == 0)
                out << "Generation " << igen << "(" << popsize << "): " << population[0] << " with " << the_best << '\n';
        }
        if (igen % MILESTONE == 0)
            cout << "At " << igen << " got " << the_best << '\n';
    }
    report_local_search();
    CNT_LS_CALL = CNT_LS_SUCC = 0;
    training(500);
    out << "Final " << population[0] << " with " << the_best;
    cout << "Final = " << the_best << '\n';
    report_local_search();
    return the_best;
}

int main()
{
    MapType testset_start;
    SetType included_sets(SETS_BENCHMARK);
    SetType excluded_sets;
    SetType included_tests;
    SetType excluded_tests;
    for (int i = 0; i < 10; i++) {
        run_tests("IGA_F", 
            main_algorithm, 
            false, 
            testset_start, 
            included_sets, 
            excluded_sets, 
            included_tests, 
            excluded_tests,
            true);
    }
}
