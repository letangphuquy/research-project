#if !defined(FLOYD_H)
#define FLOYD_H

#include "template.hpp"
#include "graph.hpp"

class ShortestPath
{
private:
    static const int N_MAX = 3000;
    int V;
    Int** dist;
    int** trace; // positive: edge index, negative: mid node index
    bool calculated;
    void reset() {
        calculated = false;
        free_2d_array<Int>(dist, V+1);
        free_2d_array<int>(trace, V+1);
    }
    void trace_internal(int u, int v, Gene* path);

public:
    ~ShortestPath() { reset(); }
    ShortestPath() { dist = nullptr; trace = nullptr; }
    ShortestPath(cst(Graph) g) { calc_for(g); }

    void calc_for(cst(Graph) g);
    void trace_path(int s, int t, Gene* path);
} sp_handler;

void ShortestPath::calc_for(cst(Graph) g) {
    this->V = g.size();
    if (V > N_MAX) {
        cout << "Graph too large. Couldn't compute D(g)\n";
        return ;
    }
    calculated = true;
    populate_2d_array<Int>(dist, V+1, V+1, INF);
    populate_2d_array<int>(trace, V+1, V+1, 0);

    for (int u = 1; u <= V; u++) {
        for (auto [idx, edge] : g[u]) {
            auto [fr, to, wei] = *edge;
            int v = fr^to^u;
            if (umin(dist[u][v], wei)) trace[u][v] = idx;
        }
    }
    for (int m = 1; m <= V; m++) {
        for (int u = 1; u <= V; u++) {
            if (dist[u][m] == INF) continue;
            for (int v = 1; v <= V; v++) {
                if (dist[m][v] == INF) continue;
                if (umin(dist[u][v], dist[u][m] + dist[m][v]))
                    trace[u][v] = -m;
            }
        }
    }
}

void ShortestPath::trace_internal(int u, int v, Gene* path) {
    if (dist[u][v] == INF) return ;
    int id = trace[u][v];
    if (id >= 0) {
        (*path)[id] = bit::bit1;
        return ;
    }
    trace_internal(u, -id, path);
    trace_internal(-id, v, path);

}
void ShortestPath::trace_path(int u, int v, Gene* path) {
    if (!calculated) {
        cout << "No graph instance passed in. Provide input first\n";
        return ;
    }
    path->clear();
    trace_internal(u,v, path);
}

#endif // FLOYD_H
