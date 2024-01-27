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
    vector<int> degree;
    static Graph temp;
    vector<vector<Arc>> adj;

public:
    Graph() { 
        cout << "Graph object constructed"; 
    }
    ~Graph() {
        adj.clear();
        cout << "Graph object Destructed";
    }
    void resize(int V) { adj.resize(V); }
    void add_arc(int from, int to, int weight) {
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
