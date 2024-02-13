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

// RWS for Crossover candidates
// returns index(s) of chosen in pool
// Update: Changed internal API to support more dynamic fitness scheme (fitness sharing) IGA_R
// NOTE: Sampling With Replacement causes Premature Convergence
vector<int> pool_index;
vector<int> roulette_wheel_selection_index(Social population, vector<Real> fitness, bool is_minimization) {
    pool_index.clear();
    sort(all_of(population));
    if (is_minimization) for (auto &f_i : fitness) f_i = 1 / f_i;
    Real sum = 0;
    for (auto &f_i : fitness) sum += f_i;
    for (auto &f_i : fitness) f_i /= sum;

    vector<Real> spins;
    for (int i = 0; i < popsize; i++)
        spins.push_back(random());
    sort(all_of(spins));

    sum = 0;
    for (int it = -1, i = 0; i < int(size(spins)); i++) {
        for (; sum-EPS < spins[i] && it+1 < popsize;) 
            sum += fitness[++it];
        pool_index.push_back(it);
    }
    return pool_index;
}

Social roulette_wheel_selection(cst(Social) population, vector<Real> fitness, bool is_minimization = true) {
    vector<int> index = roulette_wheel_selection_index(population, fitness, is_minimization);
    Social pool;
    for (auto i : index) pool.push_back(population[i]);
    return pool;
}

// default fitness scheme
Social roulette_wheel_selection(Social& population, bool is_minimization = true) {
    vector<Real> fitness;
    for (auto p_i : population) fitness.push_back(p_i.get_objval());
    return roulette_wheel_selection(population, fitness, is_minimization);
}

void elitism(Social& pop, Real min_diff = R_CHANGE, Real min_quality = 0.95) {
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

#endif // SOLVER_H
