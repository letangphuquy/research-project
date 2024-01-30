#if !defined(HEURISTICS_H)
#define HEURISTICS_H

#include "template.hpp"
#include "problem.hpp"
#include "solution.hpp"

Solution random_heuristics() { // stems from the fact :)
    vector<bool> in_tree(is_terminal);
    vector<int> tree_nodes(terminals);
    Real r_coverage = random_num(0.2, 0.6);
    int n_coverage = r_coverage * num_edges;
    Gene subgraph(num_edges, bit::bit0);
    for (int _ = 0; _ < n_coverage; _++) {
        int u = random_element(tree_nodes);
        auto [idx, edge] = random_element(graph[u]);
        auto [fr,to, w] = *edge;
        int v = fr ^ to ^ u;
        if(!in_tree[v]) {
            subgraph[idx].set(true);
            in_tree[v] = true;
            tree_nodes.push_back(v);
        }
    }
    Solution sol;
    sol.set_gene(subgraph);
    sol.make_span();
    return sol;
}

#endif // HEURISTICS_H
