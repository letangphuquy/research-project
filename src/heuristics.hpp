#if !defined(HEURISTICS_H)
#define HEURISTICS_H

#include "template.hpp"
#include "problem.hpp"
#include "solution.hpp"
#include "floyd.hpp"

const int N_MAX_HEUR = 1500;
#define TOO_MUCH_TERMINALS (num_terminals >= N_MAX_HEUR)

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
    vector<Solution> set;
    int num_rand_heur = R_HEUR_RANDOM * POP_SIZE;
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
    vector<Solution> set;
    if (TOO_MUCH_TERMINALS) return set;
    got_init_mst_heuristics = false;
    int num_mst_heur = round(R_HEUR_MST * POP_SIZE);
    for (int i = 0; i < num_mst_heur; i++) 
        set.push_back(heuristics_mst());
    return set;
}

bool got_init_order_sp_heuristics;
Solution heuristics_shortest_path() {
    static vector<int> order;
    if (!got_init_order_sp_heuristics) {
        order = vector<int>(terminals);
        got_init_order_sp_heuristics = true;
    }
    permute(order);
    vector<bool> in_tree(num_nodes+1, false);
    vector<int> tree_nodes({order[0]});
    in_tree[order[0]] = true;
    Gene steiner_tree(num_edges, bit::bit0);
    for (int i = 1; i < num_terminals; i++) {
        int tangent = -1;
        Int min_dist = INF;
        int node = order[i];
        for (auto u : tree_nodes)
            if (umin(min_dist, sp_handler.distance(u, node))) tangent = u;
        sp_handler.trace_path(tangent, node, &steiner_tree, false);
    }
    Solution sol;
    sol.set_gene(steiner_tree);
    return sol;
}

vector<Solution> heuristics_sp_set(void) {
    vector<Solution> set;
    if (TOO_MUCH_TERMINALS) return set;
    got_init_order_sp_heuristics = false;
    int num_sp_heur = round(R_HEUR_SP * POP_SIZE);
    for (int i = 0; i < num_sp_heur; i++) 
        set.push_back(heuristics_shortest_path());
    return set;
}

#endif // HEURISTICS_H
