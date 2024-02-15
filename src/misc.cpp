#include "solver.hpp"
#include "testrun.hpp"

/*
TRY:
    Dynamic Mutation Ratio based on
        |E| / |V|
*/

Social population;
#define best_value population[0].get_objval()

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
        recall = sol.local_search(RATIO_LOW * R_CHANGE, BATCH_SIZE, true);
        num_calls += BATCH_SIZE;
    } while (recall >= rate * BATCH_SIZE && num_calls < MAX_ITER);
    return num_calls;
}

void enhance_seeds() {
    const int QTY = 100;
    for (int i = 0; i < N_ELITE + N_SEED; i++) {
        int rem = QTY;
        rem -= enhance(population[i], 0.5, QTY);
        population[i].local_search(RATIO_LOW * R_CHANGE_ADAPT, rem);
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
