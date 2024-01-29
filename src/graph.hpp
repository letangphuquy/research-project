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

/*
class Graph
{
private:
    int V;
    vector<int> degree;
    static Graph temp;
    vector<vector<Arc>> adj;

public:
    Graph() { clear(); }
    const vector<Arc>& operator[] (int u) const { return adj[u]; } // readonly field
    int size(void) const { return V; }
    
    void clear() {
        resize(0);
        degree.clear();
    }
    void resize(int nV) { adj.resize((V = nV) + 1); }
    void add_arc(int from, int to, int weight) {
        if (from <= V && to <= V)
            adj[from].push_back(Arc(to, weight));
    }
    void add_edge(int u, int v, int w) {
        add_arc(u,v, w);
        add_arc(v,u, w);
    }
    
    static Graph* construct_from_solution(cst(string) gene) {
        return &temp;
    }
};
*/

// stores edge's ID and ref. (s) in adjacency list
using Link = pair<int, Edge*>;
class Graph
{
private:
    static vector<Edge>* edge_set;
    Gene* subgraph;
    bool has_degree, has_adj;

    int V;
    vector<int> degree; //out-degree
    vector<vector<Link>> adj;

    static Graph instance;
    static string owner;
    void add_arc(int u, int idx);
public:
    vector<bool> to_remove; // for leaves trimmer
    Graph() { clear(); }
    const vector<Link>& operator[] (int u) const { return adj[u]; } // readonly field
    int size(void) const { return V; }
    static Graph* get_public_instance(string addr) {
        owner = addr;
        instance.clear();
        return &instance;
    }
    static string get_instance_owner(void) { return owner; }
    
    static void init(vector<Edge>* ref) { edge_set = ref; }
    void assign_subgraph(Gene* gene) {
        subgraph = gene;
        clear();
    }
    void clear() { 
        resize(0);
        // edge_set = nullptr; // NOPE! persists the E set for later use
        has_degree = has_adj = false;
    }
    void resize(int nV) { 
        adj.resize((V = nV) + 1); 
        degree.resize(V + 1);
    }

    void compute_degree(void) {
        if (has_degree) return;
        has_degree = true;
        fill(all_of(degree), 0);
        for (int i = 0; i < subgraph->size(); i++) {
            if ((*subgraph)[i]) {
                auto edge = (*edge_set)[i];
                ++degree[edge.from];
                ++degree[edge.to];
            }
        }
    }
    void construct_adjacency_list(void) {
        if (has_adj) return ;
        has_adj = true;
        for (auto &neigh : adj) neigh.clear();
        for (int i = 0; i < subgraph->size(); i++) 
            if ((*subgraph)[i]) {
                auto edge = (*edge_set)[i];
                add_arc(edge.from, i);
                add_arc(edge.to, i);
            }
    }
    bool is_leaf(int u) const { return degree[u] == 1; }
    void remove_leaf_edge(int par, int leaf, int idx) {
        (*subgraph)[idx].set(false);
        --degree[par];
        --degree[leaf];
        to_remove[leaf] = true;
    }
};
void Graph::add_arc(int from, int idx) {
    adj[from].push_back(Link(idx, &((*edge_set)[idx])));
}


#endif // GRAPH_H
