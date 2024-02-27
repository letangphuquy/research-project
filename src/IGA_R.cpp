#include "solver.hpp"
#include "testrun.hpp"

/*
IGA with Fitness Sharing
*/

Social population;
#define best_value (population[0].get_objval() + added_cost)

const Real P_CROSS_MIN = 0.25;
const Real P_CROSS_MAX = 0.95;

// Fitness sharing
const Real DELTA_SHARE = 0.4;
const Real ALPHA = 1 / EULER;
Real sharing_function(int distance) {
    Real diff = (Real) distance / num_edges;
    return 1 - pow(diff / DELTA_SHARE, ALPHA);
}
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
    diff_avg = dist_avg / num_edges;
    diff_threshold = std::max(diff_avg / 2, DIFF_MIN);
    // diff_threshold = std::max(diff_avg, R_CHANGE);
    R_CHANGE_ADAPT = R_CHANGE * exp(dist_avg / dist_avg_space - 1);
}

// repeated local search until rendered ineffective
int enhance(Solution& sol, Real rate, int MAX_ITER) {
    const int BATCH_SIZE = 10;
    int recall = 0, num_calls = 0;
    do {
        recall = sol.augment(R_CHANGE, BATCH_SIZE, true);
        num_calls += BATCH_SIZE;
    } while (recall >= rate * BATCH_SIZE && num_calls < MAX_ITER);
    return num_calls;
}
void enhance_seeds() {
    const int QTY = 100;
    for (int i = 0; i < N_KEEP; i++) {
        int rem = QTY;
        rem -= enhance(population[i], 0.5, QTY);
        population[i].augment(R_CHANGE_ADAPT, rem);
    }
}

int main_algorithm(std::ofstream& out) {
    // CONSTANTS();
    cout << "Running algorithm...\n";
    auto& population = ::population;
    population = init_population();
    cout << "\tInit population: Done heuristics\n";
    cout.flush();
    dist_avg_space = distance_measure(population);
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
            possibly(P_MUTATION, [&] { child.mutate(R_CHANGE); });
        for (auto &child : offspring) // there maybe a genius?
            possibly(P_MUTATION, [&] { child.augment(R_CHANGE_ADAPT, 30); });

        // Survival & Diversification
        population.insert(end(population), all_of(offspring));
        elitism(population, diff_threshold);
        kld_seed(population);
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
        if (igen % MILESTONE == 0)
            cout << "At " << igen << " got " << best_value << '\n';
    }
    for (int i = 0; i < N_KEEP; i++) 
        enhance(population[i], 0.1, 300);
    for (int i = N_KEEP; i < popsize; i++)
        enhance(population[i], 0.5, 100);
    sort(all_of(population));
    out << "Final " << population[0] << " with " << (best_value + added_cost);
    cout << "Final = " << (best_value + added_cost) << '\n';
    cout << "LS success rate: " << CNT_LS_SUCC << " / " << CNT_LS_CALL 
        << ": " << ((Real) CNT_LS_SUCC / CNT_LS_CALL) << '\n';
    return (best_value + added_cost);
}

int main()
{
    bool F = true;
    MapType testset_start;
    SetType included_sets(SETS_PROTOTYPE);
    SetType excluded_sets;
    SetType included_tests;
    SetType excluded_tests;
    for (int i = 0; i < 5; i++) {
        run_tests("IGA_R", 
            main_algorithm, 
            false, 
            testset_start, 
            included_sets, 
            excluded_sets, 
            included_tests, 
            excluded_tests,
            true,
            true
            );
    }
}
