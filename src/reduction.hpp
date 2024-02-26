#if !defined(REDUCTION_H)
#define REDUCTION_H

// Cite: KM96 Koch & Martin Solving STP in Graphs to Optimality
#include "problem.hpp"
#include "sdist.hpp"
#include "mst.hpp"
#include <queue>

// int added_cost; <-- this was move to GLOBAL!

Graph reduced_graph;
Gene active_edges; // remaining edges
vector<bool> is_removed; // for nodes

void get_terminals();
void refresh();
bool revalidate(bool flag);

void init_reduced_graph() {
    active_edges.resize(num_edges);
    bit::fill(all_of(active_edges), bit::bit1);
    reduced_graph.resize(num_nodes);
    reduced_graph.assign_subgraph(&active_edges);
    refresh();
}

void relabel_nodes_edges() {
    // Relabel nodes for Floyd?
    vector<int> labels(num_nodes + 1, 0);
    for (int idx = 0, u = 1; u <= num_nodes; u++)
        if (!is_removed[u]) labels[u] = ++idx;
    // for (int u = 1; u <= num_nodes; u++) {
    //     if (labels[u]) std::cerr << "\t" << u << " mapped into " << labels[u] << '\n';
    // }
    num_nodes = *max_element(all_of(labels));
    for (auto &t_i : terminals) t_i = labels[t_i];

    vector<Edge> new_edges;
    Iterate(active_edges, [&] (int idx) {
        new_edges.push_back(edges[idx]);
    });
    edges = new_edges;
    for (auto& [u,v,w] : edges)
        u = labels[u], v = labels[v];
    num_edges = edges.size();
    // for (auto [u,v,w] : edges)
    //     std::cerr << "\t" << u << ',' << v << ',' << w << '\n';

    Graph::init(&edges);
    init_reduced_graph();
    revalidate(false);
    initialization();
}

void remove_node(int u) {
    is_removed[u] = true; 
    is_terminal[u] = false;
}

void remove_edge(int idx) {
    active_edges[idx].set(false);
    assign_edge_index(edges[idx], -1);
}

void add_edge(int idx) {
    active_edges[idx].set(true);
    assign_edge_index(edges[idx], idx);
}

void contract_edge(int idx) {
    // node v is merged into node u
    auto [u,v,w] = edges[idx];
    added_cost += w;
    if (is_terminal[v]) is_terminal[u] = true;
    remove_node(v);
    remove_edge(idx);
    for (auto [id_vk, _] : reduced_graph[v]) {
        int k = edges[id_vk].other_end(v);
        if (k == u) continue;
        int w_vk = edges[id_vk].weight;
        int id_uk = get_edge_idx(u,k);
        if (id_uk != -1) {
            remove_edge(id_vk);
            umin(edges[id_uk].weight, w_vk);
            add_edge(id_uk);
        }
        else {
            edges[id_vk] = Edge(u,k, w_vk);
            add_edge(id_vk);
        }
    }
    for (int i = 0; i < num_edges; i++) if (active_edges[i]) {
        auto [fr,to,w] = edges[i];
        assert((fr != v && to != v)); // v is merged fully
    }
    get_terminals();
    assign_indices_for_edges(active_edges);
    reduced_graph.refresh();
    reduced_graph.compute_degree();
    reduced_graph.construct_adjacency_list();
    cout << "Graph after contracting edge (" << u << ',' << v << ")\n";
    Iterate(active_edges, [&] (int i) {
        auto [u,v,w] = edges[i];
        cout << "\t" << u << ' ' << v << ' ' << w << "\n";
    });
}

bool status_graph_updated;
void refresh() {
    reduced_graph.refresh();
    status_graph_updated = false;
}

void get_terminals() {
    terminals.clear();
    for (int u = 1; u <= num_nodes; u++)
        if (!is_removed[u] && is_terminal[u]) 
            terminals.push_back(u);
    num_terminals = terminals.size();
}

bool revalidate(bool do_terminal = true) {
    if (status_graph_updated) return false;
    status_graph_updated = true;
    if (do_terminal) get_terminals();
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
    int cnt_removed = 0;
    while (!leaves.empty()) {
        int u = leaves.front(); leaves.pop();
        for (auto [idx, edge] : reduced_graph[u]) {
            auto [fr, to, wei] = edges[idx];
            int v = fr^to^u;
            if (is_removed[v]) continue;
            ++cnt_removed;
            reduced_graph.remove_leaf_edge(v, u, idx);
            remove_node(u);
            remove_edge(idx);                
            if (reduced_graph.is_leaf(v)) leaves.push(v);
            if (is_terminal[u]) { // "hoists up" terminal
                is_terminal[u] = false;
                is_terminal[v] = true;
                ::added_cost += wei;
            }
        }
    }
    std::cerr << "Degree test " << cnt_removed << "\n";
    refresh();
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
    if (changed) std::cerr << "Special distance " << edges_to_remove.size() << "\n";
    return changed;
}

// This test can be seen as a specialization of the NSV test below 
bool nearest_vertex_test() {
    revalidate();
    vector<int> edges_to_contract;
    for (int k : terminals) {
        int second = -1, first = -1;
        for (auto [idx, _] : reduced_graph[k]) {
            if (second == -1 or edges[idx].weight <= edges[second].weight) {
                second = idx;
                if (first == -1 or edges[second].weight <= edges[first].weight) {
                    std::swap(first, second);
                }
            }
        } 
        if (first == -1) continue;
        int u = edges[first].other_end(k);
        int z = -1, dist = INF;
        for (int k2 : terminals) if (k2 != k)
            if (z == -1 or umin(dist, sp_handler.distance(u, k2))) z = k2;
        if (z == -1) continue;
        second = (second == -1) ? INF : edges[second].weight;
        if (edges[first].weight + dist <= second) {
            std::cerr << "\t" << k << ',' << u << ',' << z << " with " << edges[first].weight + dist << " compared to " << second << '\n';
            edges_to_contract.push_back(first);
        }
    }
    if (edges_to_contract.empty()) return false;
    std::cerr << "Current Special (|T| = " << num_terminals << ") = {";
    for (auto ti : terminals) std::cerr << ti << ' '; std::cerr << "}\n";
    cout << "The graph when doing the NV test\n";
    Iterate(active_edges, [&] (int i) {
        auto [u,v,w] = edges[i];
        cout << "\t" << u << ' ' << v << ' ' << w << "\n";
    });
    // remove duplication in case of consecutive special vertices
    sort(all_of(edges_to_contract));
    edges_to_contract.erase(unique(all_of(edges_to_contract)), end(edges_to_contract));
    std::cerr << "Nearest vertex test : " << size(edges_to_contract) << "\n";
    // for (auto e : edges_to_contract) contract_edge(e);
    contract_edge(edges_to_contract[0]);
    refresh();
    return true;
}

// Duin, C. W. and Volgenant, A. (1989b). Reduction tests for the steiner problem in 
// graphs. Networks, 19:549–567
bool nearest_speical_vertices_test() {
    vector<int> edges_to_contract;

    return false;
}

void input_preprocessing() {
    std::cerr << "Before reduction: " << num_nodes << ' ' << num_edges << ' ' << num_terminals << '\n';
    ::added_cost = 0;
    init_reduced_graph();
    is_removed.assign(num_nodes + 1, false);
    bool improved;
    do {
        improved = false;
        improved |= degree_test();
        
        improved |= special_distance_test();
        improved |= degree_test();
        
        improved |= nearest_vertex_test();
        // improved |= nearest_speical_vertices_test();
        
        improved |= special_distance_test();
        improved |= degree_test();
        
        improved |= special_distance_test();
        improved |= degree_test();
    } while (improved);
    relabel_nodes_edges();
    std::cerr << "After reduction: " << num_nodes << " " << num_edges << " " << num_terminals << '\n';
}

#endif // REDUCTION_H
