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

const int NUM_GEN = 1000;
const int POP_SIZE = 100;
const Real R_HEUR_RANDOM = 0.5;
const Real R_HEUR_MST = 0.3;
const Real R_HEUR_SP = 0.2;
const Real R_ELITE = 0.02;
const int N_ELITE = round(POP_SIZE * R_ELITE);
const Real P_MUTATION = 0.05;
const Real P_CROSS_MIN = 0.5;
// dynamic P_CROSSOVER

const Real R_CHANGE = 0.05;
const Real R_FLUCTUATE = 0.1;

// const int MUTATION_EPOCH_SIZE = 500;
vector<int> rand_order;


#endif // PROBLEM_H

