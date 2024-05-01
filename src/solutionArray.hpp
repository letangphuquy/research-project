#if !defined(SOLUTION_ARRAY_H)
#define SOLUTION_ARRAY_H

#include "template.hpp"
#include "interfaces/ISolution.hpp"
#include "genotypeArray.hpp"

#include "bookkeeper.hpp"
#include "dsu.hpp"
#include "mst.hpp"
#include "graph.hpp"
#include "problem.hpp"
#include <queue>

#define INF 1'000'000'000

class Solution: public ISolution
{
private:
    Genotype chromosome;
    pair<bool,int> objVal; 

    void filter_repeat();
    void reduce(Real r_fluctuate);
    void make_span(); // terminals only
    void make_span_wide(Real r_drop); // some distinct components, also
    void selfCorrect(Real r_drop = 1, Real r_fluctuate = R_FLUCTUATE) {
        r_drop >= 1-EPS ? make_span() : make_span_wide(r_drop);
        reduce(r_fluctuate);
    }
public:
    int age;
    Solution() {
        chromosome = Genotype(2*num_nodes);
        objVal = {false, INF}; age = 0;
    }
    void fromBitString(Gene gene) {
        chromosome.clear();
        Iterate(gene, [&] (int idx) { chromosome.append(idx); });
    }
    void fromVector(cst(vector<int>) vec) {
        chromosome.clear();
        for (auto x : vec) chromosome.append(x);
    }
    void fromGenotype(Genotype chromo) { chromosome = chromo;  }
    int getObjval(void);
    bool operator< (Solution rhs) { return getObjval() < rhs.getObjval(); }
    void mutate(Real pMutate);
    void mutate(void) { mutate(P_MUTATION); };
    pair<Solution, Solution> crossover(const Solution& mate);
};

int Solution::getObjval() {
    if (!objVal.first) { 
        objVal.second = 0;
        cc_handler.init(num_nodes);
        for (int i = 0; i < chromosome.size(); i++) {
            int e = chromosome[i];
            auto [u,v,w] = edges[e];
            objVal.second += w;
            cc_handler.merge_set(u,v);
        }
        for (int i = 1; i < num_terminals; i++)
            if (!cc_handler.same_set(terminals[i], terminals[0]))
                objVal.second = INF;
    }
    return objVal.second;
}

pair<Solution, Solution> Solution::crossover(const Solution& mate) { // extract common edges from parents
    if (marker.size() < num_edges) marker = BookKeep(num_edges + 5);
    marker.tick();
    for (int i = 0; i < chromosome.size(); i++) marker.inc(chromosome[i]);
    for (int i = 0; i < mate.chromosome.size(); i++) 
        marker.inc(mate.chromosome[i]);
    Genotype chromoA(num_nodes), chromoB(num_nodes);
    for (int i = 0; i < chromosome.size(); i++)
        if (marker.get(chromosome[i]) >= 2) {
            chromoA.append(chromosome[i]);
            chromoB.append(chromosome[i]);
        } else (random_int(0,1) ? chromoA : chromoB).append(chromosome[i]);
    for (int i = 0; i < mate.chromosome.size(); i++)
        if (marker.get(mate.chromosome[i]) == 1)
            (random_int(0,1) ? chromoA : chromoB).append(mate.chromosome[i]);
    auto children = std::make_pair(Solution(), Solution());
    children.first.fromGenotype(chromoA);
    children.second.fromGenotype(chromoB);
    children.first.selfCorrect(0.5);
    children.second.selfCorrect(0.5);
    return children;
}

void Solution::mutate(Real pMutate) {
    for (int i = 0; i < chromosome.size(); i++) {
        possibly(pMutate, [&] {
            int newEdge = random_int(0, num_edges-1);
            mst_handler.set_bias(chromosome[i], 0);
            mst_handler.set_bias(newEdge, 1);
            chromosome.set(i, newEdge);
        });
    }
    filter_repeat();
    selfCorrect(0.8);
}

void Solution::filter_repeat(void) {
    marker.tick();
    for (int i = 0; i < chromosome.size(); ) {
        if (marker.get(chromosome[i])) chromosome.remove(i);
        else marker.inc(chromosome[i++]);
    }
}

void Solution::reduce(Real r_fluctuate) {
    static vector<bool> is_removed;
    fromGenotype(mst_handler.calc_for(chromosome, r_fluctuate));
    pheno->construct_adjacency_list();
    pheno->compute_degree();
    is_removed.assign(num_nodes+1, false);
    std::queue<int> leaves;
    for (int u = 1; u <= num_nodes; u++) {
        if (pheno->is_leaf(u)) leaves.push(u);
    }
    while (!leaves.empty()) {
        int u = leaves.front(); leaves.pop();
        if (is_terminal[u]) continue;
        for (auto [idx, edge] : (*pheno)[u]) {
            auto [fr, to, wei] = *edge;
            int v = fr^to^u;
            if (is_removed[v]) continue;
            pheno->remove_leaf_edge(v, u, idx);
            gene[idx].set(0); // moved out here due to weird bug
            if (pheno->is_leaf(v)) leaves.push(v);
        }
    }
    force_update();
    return *this;
}

std::ostream& operator<< (std::ostream& stream, Solution solution) {
    return stream;
}


#endif // SOLUTION_ARRAY_H
