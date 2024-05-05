#if !defined(BELLMAN_H)
#define BELLMAN_H

// Source: https://cp-algorithms.com/graph/bellman_ford.html#shortest-path-faster-algorithm-spfa

#include "template.hpp"
#include "array.hpp"
#include "graph.hpp"
// use the shared graph instance
#include <queue>

class MultipleShortestPath
{
private:
    Array<int> path_edges;

public:
    MultipleShortestPath() {}
    // returns distance and corresponding source node
    vector<pair<int,int>> calc_distance(cst(vector<int>) src, bool first = true) {
        int n = input_graph.size();
        static vector<pair<int,int>> labels;
        if (first) labels.assign(n+1, std::make_pair(INF, -1));
        // vector<int> cnt(n, 0);
        vector<bool> inqueue(n+1, false);
        std::queue<int> q;

        for (auto s : src) {
            labels[s] = {0,s};
            q.push(s); inqueue[s] = true;
        }
        while (!q.empty()) {
            int v = q.front(); 
            q.pop(); inqueue[v] = false;
            for (int e : input_graph[v]) {
                auto& edge = input_graph.edge(e);
                int to = edge.other_end(v);
                if (umin(labels[to].first, labels[v].first + edge.weight)) {
                    labels[to].second = labels[v].second; // same source
                    if (!inqueue[to]) {
                        q.push(to); inqueue[to] = true;
                        // cnt[to]++;
                        // if (cnt[to] > n) return false;  // negative cycle
                    }
                }
            }
        }
        return labels;
    }
} mssp_handler;

#endif // BELLMAN_H
