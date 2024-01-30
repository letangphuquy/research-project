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

class Solution
{
private:
    string address, id;
    int version;
    Gene gene; // gene
    Graph* pheno = nullptr;
    Int objval;
    bool objval_updated;
    void set_version(int ver) {
        version = ver; id = address + std::to_string(version);
    }
    Int sum_edges(void) {
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
public:
    Solution(): gene(Gene(num_edges, bit::bit1)) {
        address = std::to_string((unsigned long long) (void**) this); // https://stackoverflow.com/questions/7850125/convert-this-pointer-to-string
        set_version(0);
        objval = 0;
        objval_updated = false;
    }
    void get_graph_instance(void) {
        if (Graph::get_instance_owner() == id) return ;
        pheno = Graph::get_public_instance(id);
        pheno->resize(num_nodes);
        pheno->assign_subgraph(&gene);
    }
    void get_graph_reprsentation(void) { // to calc objval
        get_graph_instance();
        pheno->construct_adjacency_list();
    }
    Int get_objval(void) {
        if (objval_updated) return objval;
        objval_updated = true;
        objval = sum_edges();
        for (int i = 1; i < num_terminals; i++)
            if (!cc_handler.same_set(terminals[i], terminals[0]))
                return objval = INF;
        return objval;
    }
    void set_gene(cst(Gene) new_gene) {
        gene = new_gene;
        force_update();
    }
    friend std::ostream& operator<< (std::ostream& stream, Solution solution) {
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
    void force_update() {
        objval_updated = false;
        set_version(version + 1);
        get_graph_instance();
    }
    void reduce(Real r_fluctuate, bool is_biased);
    void make_span();
    void mutate();
    pair<Solution, Solution> crossover(Solution pal);
};
pair<Solution, Solution> Solution::crossover(Solution pal) {
    return std::make_pair(Solution(), Solution());
}

void Solution::reduce(Real r_fluctuate = 0, bool is_biased = false) {
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
}

vector<int> to_comp_id;
void Solution::make_span() {
    sum_edges();
    int num_comps = 0;
    vector<vector<int>> components;
    to_comp_id.assign(num_nodes + 1, -1);
    for (int i = 0; i < num_terminals; i++) {
        int si = terminals[i], rt = cc_handler.find_root(si);
        int comp_id;
        if (to_comp_id[rt] == -1) {
            comp_id = num_comps++;
            components.emplace_back();
            to_comp_id[rt] = comp_id;
        } else comp_id = to_comp_id[rt];
        components[comp_id].push_back(si);
    }
    auto labels = random_permutation(num_comps);
    for (int i = 1; i < num_comps; i++) {
        int u = labels[i]-1;
        int j = random_int(0, i-1);
        int v = labels[j]-1;
        u = random_element(components[u]);
        v = random_element(components[v]);
        sp_handler.trace_path(u, v, &gene, false);
    }
    reduce();
}

void Solution::mutate() {
    static int count = 0;
    int num_adds = std::max(2, int(num_edges * R_CHANGE));
    // if ((++count) % MUTATION_EPOCH_SIZE == 0)
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
}

#endif // SOLUTION_H
