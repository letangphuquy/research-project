#include "solver.hpp"
#include "testrun.hpp"

Social population;
#define best_value (population[0].get_objval() + added_cost)

const Real P_CROSS_MIN = 0.25;
const Real P_CROSS_MAX = 0.95;

// Dynamic P_CROSS, Mutation's R_CHANGE and imposed DIFF on Elitism
const Real DIFF_MIN = 0.005;
Real diff_avg, diff_threshold;
Real dist_avg;
Real dist_avg_space;
Real R_CHANGE_ADAPT;

#define N_KEEP (N_ELITE + N_SEED)

Real distance_sampling(Social& pop) {
    int num_tries = pop.size() * log2(pop.size());
    umax(num_tries, 1);
    Int sum_distance = 0;
    for (int _ = 0; _ < num_tries; _++) {
        auto u = random_element(pop);
        auto v = random_element(pop);
        sum_distance += u.distance_to(v);
    }
    return sum_distance / num_tries;
}

void calculate_stat() {
    dist_avg = distance_sampling(population);
    diff_avg = dist_avg / num_edges;
    diff_threshold = std::max(diff_avg / 2, DIFF_MIN);
    // diff_threshold = std::max(diff_avg, R_CHANGE);
    R_CHANGE_ADAPT = R_CHANGE * exp(dist_avg / dist_avg_space - 1);
}

/*
IMPORTANT CHOICE: (NGÃ BA ĐƯỜNG) 
- CHANGE CONDITION TO KEEP "FEATURE-NOT-BUG" BACK (NOT LIKELY WHEN WE HAVE SUPERIOR IGA_F)
- CHANGE ENHANCE SCHEME (TO BE MORE ECO-FRIENDLY) (CURRENTLY)
- DROP THIS APPROACH (ALMOST CERTAINLY?)
- APPLY DIVERGING AS SEEN IN IGA_F (NOPE)

- UP NEXT: MAKE IT SAME AS RGA, ONLY DIFFERENT IN DYNAMIC CROSSOVER, THEN CONSIDER MERGING TO RGA?
*/

int cached_dist[POP_SIZE][POP_SIZE];

// DIAGNOSING WHY RESULT FOR WORLD666 IS HIGH ABNORMALLY

int main_algorithm(std::ofstream& out) {
    assert(added_cost == 0);
    cout << "Running algorithm...\n";
    auto& population = ::population;
    population = init_population();
    // for (auto &pop : population) pop = heuristics_random();
    cout << "\tInit population: Done heuristics\n";
    cout.flush();
    dist_avg_space = distance_sampling(population);
    for (int igen = 1; igen <= NUM_GEN; igen++) {
        calculate_stat();
        roulette_wheel_selection(population); // takes index only
        // Direct Promote Like in RGA
        for (int i = 0; i < N_KEEP; i++) 
            pool_index[size(pool_index)-1 - i] = i;
        // Crossover
        Social offspring;
        memset(cached_dist, -1, sizeof(cached_dist));
        if (!equals(dist_avg, 0)) {
            while (offspring.size() < 2 * popsize) {
                int pa = random_int(0, popsize-1);
                int ma = random_int(0, popsize-1);
                pa = pool_index[pa]; ma = pool_index[ma];
                #define father population[pa]
                #define mother population[ma]
                int& dist = cached_dist[pa][ma];
                if (dist == -1) dist = father.distance_to(mother);
                Real P_CROSS = std::min((Real) 1, pow(dist / dist_avg, 1 / EULER)) * P_CROSS_MAX;
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
        } 
        else {
            // Divergence as applied in IGA_F
            Real V_E_RATIO = (Real) num_nodes / num_edges;
            for (int i = N_ELITE; i < popsize; i++) {
                population[i] = population[random_int(0, N_ELITE-1)]; // template
                population[i].mutate_hard(V_E_RATIO * 0.5);
            }
        }

        // Survival & Diversification
        population.insert(end(population), all_of(offspring));
        elitism(population, diff_threshold);
        kld_seed(population);
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
    sort(all_of(population));
    out << "Final " << population[0] << " with " << best_value;
    cout << "Final = " << best_value << '\n';
    report_local_search();
    return best_value;
}

int main()
{
    MapType testset_start;
    SetType included_sets(SETS_BENCHMARK);
    SetType excluded_sets;
    SetType included_tests({"world666"});
    SetType excluded_tests;
    for (int i = 0; i < 5; i++) {
        run_tests("IGA", 
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
