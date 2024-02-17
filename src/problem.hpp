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

#define DEBUG_MODE 0
// WARNING: Enable Flexible Changing of Constant for different algorithm
// R : Ratio, P: Probability, C: Coefficient, N: Number of
const int NUM_GEN = 100;
const int POP_SIZE = 100;
Real R_HEUR_RAND = 0.4;
Real R_HEUR_STEM = 0.4;
Real R_HEUR_MST = 0.1;
Real R_HEUR_SP = 0.1;
int N_ELITE = 2;
int N_SEED_PER_ELITE = 3;
#define N_SEED (N_ELITE * N_SEED_PER_ELITE)

// const Real THRESHOLD = 0.002;
const Real P_MUTATION = 0.05;
// dynamic P_CROSSOVER
const Real R_CHANGE = 0.05;
const Real R_FLUCTUATE = 0.2;

#define IS_SPARSE_GRAPH (num_edges <= num_nodes * log2(num_nodes))
vector<int> edges_order;

int CNT_LS_CALL = 0;
int CNT_LS_SUCC = 0;

const string REDNAME = "reducer";
int added_cost;

#endif // PROBLEM_H

