#if !defined(PROBLEM_H)
#define PROBLEM_H

#include "graph.hpp"

#define N_NODES 1'000
#define N_EDGES 100'000

int num_nodes, num_edges, num_terminals;
vector<Edge> edges;
vector<int> terminals;
vector<bool> is_terminal;
Graph input_graph;

// WARNING: Enable Flexible Changing of Constant for different algorithm
// R : Ratio, P: Probability, C: Coefficient, N: Number of
#define NUM_GEN 100
#define POP_SIZE 100
#define R_HEUR_RAND 0.4
#define R_HEUR_STEM 0.4
#define R_HEUR_MST 0.1
#define R_HEUR_SP 0.1
#define N_ELITE 2
#define N_SEED_PER_ELITE 3
#define N_SEED (N_ELITE * N_SEED_PER_ELITE)

#define P_MUTATION 0.05
#define R_CHANGE 0.05
#define R_FLUCTUATE 0.2

#define IS_SPARSE_GRAPH (num_edges <= num_nodes * log2(num_nodes))
vector<int> edges_order;

int CNT_LS_CALL = 0;
int CNT_LS_SUCC = 0;

const string REDNAME = "reducer";
int added_cost;

#endif // PROBLEM_H

