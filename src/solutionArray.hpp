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

typedef pair<bool,int> ObjectiveValue; 
const ObjectiveValue UNCALC(false, 1e9);
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
    void selfCorrect(Real r_drop = 1, Real r_fluctuate = R_FLUCTUATE) {
        r_drop >= 1-EPS ? make_span() : make_span_wide(r_drop);
        reduce(r_fluctuate);
        objVal = UNCALC;
    }
public:
    int age;
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
        // std::cerr << "Assigning new chromosome\n";
        // chromo.debug();
        // chromosome.debug();
        chromosome = chromo; 
        objVal = UNCALC; 
    }
    int get_objval(void);
    int raw_objval(void) const { 
        // std::cerr << "Getting raw obj_val: " << objVal.first << ',' << objVal.second << '\n';
        return objVal.second; 
    }
    // bool operator< (Solution rhs) { return get_objval() < rhs.get_objval(); }
    bool operator< (cst(Solution) rhs) { return raw_objval() < rhs.raw_objval(); }
    bool operator== (cst(Solution) rhs);
    void mutate(Real pMutate);
    void mutate(void) { mutate(P_MUTATION); };
    pair<Solution, Solution> crossover(const Solution& mate);
    friend std::ostream& operator<< (std::ostream& stream, Solution solution);
};

bool Solution::operator== (cst(Solution) rhs) {
    if (chromosome.size() != rhs.chromosome.size()) return false;
    marker.tick();
    for (int i = 0; i < chromosome.size(); i++) marker.inc(chromosome[i]);
    for (int i = 0; i < chromosome.size(); i++)
        if (!marker.get(rhs.chromosome[i])) return false;
    return true;
}

int Solution::sum_edges() {
    int sum = 0;
    cc_handler.init(num_nodes);
    for (int i = 0; i < chromosome.size(); i++) {
        int e = chromosome[i];
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
    selfCorrect(0.5);
}

void Solution::filter_repeat(int start_index, bool begin) {
    if (begin) marker.tick();
    for (int i = start_index; i < chromosome.size(); ) {
        if (marker.get(chromosome[i])) chromosome.remove(i);
        else marker.inc(chromosome[i++]);
    }
}

void Solution::reduce(Real r_fluctuate) {
    // std::cerr << "Reducing\n";
    static vector<bool> is_removed;
    fromGenotype(mst_handler.calc_for(chromosome, r_fluctuate));
    // std::cerr << "\tDone MST\n";
    graph.resize(num_nodes);
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
    for (int i = 0; i < chromosome.size(); i++) { marker.inc(chromosome[i]); }
    auto labels = random_permutation(size(comps));
    for (int i = 1; i < size(comps); i++) {
        int u = labels[i]-1;
        int j = random_int(0, i-1);
        int v = labels[j]-1;
        u = random_element(comps[u]);
        v = random_element(comps[v]);
        int oldSize = chromosome.curSize;
        sp_handler.trace_path(u, v, &chromosome, false);
        filter_repeat(oldSize, false);
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
        for (int i = 0; i < solution.chromosome.size(); i++) {
            int idx = solution.chromosome[i];
            auto& [u,v,w] = edges[solution.chromosome[i]];
            stream << "(" << u << ',' << v << ") ";
        };
        stream << "}\n";
    } else {
        stream << "|Size = " << n_edges << "|\n";
    }
    return stream;
}


#endif // SOLUTION_ARRAY_H
