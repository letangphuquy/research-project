#if !defined(GRAPH_H)
#define GRAPH_H

#include "template.hpp"
#include "array.hpp"
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
    int other_end(int u) const { return from ^ to ^ u; }
};

// stores edge's ID and ref. (s) in adjacency list
class Solution;
class Graph;
class Graph
{
private:
    static vector<Edge> edges_set;

    int V,E;
    vector<int> degree; //out-degree
    vector<vector<int>> adj;

    // static Graph* instance;
    // static string owner;
    void reset(void) { degree.clear(); adj.clear(); E = 0; }

public:
    Graph() { reset(); }
    ~Graph() { reset(); }
    static void refer_edges_set(cst(vector<Edge>) edges) { edges_set = edges; }
    const vector<int>& operator[] (int u) const { return adj[u]; } // readonly field
    int size(void) const { return V; }
    void resize(int nV) { 
        adj.resize((V = nV) + 1); 
        degree.resize(V + 1);
    }
    // static Graph* get_public_instance(string id) {
    //     owner = id;
    //     if (instance == nullptr) instance = new Graph();
    //     instance->refresh();
    //     return instance;
    // }
    // static string get_instance_owner(void) { return owner; }
    void load_graph(Bitstr gene) {
        Array<int> edges;
        edges.resize(count(all_of(gene), bit::bit1));
        edges.clear();
        Iterate(gene, [&] (int idx) { edges.push_back(idx); });
        load_graph(edges);
    }
    
    void load_graph(cst(Array<int>) edges) {
        fill(all_of(degree), 0);
        for (auto &adj_u : adj) adj_u.clear();
        for (int i = 0; i < edges.curSize; i++) {
            auto [u,v,_] = edges_set[edges[i]];
            ++degree[u]; ++degree[v];
            adj[u].push_back(edges[i]);
            adj[v].push_back(edges[i]);
        }
        E = edges.curSize;
    }

    Array<int> read_graph_as_array(void) const {
        Array<int> edges;
        edges.resize(E);
        for (int i = 0; i <= V; i++) 
            for (auto e : adj[i]) 
                if (i <= edges_set[e].other_end(i)) edges.push_back(e);
        return edges;
    }

    Bitstr read_graph_as_bitstring(void) const {
        Bitstr subgraph(E, bit::bit0);
        auto edges = read_graph_as_array();
        for (int i = 0; i < edges.curSize; i++) subgraph[i].set(1);
        return subgraph;
    }

    void remove_leaf_edge(int par, int leaf, int idx) {
        E -= adj[leaf].size();
        adj[leaf].clear();
        for (int& edge : adj[par]) 
            if (edge == idx) { std::swap(edge, adj[par].back()); break; }
        --degree[par]; --degree[leaf];
    }
    bool is_leaf(int u) const { return degree[u] == 1; }
    int deg(int u) const { return degree[u]; }

    void debug(void) {
        cout << "Graph:\n";
        for (int u = 1; u <= V; u++) {
            cout << "\tg(" << u << "): ";
            for (auto idx : adj[u]) cout << edges[idx].other_end(u) << ' ';
            cout << '\n';
        }
    }
} graph; // public temporary instance for solution representations
//Knowledge: All static member must be declared externally, after the class definition
//https://itecnote.com/tecnote/c-a-member-with-an-in-class-initializer-must-be-const/
vector<Edge> Graph::edges_set;
// Graph* Graph::instance = nullptr;
// string Graph::owner = "";

#endif // GRAPH_H
