#if !defined(SOLUTION_H)
#define SOLUTION_H

#include "template.hpp"
// #include "bitlib/bitlib.hpp"
#include "mst.hpp"
#include "graph.hpp"
#include "problem.hpp"

// Solution's representation integrates tightly with problem 

class Solution
{
private:
    string gene; // gene
    Graph* pheno = nullptr;
    bool fitness_updated;
    bool graph_updated;
public:
    Int fitness;
    Solution() {
        fitness = 0;
        gene.assign(num_edges, '0');
        fitness_updated = true;
        graph_updated = true;
    }  
    void get_graph_repr(void) {
        if (graph_updated) return ;
        pheno = Graph::construct_from_solution(gene); // TO-DO: moves here
        graph_updated = true;
    }
    void reduce() {

    }
    void trim_edges() {

    }
    void make_span() {

    }
    void mutate() {

    }
    pair<Solution, Solution> crossover(Solution x, Solution y) {

    }
};

#endif // SOLUTION_H
