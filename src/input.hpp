#if !defined(INPUT_H)
#define INPUT_H

#include "template.hpp"
#include "problem.hpp"
#include "floyd.hpp"
#include "dsu.hpp"
#include "mst.hpp"
#include <string.h>
#include <fstream>
#include <map>

std::map<pair<int,int>, int> edge_index_mapping;
int get_edge_idx(int u, int v) { 
    if (u > v) std::swap(u,v);
    if (edge_index_mapping.count({u,v}))
        return edge_index_mapping[{u,v}];
    return -1; 
}
int get_edge_weight(int u, int v) {
    int idx = get_edge_idx(u,v);
    return idx == -1 ? INF : edges[idx].weight;
}
void assign_edge_index(Edge e, int idx) {
    auto [u,v,w] = e;
    if (u > v) std::swap(u,v);
    edge_index_mapping[{u,v}] = idx;
}

void clear_input();
void initialization();

vector<int> str2nums(const char* str) {
    vector<int> result;
    int val = 0;
    bool last = false;
    for (int i = 0; i < strlen(str); i++) {
        if (!isdigit(str[i])) {
            if (last) result.push_back(val);
            val = 0;
            last = false;
            continue;
        } 
        val = 10*val + (str[i] - '0');
        last = true;
    }
    if (last) result.push_back(val);
    return result;
}

int read_input(string inpname) {
    cout << "READING " << inpname << '\n';
    clear_input();
    std::ifstream inpf(inpname);
    FILE* tmpf = fopen("temp.txt", "w");
    string line;

    auto read_and_assign = [&] (const char* keyword, int* var) {
        if (line.rfind(keyword, 0) == 0) {
            const char* cline = line.c_str();
            auto nums = str2nums(cline);
            *var = nums[0];
            fprintf(tmpf, "%d\n", *var);
            return true;
        }
        return false;
    };
    const int N_PHRASES = 4;
    const char* phrases[N_PHRASES] = {"Nodes", "Edges", "Terminals", "AddedCost"};
    static int* ref[N_PHRASES] = {&num_nodes, &num_edges, &num_terminals, &added_cost};

    while (getline(inpf, line)) {
        if (line.empty() or line.size() <= 0) continue;
        // cout << "\treading " << line << '\n';
        bool got = false;
        for (int i = 0; i < N_PHRASES; i++)
            if (read_and_assign(phrases[i], ref[i])) got = true;
        if (got) continue ;
        if (1 < line.size() && line[1] == ' ') {
            if (line[0] == 'E') {
                auto arr = str2nums(line.c_str());
                int u = arr[0], v = arr[1], w = arr[2];
                fprintf(tmpf, "%d %d %d\n", u,v,w); 
                edges.push_back(Edge(u,v,w));
            } else
            if (line[0] == 'T') {
                int si = str2nums(line.c_str())[0];
                fprintf(tmpf, "%d\n", si);
                terminals.push_back(si);
            }
        }
    }
    fclose(tmpf);
    return added_cost;
}

void clear_input(void) {
    added_cost = 0;
    num_nodes = num_edges = num_terminals = 0;
    edges.clear();
    terminals.clear();
}

// WTF, very interesting bug here: out of this scope, the gene is dumped and so Graph get memory and pointer error
void init_global_graph(void) {
    sort(all_of(edges));
    Graph::init(&edges);
    graph.resize(num_nodes);
    Gene full_graph(num_edges, bit::bit1);
    graph.assign_subgraph(&full_graph);
}

void assign_indices_for_edges(Gene subgraph) {
    edge_index_mapping.clear();
    Iterate(subgraph, [&] (int i) {
        assign_edge_index(edges[i], i);
    });
}

Gene full_graph; // MUST BE GLOBAL for pointer to work!!!
// Mark terminals, sort edges and assign indices accordingly, init cc_handler & mst_handler
void initialization(void) {
    printf("I read: |V| = %d, |E| = %d, |S| = %d\n", num_nodes, num_edges, num_terminals);
    CNT_LS_CALL = CNT_LS_SUCC = 0;
    is_terminal.assign(num_nodes + 1, false);
    for (auto si : terminals) is_terminal[si] = true;
    edges_order = random_permutation(num_edges);
    for (auto &ri : edges_order) --ri;
    sort(all_of(edges));
    Graph::init(&edges);
    graph.resize(num_nodes);
    
    full_graph.resize(num_edges);
    bit::fill(all_of(full_graph), bit::bit1);
    assign_indices_for_edges(full_graph);

    graph.assign_subgraph(&full_graph);
    graph.construct_adjacency_list();
    cc_handler.init(num_nodes);
    mst_handler.resize(num_edges);
    // sp_handler.calc_for(graph);
}

#endif // INPUT_H
