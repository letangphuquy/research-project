#if !defined(SOLUTION_H)
#define SOLUTION_H

#include "template.hpp"
#include "bitlib/bitlib.hpp"
#include "dsu.hpp"
#include "mst.hpp"
#include "graph.hpp"
#include "problem.hpp"
#include <queue>

// Solution's representation integrates tightly with problem 
// bit i ON : choose i-th edge in global list.

// WARNING: Dangerous reference passing around for better speed? (reduce copy time).
const auto OPER_XOR = std::bit_xor<WordType>();
const auto OPER_FLIP = std::bit_not<WordType>();
class Solution
{
private:
    string address,id;
    int version;
    Gene gene;
    Graph* pheno = nullptr;
    Int objval;
    bool objval_updated;
    void set_version(int ver) { version = ver; id = address + std::to_string(version); }
    Int sum_edges(void);
    vector<vector<int>> get_components(cst(vector<int>) nodes);
    void connect_components(cst(vector<vector<int>>) comps);

    static Gene temp_gene;
public:
    Solution(Gene dna) {
        temp_gene.resize(num_edges);
        address = std::to_string((unsigned long long) (void**) this); // https://stackoverflow.com/questions/7850125/convert-this-pointer-to-string
        set_version(0);
        objval = 0;
        objval_updated = false;
        set_gene(dna);
    }
    Solution(): Solution(Gene(num_edges, bit::bit1)) {}
    void get_graph_instance(void);
    void force_update();
    
    Int get_objval(void);
    bool operator< (Solution rhs) {  return get_objval() < rhs.get_objval(); }
    void set_gene(cst(Gene) new_gene) { gene = new_gene; force_update(); }
    Solution& reduce(Real r_fluctuate, bool is_biased);
    Solution& make_span(); // terminals only
    Solution& make_span_wide(); // some distinct components, also
    Solution& mutate(Real r_change);
    pair<Solution, Solution> crossover(Solution& pal);
    int distance_to(Solution rhs) {
        bit::transform(all_of(gene), begin(rhs.gene), begin(temp_gene), OPER_XOR);
        return bit::count(all_of(temp_gene), bit::bit1);
    }
    friend std::ostream& operator<< (std::ostream& stream, Solution solution);
};
Gene Solution::temp_gene = Gene();
typedef vector<Solution> Social;

void Solution::get_graph_instance() {
    if (Graph::get_instance_owner() == id) return ;
    pheno = Graph::get_public_instance(id);
    pheno->resize(num_nodes);
    pheno->assign_subgraph(&gene);
}
void Solution::force_update() {
    objval_updated = false;
    set_version(version + 1);
    get_graph_instance();
}

Int Solution::sum_edges(void) {
    Int sum = 0;
    cc_handler.init(num_nodes);
    for (int i = 0; i < gene.size(); i++) 
    if (gene[i]) {
        auto [u,v,w] = edges[i];
        cc_handler.merge_set(u,v);
        sum += w;
    }
    return sum;
}

Int Solution::get_objval(void) {
    if (objval_updated) return objval;
    objval_updated = true;
    objval = sum_edges();
    for (int i = 1; i < num_terminals; i++)
        if (!cc_handler.same_set(terminals[i], terminals[0]))
            return objval = INF;
    return objval;
}

Solution& Solution::reduce(Real r_fluctuate = 0, bool is_biased = false) {
    if (!is_biased) mst_handler.clear_bias();
    set_gene(mst_handler.calc_for(gene, r_fluctuate));
    pheno->construct_adjacency_list();
    pheno->compute_degree();
    pheno->to_remove.assign(num_nodes+1, false);
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
            if (pheno->to_remove[v]) continue;
            pheno->remove_leaf_edge(v, u, idx);
            if (pheno->is_leaf(v)) leaves.push(v);
        }
    }
    force_update();
    return *this;
}

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
    auto labels = random_permutation(size(comps));
    for (int i = 1; i < size(comps); i++) {
        int u = labels[i]-1;
        int j = random_int(0, i-1);
        int v = labels[j]-1;
        u = random_element(comps[u]);
        v = random_element(comps[v]);
        sp_handler.trace_path(u, v, &gene, false);
    }
    reduce();
}

Solution& Solution::make_span() {
    sum_edges();
    connect_components(get_components(terminals));
    return *this;
}
vector<bool> is_node_involved;
Solution& Solution::make_span_wide() {
    vector<int> nodes(terminals);
    is_node_involved.assign(num_nodes+1, false);
    for (int i = 0; i < num_edges; i++)
        if (gene[i]) {
            auto [u,v,w] = edges[i];
            is_node_involved[u] = is_node_involved[v] = true;
        }
    for (int u = 1; u <= num_nodes; u++)
        if (is_node_involved[u] && !is_terminal[u]) nodes.push_back(u);
    connect_components(get_components(nodes));
    return *this;
}

Solution& Solution::mutate(Real r_change = R_CHANGE) {
    static int count = 0;
    int num_adds = std::max(2, int(num_edges * r_change));
    permute(rand_order);
    for (auto idx : rand_order) {
        if (!gene[idx]) {
            gene[idx].set(true);
            mst_handler.change_bias(idx);
            if ((--num_adds) <= 0) break;
        }
    }
    reduce(R_FLUCTUATE, true);
    mst_handler.clear_bias();
    return *this;
}

pair<Solution,Solution> Solution::crossover(Solution& pal) {
    pair<Solution,Solution> children;
    for (int i = 0; i < num_edges; i++) {
        temp_gene[i] = ((random_num(1,100) <= 50) ? this->gene[i] : pal.gene[i]);
    }
    auto assign_solution = [&] (Solution& child) {
        child.set_gene(temp_gene);
        child.make_span_wide();
        possibly(P_MUTATION, [&] { child.mutate(); });
    };
    assign_solution(children.first);
    // pa XOR ma XOR child1 = child2
    bit::transform(all_of(gene), begin(pal.gene), begin(temp_gene), OPER_XOR);
    bit::transform(all_of(temp_gene), begin(children.first.gene), begin(children.second.gene), OPER_XOR);
    // bit::transform(all_of(temp_gene), children.second.gene.begin(), OPER_FLIP);
    assign_solution(children.second);
    return children;
}

std::ostream& operator<< (std::ostream& stream, Solution solution) {
    stream << "{";
    for (int i = 0; i < solution.gene.size(); i++) {
        if (solution.gene[i]) {
            auto [u,v,w] = edges[i];
            stream << "(" << u << ',' << v << ") ";
        }
    }
    stream << "}\n";
    return stream; 
}

#endif // SOLUTION_H
