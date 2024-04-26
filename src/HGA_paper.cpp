#include "solver.hpp"
#include "testrun.hpp"

/*
IGA_F, but nerfed significantly to MATCH THE STATED IN PAER
SGA:
+ Jaccard distance
+ Dynamic Crossover
+ 2 Longest Distance
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
    pop = init_pop_mixed();
    if (VERBOSE_LOG) cout << "\tInit population: Done heuristics\n";
    distance_measure(pop);
    dist_avg_space = dist_avg;
    return pop;
}

int main_algorithm(std::ofstream& out) {
    CONSTANTS();
    if (VERBOSE_LOG) cout << "Running algorithm...\n";
    auto population = algorithm_initialization(); 
    cout.flush();
    
    // Mild Divergence
    vector<int> record(NUM_GEN + 5, INF);
    record[0] = best_value;

    for (int igen = 1; igen <= NUM_GEN; igen++) {
        calculate_stat();
        // RGA & IGA 's simpler scheme
        roulette_wheel_selection(population);
        for (int i = 0; i < N_KEEP; i++) 
            pool_index[size(pool_index)-1 - i] = i;
        // std::copy_backward(begin(population), begin(population) + N_KEEP, end(mating_pool));
        // Crossover
        Real max_diff = 0;
        for (auto i : pool_index) for (auto j : pool_index) umax(max_diff, diff[i][j]);
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
            possibly(P_MUTATION, [&] { child.mutate(R_CHANGE); });
        // Survival & Diversification
        population.insert(end(population), all_of(offspring));
        elitism_v2(population, diff_threshold);
        kld_seed_v2(population);
        sort(begin(population) + N_KEEP, end(population)); // CHC Adaptive
        remove_duplication(population);
        if (size(population) > POP_SIZE)
            population.resize(POP_SIZE);
        
        // Post-analysis
        record[igen] = best_value;
            
        // Report
        if (DEBUG_MODE) {
            if (igen % STEP == 0)
                out << "Generation " << igen << "(" << popsize << "): " << population[0] << " with " << best_value << '\n';
        }
        if (VERBOSE_LOG) {
            if (igen % MILESTONE == 0)
                cout << "At " << igen << " got " << best_value << std::endl;
        }
    }
    report_local_search();
    out << "Final " << population[0] << " with " << best_value;
    
    cout << "Summary: ";
    for (int i = 1; i <= NUM_GEN; i++) cout << record[i] << ' ';
    cout << '\n';
    
    cout << "Final = " << best_value << '\n';
    return best_value;
}

int main()
{
    MapType testset_start;
    SetType included_sets(SETS_MODEST);
    SetType excluded_sets;
    SetType included_tests;
    SetType excluded_tests;
    for (int i = 0; i < 10; i++) {
        run_tests("HGA", 
            main_algorithm, 
            false, 
            testset_start, 
            // i % 2 == 0 ? SETS_BENCHMARK_ADDITIONAL : SETS_BENCHMARK, 
            included_sets,
            excluded_sets, 
            included_tests, 
            excluded_tests,
            true,
            false
        );
    }
}
