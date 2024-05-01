#if !defined(MST_H)
#define MST_H

#include "template.hpp"
// #include "array.hpp"
#include "array.hpp"
#include "bookkeeper.hpp"
#include "dsu.hpp"
#include "problem.hpp"

/// @brief Almost Minimal Spanning Tree
// fluctuation & randomness
class AlmostMST
{
private:
    Gene bias;
    int bias_count = 0;
    void clear_bias(void) {
        bias.clear();
        bias_count = 0;
    }
    Array<int> inputEdges, outputEdges;
    void _calc_for(Real r_fluctuate = 0);
public:
    AlmostMST() {}
    void resize(int E) { bias.resize(E+1); }
    void set_bias(int idx, int v) {
        if ((bias[idx] == bit::bit1) == (v>0)) return;
        change_bias(idx);
    }
    void change_bias(int idx) {
        bias_count += bias[idx] ? -1 : +1;
        bias[idx].flip();
    }
    void resetEdges() { 
        inputEdges.resize(2*num_nodes);
        inputEdges.clear(); 
    }
    void loadEdges(Gene set) {
        resetEdges();
        Iterate(set, [&] (int idx) { inputEdges.pushBack(idx); });
    }
    void loadEdges(cst(Array<int>) set) {
        resetEdges();
        for (int i = 0; i < set.curSize; i++) inputEdges.pushBack(set[i]);
    }
    Gene calc_for(Gene set, Real r_fluctuate = 0) {
        loadEdges(set);
        Gene result(set.size(), bit::bit0);
        _calc_for(r_fluctuate);
        for (int i = 0; i < outputEdges.curSize; i++)
            result[outputEdges[i]].set(1);
        return result;
    }
    Array<int> calc_for(cst(Array<int>) set, Real r_fluctuate = 0) {
        loadEdges(set);
        _calc_for(r_fluctuate);
        return outputEdges;
    }
} mst_handler;

void AlmostMST::_calc_for(Real r_fluctuate = 0) {
    outputEdges.resize(num_nodes);
    outputEdges.clear(); 
    marker.tick();
    cc_handler.fill();
    #define u edges[idx].from
    #define v edges[idx].to
    auto add_edge = [&] (int idx) {
        if (cc_handler.merge_set(u,v)) {
            if (marker.get(idx) < 1) outputEdges.pushBack(idx);
            marker.inc(idx);
        }
    };
    if (bias_count > 0) Iterate(bias, [&] (int idx) { add_edge(idx); }); 
    for (int i = 0; i < inputEdges.curSize; i++) {
        int idx = inputEdges[i];
        if (equals(r_fluctuate, 0) or random(0,1) >= r_fluctuate) 
            if (marker.get(idx) < 1 && !cc_handler.same_set(u,v))
                add_edge(idx);
    }
    for (int i = 0; i < inputEdges.curSize; i++) add_edge(inputEdges[i]);
    if (bias_count) clear_bias();
    #undef u
    #undef v
}


#endif // MST_H
