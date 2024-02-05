#if !defined(SOLVER_H)
#define SOLVER_H

#include "template.hpp"
#include "input.hpp"
#include "solution.hpp"
#include "unit_test.hpp"


const int STEP = std::min(50, NUM_GEN / 20);
const int MILESTONE = NUM_GEN / 10; // for observing

#define popsize (population.size())
void debug_social(Social& pop, string title = "") {
    if (title.size()) cout << title << '\n';
    for (auto pi : pop)
        cout << '\t' << pi << ": " << pi.get_objval() << '\n';
}

// Init phase
Social init_population(void) {
    Social pop;
    auto join = [&] (Social group) { pop.insert(end(pop), all_of(group)); };
    join(heuristics_random_set());
    join(heuristics_stem_set());
    join(heuristics_mst_set());
    join(heuristics_sp_set());
    while (pop.size() < POP_SIZE)
        pop.push_back(heuristics_random());
    pop.resize(POP_SIZE);
    return pop;
}

// Crossover selection
vector<int> pool_index;
Social roulette_wheel_selection(Social& population, bool is_minimization = true) {
    pool_index.clear();
    Social pool;
    sort(all_of(population));

    vector<Real> fitness(popsize);
    for (int i = 0; i < popsize; i++)
        fitness[i] = is_minimization ?
            ((Real) 1 / population[i].get_objval()) : population[i].get_objval();
    Real sum = 0;
    for (int i = 0; i < popsize; i++)
        sum += fitness[i];
    for (auto &p_i : fitness) p_i /= sum;

    vector<Real> spins;
    for (int i = 0; i < popsize; i++)
        spins.push_back(random());
    sort(all_of(spins));

    sum = 0;
    for (int it = -1, i = 0; i < int(size(spins)); i++) {
        for (; sum-EPS < spins[i] && it+1 < popsize;) 
            sum += fitness[++it];
        pool.push_back(population[it]);
        pool_index.push_back(it);
    }
    return pool;
}

// Diversity maintenance
void elitism(Social& pop, Real min_diff = R_CHANGE) {
    sort(all_of(pop));
    for (int i = 1, it = 1; i < N_ELITE; i++) {
        int idx = -1;
        for (int j = it; j < int(size(pop)); j++) {
            bool different_enough = true;
            for (int k = 0; k < i; k++)
                different_enough &= (pop[k].difference(pop[j]) >= min_diff);
            if (different_enough) {
                idx = j; break;
            }
        }
        if (idx == -1) break;
        std::swap(pop[idx], pop[it++]);
    }
}

void kld_seed(Social& pop) {
    for (int i = 0, it = N_ELITE; i < N_ELITE; i++) {
        for (int _ = 0; _ < N_SEED_PER_ELITE; _++) {
            int idx = -1, max_dist = -1;
            for (int j = it; j < int(size(pop)); j++) {
                // maximizes distance to elite AND Previous Seed
                int sum = pop[j].distance_to(pop[i]);
                for (int k = it-1; k >= it - _; k--) 
                    sum += pop[j].distance_to(pop[k]);
                if (umax(max_dist, sum)) idx = j;
            }
            std::swap(pop[idx], pop[it++]);
        }
    }
}

void remove_duplication(Social& pop) {
    sort(all_of(pop));
    pop.erase(std::unique(all_of(pop)), end(pop));
}
/*
void remove_duplication(Social& pop, Real min_diff = 0) {
    sort(all_of(pop));
    pop.erase(
        std::unique(all_of(pop), [&] (Solution a, Solution b) { 
            return a.difference(b) > min_diff;
        }), 
        end(pop)
    );
}
*/

Real distance_sampling(const Social& pop) {
    int num_tries = pop.size() * log2(pop.size());
    umax(num_tries, 1);
    int sum_distance = 0;
    for (int _ = 0; _ < num_tries; _++) {
        auto u = random_element(pop);
        auto v = random_element(pop);
        sum_distance += u.distance_to(v);
    }
    return sum_distance / num_tries;
}

#endif // SOLVER_H
