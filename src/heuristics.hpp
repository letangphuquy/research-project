#if !defined(HEURISTICS_H)
#define HEURISTICS_H

#include "template.hpp"
#include "problem.hpp"
#include "solution.hpp"
#include "floyd.hpp"

const int N_MAX_HEUR = 1500;
#define TOO_MUCH_TERMINALS (num_terminals >= N_MAX_HEUR)

using HeuristicGenerator = std::function<Solution()>;
Social get_heuristics_set(HeuristicGenerator generator, cst(Real) RATIO, Void init = doing_nothing) {
    init();
    Social set;
    int num_heur_candidates = RATIO * POP_SIZE;
    for (int i = 0; i < num_heur_candidates; i++)
        set.push_back(generator());
    return set;
}

Solution heuristics_random() {
    Gene r_edges(num_edges);
    for (int i = 0; i < num_edges; i++)
        r_edges[i].set(random_int(0,1));
    if (random() < 0.7)
        return Solution(r_edges).make_span_wide(0.5).reduce();
    return Solution().make_span().reduce();
}

Solution heuristics_stem(void) { // stems from the fact :)
    vector<bool> in_tree(is_terminal);
    vector<int> tree_nodes(terminals);
    Real r_coverage = random(0,1);
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
    Solution sol(subgraph);
    possibly(0.5, 
        [&] { sol.make_span(); },
        [&] { sol.make_span_wide(); });
    return sol.reduce();
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
Solution heuristics_mst(void) { return heuristics_mst_deterministic().mutate(R_CHANGE); }

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
        int min_dist = INF;
        int node = order[i];
        for (auto u : tree_nodes)
            if (umin(min_dist, sp_handler.distance(u, node))) tangent = u;
        sp_handler.trace_path(tangent, node, &steiner_tree, false);
    }
    return Solution(steiner_tree);
}

Social heuristics_random_set(void) { return get_heuristics_set(heuristics_random, R_HEUR_RAND); }
Social heuristics_stem_set(void) { return get_heuristics_set(heuristics_stem, R_HEUR_STEM); }
Social heuristics_mst_set(void) {
    if (TOO_MUCH_TERMINALS) return Social();
    return get_heuristics_set(heuristics_mst, R_HEUR_MST, [&] { got_init_mst_heuristics = false; });
}
Social heuristics_sp_set(void) {
    if (TOO_MUCH_TERMINALS) return Social();
    return get_heuristics_set(heuristics_shortest_path, R_HEUR_SP, [&] { got_init_order_sp_heuristics = false; });
}
#endif // HEURISTICS_H
