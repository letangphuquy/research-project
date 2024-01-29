#if !defined(SOLUTION_H)
#define SOLUTION_H

#include "template.hpp"
#include "bitlib/bitlib.hpp"
#include "dsu.hpp"
#include "mst.hpp"
#include "graph.hpp"
#include "problem.hpp"

// Solution's representation integrates tightly with problem 
// bit i ON : choose i-th edge in global list.

class Solution
{
private:
    string address;
    Gene gene; // gene
    Graph* pheno = nullptr;
    Int objval;
    bool objval_updated;
public:
    Solution() {
        address = std::to_string((unsigned long long) (void**) this); // https://stackoverflow.com/questions/7850125/convert-this-pointer-to-string
        objval = 0;
        objval_updated = false;
    }
    void add_edge(int idx) {
        auto [u,v,w] = edges[idx];
        pheno->add_edge(u,v, idx, &edges[idx]);
        cc_handler.merge_set(u,v);
        objval += w;
        objval_updated = false;
    }
    void get_graph_reprsentation(void) { // and calc. by the way
        if (Graph::get_instance_owner() == address) return ;
        pheno = Graph::get_public_instance(address);
        pheno->resize(num_nodes);
        cc_handler.init(num_nodes);
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
    void reduce();
    void trim_edges();
    void make_span();
    void mutate();
    pair<Solution, Solution> crossover(Solution pal);
};
pair<Solution, Solution> Solution::crossover(Solution pal) {
    return std::make_pair(Solution(), Solution());
}

#endif // SOLUTION_H
