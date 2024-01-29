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
    int V;
    vector<int> degree; //out-degree
    vector<vector<Link>> adj;
    static Graph instance;
    static string owner;

public:
    Graph() { clear(); }
    const vector<Link>& operator[] (int u) const { return adj[u]; } // readonly field
    int size(void) const { return V; }
    static Graph* get_public_instance(string addr) {
        owner = addr;
        instance.clear();
        return &instance;
    }
    static string get_instance_owner(void) { return owner; }
    
    void clear() { resize(0); degree.clear(); }
    void resize(int nV) { 
        adj.resize((V = nV) + 1); 
        degree.resize(V + 1);
    }
    void add_arc(int from, int index, Edge *edge) {
        adj[from].push_back(Link(index, edge));
        ++degree[from];
    }
    void add_edge(int u, int v, int index, Edge *edge) {
        add_arc(u, index, edge);
        add_arc(v, index, edge);
    }
};


#endif // GRAPH_H
