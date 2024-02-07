#if !defined(MST_H)
#define MST_H

#include "template.hpp"
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

public:
    AlmostMST() {}
    void resize(int E) { bias.resize(E+1); }
    void change_bias(int idx) {
        bias_count += bias[idx] ? -1 : +1;
        bias[idx].flip();
    }
    Gene calc_for(Gene curset, Real r_fluctuate = 0) {
        Gene result(curset.size(), bit::bit0);
        cc_handler.fill();
        #define u edges[idx].from
        #define v edges[idx].to
        auto add_edge = [&] (int idx) {
            if (cc_handler.merge_set(u,v))
                result[idx].set(true);
        };
        if (bias_count > 0) {
            Iterate(bias, [&] (int idx) { add_edge(idx); }); 
        }
        Iterate(curset, [&] (int idx) {
            if (!result[idx] && !cc_handler.same_set(u,v)) {
                if (equals(r_fluctuate, 0) or random(0,1) >= r_fluctuate) 
                    add_edge(idx);
            }
        });
        if (!equals(r_fluctuate, 0)) { // add remaining edges to "spans"
            Iterate(curset, [&] (int idx) { add_edge(idx); });
        }
        if (bias_count) clear_bias();
        #undef u
        #undef v
        return result;
    }
} mst_handler;


#endif // MST_H
