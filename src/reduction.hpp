#if !defined(REDUCTION_H)
#define REDUCTION_H

// Cite: KM96 Koch & Martin Solving STP in Graphs to Optimality
#include "problem.hpp"
#include "sdist.hpp"
#include <queue>

int added_cost;
Graph reduced_graph;
Gene active_edges; // remaining edges
vector<bool> is_removed; // for nodes

void relabel_nodes_edges() {
    graph;
}

void remove_node(int u) {
    is_removed[u] = true;
}

void remove_edge(int idx) {
    active_edges[idx].set(false);
}

void revalidate() {
    terminals.clear();
    for (int u = 1; u <= num_nodes; u++)
        if (!is_removed[u] && is_terminal[u]) 
            terminals.push_back(u);
    num_terminals = terminals.size();
    reduced_graph.compute_degree();
    reduced_graph.construct_adjacency_list();
}

bool degree_test() {
    // essentialy the same as "Solution.reduce()"
    revalidate();
    std::queue<int> leaves;
    for (int u = 1; u <= num_nodes; u++) {
        if (is_removed[u]) continue;
        if (reduced_graph.is_leaf(u)) leaves.push(u);
    }
    if (leaves.empty()) return false;
    while (!leaves.empty()) {
        int u = leaves.front(); leaves.pop();
        for (auto [idx, edge] : reduced_graph[u]) {
            auto [fr, to, wei] = *edge;
            int v = fr^to^u;
            if (is_removed[v]) continue;
            reduced_graph.remove_leaf_edge(v, u, idx);
            remove_node(u);
            remove_edge(idx);                
            if (reduced_graph.is_leaf(v)) leaves.push(v);
            if (is_terminal[u]) { // "hoists up" terminal
                is_terminal[u] = false;
                is_terminal[v] = true;
                added_cost += wei;
            }
        }
    }
    reduced_graph.refresh();
    return true;
}

bool special_distance_test() {
    revalidate();
    sp_handler.calc_for(reduced_graph);
    SD_handler.calc_for(reduced_graph);
    bool changed = false;
    Iterate(active_edges, [&] (int idx) {
        auto& [u,v,w] = edges[idx];
        if (SD_handler.distance(u,v) < w) {
            remove_edge(idx);
            changed = true;
        } 
    });
    reduced_graph.refresh();
    return changed;
}

bool terminal_distance_test() {
    return false;
}

void input_preprocessing() {
    added_cost = 0;
    active_edges.resize(num_edges);
    bit::fill(all_of(active_edges), bit::bit1);
    reduced_graph.resize(num_nodes);
    reduced_graph.assign_subgraph(&active_edges);
    is_removed.assign(num_nodes + 1, false);
    bool improved;
    do {
        improved = false;
        improved |= degree_test();
        
        improved |= special_distance_test();
        improved |= degree_test();
        
        improved |= terminal_distance_test();
        
        improved |= special_distance_test();
        improved |= degree_test();
        
        improved |= special_distance_test();
        improved |= degree_test();
    } while (improved);
    int size = bit::count(all_of(active_edges), bit::bit1);
    cout << "After reduction: " << num_nodes << " " << size << " " << num_terminals << '\n';
    relabel_nodes_edges();
}

#endif // REDUCTION_H
