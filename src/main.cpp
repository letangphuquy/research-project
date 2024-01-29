#include "template.hpp"
#include "input.hpp"
#include "solution.hpp"
#include <filesystem>
namespace fs = std::filesystem;

// PROBLEM_H
/*
Generic DS (boilerplate) // Singleton?
    dynamic array
    Disjoint Set Union

GRAPH_H
    read graph.
    construct graph from INPUT
    construct graph from REPR
    check connectivity <- DSU
    cut leaves

PROBLEM_H
    num_nodes, num_edges, num_terminals,
    Edge set (edge list), Terminal set
    properties

"Almost" Minimum Spanning Tree (allow biased towards edges or "fluctuation")
    add biased or fluctuation

calculate "properties": 
    distance matroid, path tracer // Floyd - Warshall all-pair shortest path

\\ Solution - Chromosome representation
    // (most later steps "abuse the notation" of the repr. in impl.)
    - Binary GA: Performant bitarray
        + Reduce: (make each solution a candidate Steiner tree) MST then leaves cutter
        + Make span: shortest paths connector
    - Mutation (also Neighborhood definition for distance) // add random "bias" edges then make_span
    - Crossover: uniform & make_span

"Deterministics" Heuristic(s):
- MST heuristic: mutates MST based heur
- Shortest path heuristic: add "minimal" number of edges (path tracer)
- Random heur. : make "clusters" then make_span

GA and search - related:
    POP_SIZE
    P_CROSSOVER
    P_MUTATION
    R_CHANGE
    N_ELITES
    temperature schedule and timing.

*/

// NOTE: In this implementation, parallelism is discouraged as only one individual, one graph,
// one operation is calculated at a time (singleton && global helpers)

int main()
{
    const string TESTSETS[] = {
        "SP"//, "MC"
    };
    for (auto testset : TESTSETS) {
        string path = "..\\tests\\" + testset;
        for (const auto& entry : fs::directory_iterator(path)) {
            // std::cout << entry.path() << std::endl;
            // std::cout << entry.path().extension() << std::endl;

            if (entry.path().extension() == ".stp") {
                read_input(entry.path().string());
                break;
            }
        }
    }
}