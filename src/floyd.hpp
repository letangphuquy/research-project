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
    void trace_internal(int u, int v, Gene* path);
    bool check_calculated(void);

public:
    void reset() {
        calculated = false;
        free_2d_array<Int>(dist, V);
        free_2d_array<int>(trace, V);
    }
    ~ShortestPath() { reset(); }
    ShortestPath() { dist = nullptr; trace = nullptr; }
    ShortestPath(cst(Graph) g) { calc_for(g); }

    Int distance(int u, int v);
    bool calc_for(cst(Graph) g);
    void trace_path(int s, int t, Gene* path, bool renew);
} sp_handler;

bool ShortestPath::check_calculated() {
    if (!calculated) {
        cout << "No graph instance passed in. Provide input first\n";
        return false;
    }
    return true;
}

Int ShortestPath::distance(int u, int v) {
    if (!check_calculated()) return INF;
    if (u > v) std::swap(u,v);
    if (u < 0 or v >= V) return INF;
    return dist[u][v];
 }

bool ShortestPath::calc_for(cst(Graph) g) {
    this->V = g.size() + 1;
    if (V > N_MAX) {
        cout << "Graph too large. Couldn't compute D(g)\n";
        return false;
    }
    calculated = true;
    populate_2d_array<Int>(dist, V, V, INF);
    populate_2d_array<int>(trace, V, V, 0);

    for (int u = 1; u < V; u++) {
        for (auto [idx, edge] : g[u]) {
            auto [fr, to, wei] = *edge;
            int v = fr^to^u;
            if (umin(dist[u][v], wei)) trace[u][v] = idx;
        }
    }
    for (int m = 1; m < V; m++) {
        for (int u = 1; u < V; u++) {
            if (dist[u][m] == INF) continue;
            for (int v = 1; v < V; v++) {
                if (dist[m][v] == INF) continue;
                if (umin(dist[u][v], dist[u][m] + dist[m][v]))
                    trace[u][v] = -m;
            }
        }
    }
    cout << "Got distance matroid with size " << V << '\n';
    return true;
}

void ShortestPath::trace_internal(int u, int v, Gene* path) {
    if (dist[u][v] == INF or u == v) return ;
    int id = trace[u][v];
    if (id >= 0) {
        (*path)[id] = bit::bit1;
        return ;
    }
    trace_internal(u, -id, path);
    trace_internal(-id, v, path);

}
void ShortestPath::trace_path(int u, int v, Gene* path, bool renew = true) {
    if (!check_calculated()) return ;
    if (renew) bit::fill(all_of(*path), bit::bit0);
    trace_internal(u,v, path);
}

#endif // FLOYD_H
