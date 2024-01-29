#if !defined(UNIT_TEST_H)
#define UNIT_TEST_H

#include "template.hpp"
#include "problem.hpp"
#include "solution.hpp"
#include "floyd.hpp"

void unit_test_shortest_path() {
    Gene subgraph(num_edges, bit::bit0);
    for (int u = 1; u <= num_nodes; u++) {
        for (int v = u+1; v <= num_nodes; v++) {
            cout << "Shortest path [" << u << "," << v << "]:\n";
            sp_handler.trace_path(u,v, &subgraph);
            cout << subgraph.debug_string() << '\n';
        }
    }
}

void unit_test_reduce() {
    const int NUM_TRIES = 20;
    for (int _ = 0; _ < NUM_TRIES; _++) {
        int mask = random_int(0, (1<<num_edges) - 1);
        Solution candidate;
        Gene gene(num_edges);
        for (int i = 0; i < num_edges; i++)
            if (mask>>i&1) gene[i].set(true);
        candidate.set_gene(gene);
        cout << "For " << candidate << ", reduced: ";
        candidate.reduce();
        cout << candidate << '\n';
    }
}

void unit_test_makespan() {
    const int NUM_TRIES = 20;
    for (int _ = 0; _ < NUM_TRIES; _++) {
        int mask = random_int(0, (1<<num_edges) - 1);
        Solution candidate;
        Gene gene(num_edges);
        for (int i = 0; i < num_edges; i++)
            if (mask>>i&1) gene[i].set(true);
        candidate.set_gene(gene);
        cout << "For " << candidate << ", after spanning: ";
        candidate.make_span();
        cout << candidate << '\n';
    }
}

void unit_test() {
    cout << "Edge list:\n";
    for (auto [u,v,w] : edges) 
        cout << "\t" << u << ' ' << v << ' ' << w << '\n';
    unit_test_shortest_path();
    unit_test_reduce();
    unit_test_makespan();
}

#endif // UNIT_TEST_H
