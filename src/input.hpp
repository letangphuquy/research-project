#if !defined(INPUT_H)
#define INPUT_H

#include "template.hpp"
#include "problem.hpp"
#include "floyd.hpp"
#include "dsu.hpp"
#include "mst.hpp"
#include <string.h>
#include <fstream>

void clear_input();
bool input_preprocessing();

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

void read_input(string inpname) {
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
    const char* phrases[3] = {"Nodes", "Edges", "Terminals"};
    static int* ref[3] = {&num_nodes, &num_edges, &num_terminals};

    while (getline(inpf, line)) {
        if (line.empty() or line.size() <= 0) continue;
        // cout << "\treading " << line << '\n';
        bool got = false;
        for (int i = 0; i < 3; i++)
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
}

void clear_input(void) {
    num_nodes = num_edges = num_terminals = 0;
    edges.clear();
    terminals.clear();
    sp_handler.reset();
}

bool input_preprocessing(void) {
    printf("I read: |V| = %d, |E| = %d, |S| = %d\n", num_nodes, num_edges, num_terminals);
    is_terminal.assign(num_nodes + 1, false);
    for (auto si : terminals) is_terminal[si] = true;
    edges_order = random_permutation(num_edges);
    for (auto &ri : edges_order) --ri;
    sort(all_of(edges));
    Graph::init(&edges);
    graph.resize(num_nodes);
    Gene full_graph(num_edges, bit::bit1);
    graph.assign_subgraph(&full_graph);
    graph.construct_adjacency_list();
    cc_handler.init(num_nodes);
    mst_handler.resize(num_edges);
    return sp_handler.calc_for(graph);
}

#endif // INPUT_H
