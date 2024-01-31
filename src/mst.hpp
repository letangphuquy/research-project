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

public:
    AlmostMST() {}
    void resize(int E) { bias.resize(E+1); }
    void change_bias(int idx) {
        bias_count += bias[idx] ? -1 : +1;
        bias[idx].flip();
    }
    void clear_bias(void) {
        bias.clear();
        bias_count = 0;
    }
    Gene calc_for(Gene curset, Real r_fluctuate = 0) {
        Gene result(curset.size(), bit::bit0);
        cc_handler.fill();
        #define u edges[i].from
        #define v edges[i].to
        auto add_edge = [&] (int i) {
            if (cc_handler.merge_set(u,v))
                result[i].set(true);
        };
        if (bias_count > 0) {
            for (int i = 0; i < bias.size(); i++) {
                if (bias[i]) add_edge(i);
            }
        }
        for (int i = 0; i < curset.size(); i++)
            if (curset[i] && !result[i]) if (!cc_handler.same_set(u,v)) {
                if (!equals(r_fluctuate, 0)) {
                    if (random_num(0,1) < r_fluctuate) continue;
                }
                add_edge(i);
            }
        if (!equals(r_fluctuate, 0)) {
            // add remaining edges to "spans"
            for (int i = 0; i < curset.size(); i++)
                if (curset[i]) add_edge(i);
        }
        #undef u
        #undef v
        return result;
    }
} mst_handler;


#endif // MST_H
