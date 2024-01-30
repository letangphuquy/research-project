#if !defined(HEURISTICS_H)
#define HEURISTICS_H

#include "template.hpp"
#include "problem.hpp"
#include "solution.hpp"
#include "floyd.hpp"

Solution heuristics_random(void) { // stems from the fact :)
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

vector<Solution> heuristics_random_set(void) {
    int num_rand_heur = R_HEUR_RANDOM * POP_SIZE;
    vector<Solution> set;
    for (int i = 0; i < num_rand_heur; i++)
        set.push_back(heuristics_random());
    return set;
}

bool got_init_mst_heuristics;
Solution heuristics_mst_deterministic(void) {
    static Solution sol;
    if (!got_init_mst_heuristics) {
        vector<Edge> complete_subgraph;
        for (int i = 0; i < num_terminals; i++) {
            for (int j = i+1; j < num_terminals; j++) {
                int u = terminals[i];
                int v = terminals[j];
                complete_subgraph.push_back(Edge(u,v, sp_handler.distance(u,v)));
            }
        }
        sort(all_of(complete_subgraph));
        cc_handler.fill();
        Gene mst(num_edges, bit::bit0);
        for (auto [u,v,w] : complete_subgraph)
            if (cc_handler.merge_set(u,v)) {
                sp_handler.trace_path(u,v, &mst, false);
            }
        sol.set_gene(mst);
        sol.reduce();
    }
    return sol;
}

Solution heuristics_mst(void) {
    Solution sol = heuristics_mst_deterministic();
    sol.mutate();
    return sol;
}

vector<Solution> heuristics_mst_set(void) {
    got_init_mst_heuristics = false;
    int num_mst_heur = round(R_HEUR_MST * POP_SIZE);
    vector<Solution> set;
    for (int i = 0; i < num_mst_heur; i++) 
        set.push_back(heuristics_mst());
    return set;
}

#endif // HEURISTICS_H
