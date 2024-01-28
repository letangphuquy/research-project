#if !defined(GRAPH_H)
#define GRAPH_H

#include "template.hpp"
// #include "problem.hpp" // CROSS REF.

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

class Graph
{
private:
    int V;
    vector<int> degree;
    static Graph temp;
    vector<vector<Arc>> adj;

public:
    Graph() {}
    const vector<Arc>& operator[] (int u) const { return adj[u]; } // readonly field
    int size(void) const { return V; }
    
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


#endif // GRAPH_H
