#if !defined(REDUCTION_H)
#define REDUCTION_H

// Cite: KM96 Koch & Martin Solving STP in Graphs to Optimality
#include "problem.hpp"
#include "sdist.hpp"
#include "mst.hpp"
#include <queue>
#include <set>

// int added_cost; <-- this was move to GLOBAL!

Graph reduced_graph;
Gene active_edges; // remaining edges
vector<bool> is_removed; // for nodes

int count_edges() { return bit::count(all_of(active_edges), bit::bit1); }
void get_terminals();
// to signify an update
void refresh(); 
// must be called after each successful test, or used to require updated info
bool revalidate(bool upd_terminal, bool upd_sp, bool upd_sd); 

// also include a refresh AND revalidate() call
void init_reduced_graph() {
    active_edges.resize(num_edges);
    bit::fill(all_of(active_edges), bit::bit1);
    reduced_graph.resize(num_nodes);
    reduced_graph.assign_subgraph(&active_edges);
    refresh();
    assert(revalidate(1,1,1)); // initialization
}

void relabel_nodes_edges() {
    vector<int> labels(num_nodes + 1, 0);
    for (int idx = 0, u = 1; u <= num_nodes; u++)
        if (!is_removed[u]) labels[u] = ++idx;
    num_nodes = *max_element(all_of(labels));
    num_terminals = terminals.size();
    for (auto &t_i : terminals) assert(t_i = labels[t_i]);
    
    is_removed.assign(num_nodes + 1, false);
    is_terminal.assign(num_nodes + 1, false);
    for (auto si : terminals) is_terminal[si] = true;

    vector<Edge> new_edges;
    Iterate(active_edges, [&] (int idx) {
        auto [u,v,w] = edges[idx];
        u = labels[u], v = labels[v];
        if (u && v)
            new_edges.push_back(Edge(u, v, w));
    });
    edges = new_edges;
    sort(all_of(edges));
    num_edges = edges.size();
    // Graph::init(&edges);
    init_reduced_graph();
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

bool revalidate(bool upd_terminal, bool upd_sp, bool upd_sd) {
    if (status_graph_updated) return false;
    status_graph_updated = true;
    // std::cerr << "\trevalidate(" << upd_terminal << ',' << upd_sp << ',' << upd_sd << ")\n";
    if (upd_terminal) get_terminals();
    assign_indices_for_edges(active_edges);
    reduced_graph.compute_degree();
    reduced_graph.construct_adjacency_list();
    if (upd_sp) 
        sp_handler.calc_for(reduced_graph);
    if (upd_sd)
        SD_handler.calc_for(reduced_graph);
    return true;
}

// [Refresh] Only use as a sub-routine for degree_test, not for standalone use as it will leave distance un-updated
bool degree_one_test(void) {
    revalidate(1, 0, 0);
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
            if (reduced_graph.is_leaf(v)) leaves.push(v);
            if (is_terminal[u]) { // "hoists up" terminal
                is_terminal[u] = false;
                is_terminal[v] = true;
                ::added_cost += wei;
            }
            remove_node(u); // FUCK, I must move this line after here in order for it to work !!!
            remove_edge(idx);                
        }
    }
    if (!cnt_removed) return false;
    std::cerr << "Degree test (Leaves) " << cnt_removed << "\n";
    cout << "Degree test (Leaves) " << cnt_removed << "\n";
    refresh();
    return true;
}

// [Refresh] Same note for degree_one_test, and must be called after it !!
bool degree_two_test() {
    revalidate(1, 0, 0);
    vector<bool> done(num_nodes+1, 0);
    vector<bool> is_key(num_nodes+1, 0);
    vector<vector<int>> bounds(num_nodes+1); // 1 or 2 bound
    for (int u = 1; u <= num_nodes; u++) {
        is_key[u] = (reduced_graph.deg(u) > 2 or is_terminal[u]);
        // all nodes are now of degree >= 2, so a non-key node is trivially a "line"-node
    }
    // implementation: all "line"-nodes along a key-path are contracted into a single edge
    for (int u = 1; u <= num_nodes; u++) {
        if (is_key[u]) continue;
        for (auto [idx, edge] : reduced_graph[u]) {
            int v = edge->other_end(u);
            if (is_key[v]) bounds[u].push_back(idx);
        }
    }
    int cnt_removed = 0;
    for (int u = 1; u <= num_nodes; u++) {
        if (is_key[u] or done[u] or bounds[u].empty()) continue;
        int chain_length = 0, bound_idx = -1;
        vector<int> keys;
        for (int node = u; !done[node]; ) {
            cnt_removed += 1;
            done[node] = true;
            remove_node(node);
            for (auto idx : bounds[node]) {
                chain_length += edges[idx].weight;
                bound_idx = idx;
                keys.push_back(edges[idx].other_end(node));
            }
            int next = -1;
            for (auto [idx, edge] : reduced_graph[node]) {
                remove_edge(idx);
                int v = edge->other_end(node);
                if (is_key[v] or done[v]) continue;
                next = v;
                chain_length += edges[idx].weight;
            }
            if (next == -1) break;
            node = next;
        }
        assert((bound_idx != -1 && keys.size() == 2));
        edges[bound_idx] = Edge(keys[0], keys[1], chain_length);
        add_edge(bound_idx); // replaces the whole key-path with one edge
    }
    if (!cnt_removed) return false;
    std::cerr << "Degree test (Key-paths) " << cnt_removed << "\n";
    cout << "Degree test (Key-paths) " << cnt_removed << "\n";
    refresh();
    return true;
}

// Removes node with degree <= 2, then update both SP and SD
bool degree_test() {
    // essentialy the same as "Solution.reduce()"
    bool state = degree_one_test();
    if (degree_two_test()) state = true;
    if (state) {
        refresh();
        assert(revalidate(1, 1, 1));
    }
    // std::cerr << "\t" << state << " and " << revalidate(1, 1, 1) << '\n';
    return state;
}

// ??? Observation: After SD-test SD has not increased, but SP CAN  !!!
bool special_distance_test() {
    revalidate(1, 1, 1);
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
    if (changed) {
        refresh();
        assert(revalidate(1, 1, 0));
        std::cerr << "Special distance " << edges_to_remove.size() << "\n";
        cout << "Special distance " << edges_to_remove.size() << "\n";
    }
    return changed;
}

void contract_edge(int idx) {
    // node v is merged into node u
    auto [u,v,w] = edges[idx];
    ::added_cost += w;
    if (is_terminal[v]) is_terminal[u] = true;
    remove_node(v);
    remove_edge(idx);
    for (auto [id_vk, _] : reduced_graph[v]) {
        int k = edges[id_vk].other_end(v);
        if (k == u) continue;
        int w_vk = edges[id_vk].weight;
        int id_uk = get_edge_idx(u,k);
        remove_edge(id_vk);
        if (id_uk != -1) {
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
    assign_indices_for_edges(active_edges);
    refresh();
    assert(revalidate(1, 0, 0)); // update structure only

    // cout << "Graph after contracting edge (" << u << ',' << v << ")\n";
    // Iterate(active_edges, [&] (int i) {
    //     auto [u,v,w] = edges[i];
    //     cout << "\t" << u << ' ' << v << ' ' << w << "\n";
    // });
}

// This test can be seen as a specialization of the NSV test below 
// [Refresh] SP and SD can reduce because of edge contractions. 
bool nearest_vertex_test() {
    revalidate(1,1,1);
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
            // std::cerr << "\t" << k << ',' << u << ',' << z << " with " << edges[first].weight + dist << " compared to " << second << '\n';
            edges_to_contract.push_back(first);
        }
    }
    if (edges_to_contract.empty()) return false;
    // cout << "The graph when doing the NV test\n";
    // Iterate(active_edges, [&] (int i) {
    //     auto [u,v,w] = edges[i];
    //     cout << "\t" << u << ' ' << v << ' ' << w << "\n";
    // });
    // remove duplication in case of consecutive special vertices
    sort(all_of(edges_to_contract));
    edges_to_contract.erase(unique(all_of(edges_to_contract)), end(edges_to_contract));
    std::cerr << "Nearest vertex test : " << size(edges_to_contract) << "\n";
    cout << "Nearest vertex test : " << size(edges_to_contract) << "\n";
    for (auto e : edges_to_contract) contract_edge(e);
    refresh();
    std::cerr << "\tGraph after contracting the edges : " << count_edges() << "\n";
    cout << "\tGraph after contracting the edges : " << count_edges() << "\n";
    return true;
}

namespace NSV_Test {
    int ROOT = 1;
    Graph tree;
    Gene mst;

    int lca[N_MAX][N_MAX];
    vector<int> repr; // the LCA
    vector<int> par_edge; // this's also used for visited checking
    vector<vector<int>> events; // x > 0: add x to set. x < 0: remove -x from set
    vector<int> min_chord;

    void get_tree(void) {
        mst = mst_handler.calc_for(active_edges);
        tree.assign_subgraph(&mst);
        tree.resize(num_nodes);
        tree.construct_adjacency_list();
        for (int u = 1; u <= num_nodes; u++)
            if (!is_removed[u]) ROOT = u;
    }

    // compute lca(u,v) for all pair (u,v) and construct tree structure
    // https://cp-algorithms.com/graph/lca_tarjan.html
    // https://scholar.google.com/citations?view_op=view_citation&hl=en&user=lazJixIAAAAJ&citation_for_view=lazJixIAAAAJ:YsMSGLbcyi4C
    void dfs_lca(int u) {
        repr[u] = u;
        for (auto [idx, edge] : tree[u]) {
            int v = edge->other_end(u);
            if (par_edge[v] != -1) continue;
            par_edge[v] = idx;
            // std::cerr << u << ' ' << v << " is tree arc\n";
            dfs_lca(v);
            cc_handler.merge_set(v,u);
            // std::cerr << " after merge branch " << v << " to " << u << ", repr[" << cc_handler.find_root(u) << "] = " << u << "\n";
            repr[cc_handler.find_root(u)] = u;
        }
        for (int v = 1; v <= num_nodes; v++)
            if (par_edge[v] != -1) {
                // std::cerr << "\tpair (" << v << ',' << u << ") = " << cc_handler.find_root(v) 
                //     << "; therefore " << repr[cc_handler.find_root(v)] << '\n';
                lca[u][v] = lca[v][u] = repr[cc_handler.find_root(v)];
            }
    }

    void prepare_tree_and_lca(void) {
        cc_handler.init(num_nodes);
        repr.assign(num_nodes + 1, -1);
        par_edge.assign(num_nodes + 1, -1);
        par_edge[ROOT] = -2; // visited
        dfs_lca(ROOT);
    }

    void consider_chord(int idx) {
        auto [u,v,w] = edges[idx];
        events[u].push_back(w);
        events[v].push_back(w);
        int l = lca[u][v];
        events[l].push_back(-w);
        events[l].push_back(-w);
        // std::cerr << "\tchord " << idx << " with weight " << w << " lies on " << u << ',' << v << ',' << l << '\n';
    }

    std::multiset<int> active_chords;
    void dfs_chords(int u) {
        for (auto [idx, edge] : tree[u]) {
            int v = edge->other_end(u);
            if (par_edge[v] != idx) continue;
            dfs_chords(v);
        }
        for (auto x : events[u])
            if (x > 0) active_chords.insert(x);
            else active_chords.erase(active_chords.find(-x));

        int pa = par_edge[u];
        if (pa >= 0 && active_chords.size()) {
            umin(min_chord[pa], *active_chords.begin());
            // std::cerr << "\tmin_chord(" << u << "[" << pa << "]) = " << min_chord[pa] << '\n';
        }
    }

    void consider_all_chords(void) {
        min_chord.assign(num_edges, INF);
        events.clear();
        events.resize(num_nodes + 1);
        Iterate(active_edges, [&] (int idx) {
            if (!mst[idx]) consider_chord(idx);
        });
        active_chords.clear();
        dfs_chords(ROOT);
    }

    vector<int> nsv_edges() {
        vector<int> ret;
        Iterate(mst, [&] (int idx) {
            auto [u,v,w] = edges[idx];
            int d_u = INF, d_v = INF;
            for (auto k : terminals) {
                int d_ku = sp_handler.distance(k,u);
                int d_kv = sp_handler.distance(k,v);
                if (d_ku < d_kv) umin(d_u, d_ku);
                else umin(d_v, d_kv);
            }
            if (d_u != INF && d_v != INF && d_u + w + d_v <= min_chord[idx]) {
                std::cerr << "\tEdge (" << u << ',' << v << ") with " << d_u << " + " << w << " + " << d_v << " = " << d_u + w + d_v << " compared to " << min_chord[idx] << '\n'; 
                cout << "\tEdge (" << u << ',' << v << ") with " << d_u << " + " << w << " + " << d_v << " = " << d_u + w + d_v << " compared to " << min_chord[idx] << '\n'; 
                ret.push_back(idx);
            }
        });
        return ret;
    }
};

// Duin, C. W. and Volgenant, A. (1989b). Reduction tests for the steiner problem in 
// graphs. Networks, 19:549–567
bool nearest_speical_vertices_test() {
    revalidate(1, 1, 1);
    
    NSV_Test::get_tree();
    NSV_Test::prepare_tree_and_lca();
    NSV_Test::consider_all_chords();
    vector<int> edges_to_contract(NSV_Test::nsv_edges());
    if (edges_to_contract.empty()) return false;
    std::cerr << "Nearest Special Vertices: " << size(edges_to_contract) << '\n';
    cout << "Nearest Special Vertices: " << size(edges_to_contract) << '\n';
    for (auto e : edges_to_contract) contract_edge(e);
    refresh();
    std::cerr << "\tGraph after contracting the edges : " << count_edges() << "\n";
    cout << "\tGraph after contracting the edges : " << count_edges() << "\n";
    return true;
}

void input_preprocessing() {
    std::cerr << "Before reduction: " << num_nodes << ' ' << num_edges << ' ' << num_terminals << '\n';
    ::added_cost = 0;
    is_removed.assign(num_nodes + 1, false);
    refresh();
    init_reduced_graph();
    bool improved;
    do {
        improved = false;
        improved |= degree_test();
        
        improved |= special_distance_test();
        improved |= degree_test();
        
        if (nearest_speical_vertices_test()) {
            improved = true;
            // relabel_nodes_edges();
        }
        
        improved |= special_distance_test();
        improved |= degree_test();
        
        improved |= special_distance_test();
        improved |= degree_test();

        improved |= nearest_vertex_test();
        // relabel_nodes_edges(); // increase speed for later distance matroid re-computation(s)
    } while (improved);
    relabel_nodes_edges();
    std::cerr << "After reduction: " << num_nodes << " " << num_edges << " " << num_terminals << '\n';
}

#endif // REDUCTION_H
