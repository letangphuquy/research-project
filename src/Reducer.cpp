#include "testrun.hpp"

/*
Writes reduced input back to another file, in the same directory it was read.
Example:
input: A\t1.stp (originial stp file)
output: A\%REDNAME%_t1.stp (reduced input based on 2 test)
*/
int return_reduced_input(std::ofstream& out) {
    // Which information is sent out via ?
    // cout (log file), out (the MAIN, modified Input File)
    // cout << "After Reduction: |V| = " << num_nodes << ", |E| = " << num_edges << ", |T| = " << num_terminals << '\n';
    std::cerr << "After Reduction: |V| = " << num_nodes << ", |E| = " << num_edges << ", |T| = " << num_terminals << '\n';
    

    out << "SECTION Graph\n";
    out << "AddedCost " << added_cost << '\n'; // remember to add this phrase to input reader! 
    out << "Nodes " << num_nodes << '\n';
    out << "Edges " << num_edges << '\n';
    for (auto [u,v,w] : edges)
        out << "E " << u << ' ' << v << ' ' << w << '\n'; 
    out << "END\n\n";

    out << "SECTION Terminals\n";
    out << "Terminals " << num_terminals << '\n';
    for (auto t_i : terminals)
        out << "T " << t_i << '\n';
    out << "END\n\nEOF\n";
    
    return added_cost;
}

int main()
{
    MapType testset_start;
    SetType included_sets(set_union(SETS_BENCHMARK, SETS_BENCHMARK_ADDITIONAL));
    included_sets = SETS_DEBUG;
    SetType excluded_sets;
    SetType included_tests;
    SetType excluded_tests;
    run_tests(REDNAME, 
        return_reduced_input, 
        false, 
        testset_start, 
        included_sets,
        excluded_sets, 
        included_tests, 
        excluded_tests,
        true,
        true
    );
}
