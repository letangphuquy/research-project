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
public:
    Solution() {
        address = std::to_string((unsigned long long) (void**) this); // https://stackoverflow.com/questions/7850125/convert-this-pointer-to-string
        set_version(0);
        objval = 0;
        objval_updated = false;
    }
    void add_edge(int idx) {
        auto [u,v,w] = edges[idx];
        cc_handler.merge_set(u,v);
        objval += w;
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
        cc_handler.init(num_nodes);
        objval_updated = false;
        objval = 0;
        for (int i = 0; i < gene.size(); i++)
            if (gene[i] == bit::bit1) add_edge(i);
    }
    Int get_objval(void) {
        if (objval_updated) return objval;
        objval_updated = true;
        get_graph_reprsentation();
        for (int i = 1; i < num_terminals; i++)
            if (!cc_handler.same_set(terminals[i], terminals[0]))
                return objval = INF;
        return objval;
    }
    void set_gene(cst(Gene) new_gene) {
        gene = new_gene;
        force_update();
    }
    string debug(void) { return gene.debug_string(); }
    void force_update() {
        objval_updated = false;
        set_version(version + 1);
        get_graph_instance();
    }
    void reduce();
    void make_span();
    void mutate();
    pair<Solution, Solution> crossover(Solution pal);
};
pair<Solution, Solution> Solution::crossover(Solution pal) {
    return std::make_pair(Solution(), Solution());
}

void Solution::reduce() {
    cout << "Reducing " << gene.debug_string() << '\n';
    mst_handler.clear_bias();
    gene = mst_handler.calc_for(gene);
    cout << "After MST: " << gene.debug_string() << '\n';
    force_update();
    pheno->construct_adjacency_list();
    pheno->compute_degree();
    pheno->debug();
    pheno->to_remove.assign(num_nodes+1, false);
    std::queue<int> leaves;
    for (int u = 1; u <= num_nodes; u++) {
        if (pheno->is_leaf(u)) leaves.push(u);
    }
    cout << "Removing leafs: ";
    while (!leaves.empty()) {
        int u = leaves.front(); leaves.pop();
        if (is_terminal[u]) continue;
        cout << u << ' ';
        for (auto [idx, edge] : (*pheno)[u]) {
            auto [fr, to, wei] = *edge;
            int v = fr^to^u;
            if (pheno->to_remove[v]) continue;
            pheno->remove_leaf_edge(v, u, idx);
            if (pheno->is_leaf(v)) leaves.push(v);
        }
    }
    cout << '\n';
    force_update();
    pheno->construct_adjacency_list();
    pheno->debug();
}

#endif // SOLUTION_H
