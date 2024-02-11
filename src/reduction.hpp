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
    // graph;
}

void remove_node(int u) {
    is_removed[u] = true;
}

void remove_edge(int idx) {
    active_edges[idx].set(false);
}

bool status_graph_updated;
void refresh() {
    reduced_graph.refresh();
    status_graph_updated = false;
}

bool revalidate() {
    if (status_graph_updated) return false;
    status_graph_updated = true;
    terminals.clear();
    for (int u = 1; u <= num_nodes; u++)
        if (!is_removed[u] && is_terminal[u]) 
            terminals.push_back(u);
    num_terminals = terminals.size();
    reduced_graph.compute_degree();
    reduced_graph.construct_adjacency_list();
    sp_handler.calc_for(reduced_graph);
    SD_handler.calc_for(reduced_graph);
    return true;
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
    refresh();
    std::cerr << "Trim leaves\n";
    return true;
}

bool special_distance_test() {
    revalidate();
    bool changed = false;
    vector<int> edges_to_remove;
    Iterate(active_edges, [&] (int idx) {
        auto& [u,v,w] = edges[idx];
        if (SD_handler.distance(u,v) < w) {
            // std::cerr << "remove " << idx << " : " << SD_handler.distance(u,v) << " vs. " << w << '\n';
            edges_to_remove.push_back(idx);
            changed = true;
        } 
    });
    for (auto idx : edges_to_remove) remove_edge(idx);
    if (changed) refresh();
    if (changed) std::cerr << "Special distance\n";
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
    refresh();
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
