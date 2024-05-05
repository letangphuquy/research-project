#if !defined(SOLUTION_ARRAY_H)
#define SOLUTION_ARRAY_H

#include "template.hpp"
#include "interfaces/ISolution.hpp"
#include "genotypeArray.hpp"

#include "bookkeeper.hpp"
#include "dsu.hpp"
#include "mst.hpp"
#include "graph.hpp"
#include "bellman.hpp"
#include "problem.hpp"
#include <queue>

#define INF 1'000'000'000

typedef pair<bool,int> ObjectiveValue; 
const ObjectiveValue UNCALC(false, 1e9);
const int FIBONACCI_AGES[] {1,1,1,0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0,1};
class Solution: public ISolution
{
private:
    Genotype chromosome;
    ObjectiveValue objVal; 

    void filter_repeat(int start_index = 0, bool begin = true);
    int sum_edges();
    vector<vector<int>> get_components(cst(vector<int>) nodes);
    void connect_components(cst(vector<vector<int>>) comps);
    void reduce(Real r_fluctuate);
    void make_span(); // terminals only
    void make_span_wide(Real r_drop); // some distinct components, also
public:
    int age;
    void ages(void) { 
        ++age; 
        if ((age >= 23 && age % 3 == 0) or (age < 23 && FIBONACCI_AGES[age])) chromosome.sort();
    }
    Solution() {
        chromosome = Genotype(2*num_nodes); // reserve room for crossover and path tracing
        objVal = UNCALC; age = 0;
    }
    Solution(cst(Solution) rhs) { (*this) = rhs; }
    Solution& operator= (const Solution& other) {
        if (this == &other) return *this;
        chromosome.resize(2*num_nodes); chromosome.clear();
        fromGenotype(other.chromosome);
        objVal = other.objVal;
        age = other.age;
        return *this;
    }
    void fromBitString(Bitstr gene) {
        chromosome.clear();
        Iterate(gene, [&] (int idx) { chromosome.append(idx); });
        objVal = UNCALC;
    }
    void fromVector(cst(vector<int>) vec) {
        chromosome.clear();
        for (auto x : vec) chromosome.append(x);
        objVal = UNCALC;
    }
    void fromGenotype(cst(Genotype) chromo) {
        chromosome = chromo; 
        objVal = UNCALC; 
    }
    int get_objval(void);
    int raw_objval(void) const { return objVal.second; }
    bool operator< (cst(Solution) rhs) { return raw_objval() < rhs.raw_objval(); }
    bool operator== (cst(Solution) rhs);
    void selfCorrect(Real r_drop = 1, Real r_fluctuate = R_FLUCTUATE); 
    void mutate(Real pMutate);
    void mutate(void) { mutate(P_MUTATION); };
    pair<Solution, Solution> crossover(const Solution& mate);
    // int repeated_mutate(Real r_change, int num_iter, bool is_random_rate); // effectively local searching
    friend std::ostream& operator<< (std::ostream& stream, Solution solution);
};

bool Solution::operator== (cst(Solution) rhs) {
    if (chromosome.size() != rhs.chromosome.size()) return false;
    marker.tick();
    for (auto gene : chromosome) marker.inc(gene);
    for (auto gene : chromosome)
        if (!marker.get(gene)) return false;
    return true;
}

int Solution::sum_edges() {
    int sum = 0;
    cc_handler.init(num_nodes);
    for (int e : chromosome) {
        auto [u,v,w] = edges[e];
        sum += w;
        cc_handler.merge_set(u,v);
    }
    return sum;
}

int Solution::get_objval() {
    if (!objVal.first) { 
        objVal.first = true;
        objVal.second = sum_edges();
        for (int i = 1; i < num_terminals; i++)
            if (!cc_handler.same_set(terminals[i], terminals[0]))
                return objVal.second = INF;
    }
    return objVal.second;
}

pair<Solution, Solution> Solution::crossover(const Solution& mate) { // extract common edges from parents
    marker.resize(num_edges + 5);
    marker.tick();
    for (auto gene : chromosome) marker.inc(gene);
    for (auto gene : mate.chromosome) marker.inc(gene);
    Genotype chromoA(num_nodes), chromoB(num_nodes);
    for (auto gene : chromosome)
        if (marker.get(gene) == 1)
            (random_int(0,1) ? chromoA : chromoB).append(gene);
    for (auto gene : chromosome)
        if (marker.get(gene) == 2) {
            chromoA.append(gene);
            chromoB.append(gene);
        }
    int cnt_mate_in_B = chromoB.size();
    for (auto gene : mate.chromosome)
        if (marker.get(gene) == 1)
            (random_int(0,1) ? chromoA : chromoB).append(gene);
    (cnt_mate_in_B -= chromoB.size()) *= -1;
    chromoB.reverse(0, chromoB.size());
    chromoB.reverse(0, cnt_mate_in_B);
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
    int num_swaps = std::max(1, (int) (0.015 * chromosome.size()));
    for (int _ = 0; _ < num_swaps; _++) {
        int i = random_int(0, chromosome.size()-1);
        int j = random_int(0, chromosome.size()-1);
        std::swap(chromosome[i], chromosome[j]);
    }
    selfCorrect(0.85 + 0.15 * random());
}

// int Solution::repeated_mutate(Real r_change, int num_iter, bool is_random_rate) {
//     Solution temp = *this;
//     int cnt = 0;
//     for (int _ = 0; _ < num_iter; _++) {
//         temp.mutate(is_random_rate ? random(0, r_change) : r_change);
//         if (temp < (*this)) { (*this) = temp; ++cnt; }
//         else temp.fromGenotype(chromosome);
//     }
//     return cnt;
// }

void Solution::filter_repeat(int start_index, bool begin) {
    if (begin) marker.tick();
    for (int i = start_index; i < chromosome.size(); ) {
        if (marker.get(chromosome[i])) chromosome.remove(i);
        else marker.inc(chromosome[i++]);
    }
}

void Solution::selfCorrect(Real r_drop, Real r_fluctuate) {
    graph.resize(num_nodes); // for both make_span and reduce
    if (!age) chromosome.sort();
    r_drop >= 1-EPS ? make_span() : make_span_wide(r_drop);
    reduce(r_fluctuate);
    objVal = UNCALC;
}

void Solution::reduce(Real r_fluctuate) {
    // std::cerr << "Reducing\n";
    static vector<bool> is_removed;
    fromGenotype(mst_handler.calc_for(chromosome, r_fluctuate));
    // std::cerr << "\tDone MST\n";
    graph.load_graph(chromosome);
    // std::cerr << "\tLoaded graph\n";
    is_removed.assign(num_nodes+1, false);
    std::queue<int> leaves;
    for (int u = 1; u <= num_nodes; u++) {
        if (graph.is_leaf(u)) leaves.push(u);
    }
    marker.tick();
    while (!leaves.empty()) {
        int u = leaves.front(); leaves.pop();
        if (is_terminal[u]) continue;
        for (auto idx : graph[u]) {
            int v = edges[idx].other_end(u);
            if (is_removed[v]) continue;
            graph.remove_leaf_edge(v, u, idx);
            marker.inc(idx);
            if (graph.is_leaf(v)) leaves.push(v);
        }
    }
    for (int i = 0; i < chromosome.size(); i++)
        if (marker.get(chromosome[i])) chromosome.remove(i);
}

// Copied from original solution.hpp
vector<int> to_comp_id;
vector<vector<int>> Solution::get_components(cst(vector<int>) nodes) {
    sum_edges();
    int num_comps = 0;
    vector<vector<int>> comps;
    to_comp_id.assign(num_nodes + 1, -1);
    for (int u : nodes) {
        int rt = cc_handler.find_root(u);
        int comp_id;
        if (to_comp_id[rt] == -1) {
            comp_id = num_comps++;
            comps.emplace_back();
            to_comp_id[rt] = comp_id;
        } else comp_id = to_comp_id[rt];
        comps[comp_id].push_back(u);
    }
    return comps;
}

void Solution::connect_components(cst(vector<vector<int>>) comps) {
    // account for repeated edges !!!
    marker.tick();
    for (auto gene : chromosome) { marker.inc(gene); }
    vector<int> nodes;
    vector<bool> node_in_tree(num_nodes+5, false);
    auto add_node = [&] (int u) {
        if (!node_in_tree[u]) { node_in_tree[u] = true; nodes.push_back(u); }
    };
    auto labels = random_permutation(size(comps));
    for (auto u : comps[labels[0]-1]) add_node(u);
    const int NUM_ITERS = 16;
    for (int i = 1; i < size(comps); i++) {
        int min_dist = INF, ov, ou;
        // Using already-computed distance
        // * Choose random tangent point instead of an exact heuristics
        for (int _ = 0; _ < NUM_ITERS; _++) {
            int u = random_element(nodes);
            int dist = INF, v_nearest;
            for (auto v : comps[labels[i]-1]) {
                if (umin(dist, sp_handler.distance(u,v))) v_nearest = v;
            }
            if (umin(min_dist, dist)) ou = u, ov = v_nearest;
        }
        // Using Bellman-Ford SPFA
        /*
        auto dist = mssp_handler.calc_distance(nodes, i == 1);
        for (int v : comps[labels[i]-1]) {
            auto [dv, tangent] = dist[v];
            if (umin(min_dist, dv)) ov = v, ou = tangent;
        }
        */
        int oldSize = chromosome.curSize;
        sp_handler.trace_path(ou, ov, &chromosome, false);
        filter_repeat(oldSize, false);
        for (int j = oldSize; j < chromosome.size(); j++) {
            auto& [u,v,_] = edges[chromosome[j]];
            add_node(u);
            add_node(v);
        }
    }
}

void Solution::make_span() { connect_components(get_components(terminals)); }
vector<bool> is_node_involved;
void Solution::make_span_wide(Real r_drop = 0) {
    vector<int> nodes(terminals);
    is_node_involved.assign(num_nodes+1, false);
    for (int i = 0; i < chromosome.curSize; i++) {
        int idx = chromosome[i];
        auto& [u,v,w] = edges[idx];
        if (!is_terminal[u] or !is_terminal[v])
            possibly(r_drop, doing_nothing, [&] {
                is_node_involved[u] = is_node_involved[v] = true;
            });
    }
    for (int u = 1; u <= num_nodes; u++)
        if (is_node_involved[u] && !is_terminal[u]) nodes.push_back(u);
    connect_components(get_components(nodes));
}

std::ostream& operator<< (std::ostream& stream, Solution solution) {
    int n_edges = solution.chromosome.size();
    // solution.chromosome.debug();
    if (n_edges <= 200) {
        stream << "{";
        for (int idx : solution.chromosome) {
            auto& [u,v,w] = edges[idx];
            stream << "(" << u << ',' << v << ") ";
        };
        stream << "}\n";
    } else {
        stream << "|Size = " << n_edges << "|\n";
    }
    return stream;
}


#endif // SOLUTION_ARRAY_H
