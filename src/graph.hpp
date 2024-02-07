#if !defined(GRAPH_H)
#define GRAPH_H

#include "template.hpp"
struct Arc {
    int to, weight;
    Arc(int to, int weight): to(to), weight(weight) {}
};

struct Edge 
{
    int from, to, weight;
    Edge(int from, int to, int weight): from(from), to(to), weight(weight) {}
    bool operator< (cst(Edge) rhs) const {
        return weight < rhs.weight;
    }
};

// stores edge's ID and ref. (s) in adjacency list
using Link = pair<int, Edge*>;
class Graph;
class Graph
{
private:
    static vector<Edge>* edge_set;
    Gene* subgraph;
    bool has_degree, has_adj;

    int V;
    vector<int> degree; //out-degree
    vector<vector<Link>> adj;

    static Graph* instance;
    static string owner;
    void add_arc(int u, int idx);
public:
    vector<bool> to_remove; // for leaves trimmer
    Graph() { 
        subgraph = nullptr;
        refresh();
    }
    const vector<Link>& operator[] (int u) const { return adj[u]; } // readonly field
    int size(void) const { return V; }
    static Graph* get_public_instance(string id) {
        owner = id;
        if (instance == nullptr) instance = new Graph();
        instance->refresh();
        return instance;
    }
    static string get_instance_owner(void) { return owner; }
    
    static void init(vector<Edge>* ref) { edge_set = ref; }
    void assign_subgraph(Gene* gene) {
        subgraph = gene;
        refresh();
    }
    void resize(int nV) { 
        adj.resize((V = nV) + 1); 
        degree.resize(V + 1);
    }
    void refresh() { has_degree = has_adj = false; }

    void compute_degree(void) {
        if (has_degree) return;
        has_degree = true;
        fill(all_of(degree), 0);
        Iterate(*subgraph, [&] (int idx) {
            auto& edge = (*edge_set)[idx];
            ++degree[edge.from];
            ++degree[edge.to];
        });

        // iterate(*subgraph) if (!bit0) {
        //     auto& edge = (*edge_set)[idx];
        //     ++degree[edge.from];
        //     ++degree[edge.to];
        // }
        // }}

        // for (int i = 0; i < subgraph->size(); i++) {
        //     if ((*subgraph)[i]) {
        //         auto edge = (*edge_set)[i];
        //         ++degree[edge.from];
        //         ++degree[edge.to];
        //     }
        // }
    }
    void construct_adjacency_list(void) {
        if (has_adj) return ;
        has_adj = true;
        for (auto &neigh : adj) neigh.clear();
        Iterate(*subgraph, [&] (int idx) {
            auto& edge = (*edge_set)[idx];
            add_arc(edge.from, idx);
            add_arc(edge.to, idx);
        });

        // iterate(*subgraph) if (!bit0) {
        //     auto& edge = (*edge_set)[idx];
        //     add_arc(edge.from, idx);
        //     add_arc(edge.to, idx);
        // }
        // }}

        // for (int i = 0; i < subgraph->size(); i++) 
        //     if ((*subgraph)[i]) {
        //         auto edge = (*edge_set)[i];
        //         add_arc(edge.from, i);
        //         add_arc(edge.to, i);
        //     }
    }
    bool is_leaf(int u) const { return degree[u] == 1; }
    void remove_leaf_edge(int par, int leaf, int idx) {
        (*subgraph)[idx].set(false);
        --degree[par];
        --degree[leaf];
        to_remove[leaf] = true;
    }
    void debug(void) {
        construct_adjacency_list();
        cout << "Graph:\n";
        for (int u = 1; u <= V; u++) {
            cout << "\tg(" << u << "): ";
            for (auto [idx,edge] : adj[u]) {
                auto [fr,to,_] = *edge;
                int v = u^fr^to;
                cout << v << ' ';
            }
            cout << '\n';
        }
    }
};
vector<Edge>* Graph::edge_set = nullptr;
Graph* Graph::instance = nullptr;
string Graph::owner = "";
//Knowledge: All static member must be declared externally, after the class definition
//https://itecnote.com/tecnote/c-a-member-with-an-in-class-initializer-must-be-const/
void Graph::add_arc(int from, int idx) {
    adj[from].push_back(Link(idx, &((*edge_set)[idx])));
}

#endif // GRAPH_H
