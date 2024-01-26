#if !defined(GRAPH_H)
#define GRAPH_H

#include "template.hpp"
// #include "problem.hpp" // CROSS REF.

struct Arc {
    int to, weight;
};

struct Edge 
{
    int from, to, weight;
    bool operator< (cst(Edge) rhs) const {
        return weight < rhs.weight;
    }
};

class Graph
{
private:
    vector<int> degree;
    static Graph temp;

public:
    vector<vector<Arc>> adj;
    Graph() { 
        cout << "Grpah object constructed"; 
    }
    ~Graph() {
        adj.clear();
        cout << "Graph object Destructed";
    }
    
    static Graph* construct_from_solution(cst(string) gene) {
        return &temp;
    }
};


#endif // GRAPH_H
