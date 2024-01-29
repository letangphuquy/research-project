#if !defined(PROBLEM_H)
#define PROBLEM_H

#include "graph.hpp"

const int N_NODES = 1'000;
const int N_EDGES = 100'000;

int num_nodes, num_edges, num_terminals;
vector<Edge> edges;
vector<int> terminals;
vector<bool> is_terminal;
Graph graph;

const Real R_CHANGE = 0.05;
const Real R_FLUCTUATE = 0.1;

const int MUTATION_EPOCH_SIZE = 500;
vector<int> rand_order;


#endif // PROBLEM_H

