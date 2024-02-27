#if !defined(FLOYD_H)
#define FLOYD_H

#include "template.hpp"
#include "graph.hpp"

class ShortestPath
{
private:
    // static const int N_MAX = 4097; // agressive for SP & PUC largest test :)
    int V;
    int dist[N_MAX][N_MAX];
    int trace[N_MAX][N_MAX]; // positive: edge index, negative: mid node index
    void trace_internal(int u, int v, Gene* path);

public:
    ShortestPath() {}
    ShortestPath(cst(Graph) g) { calc_for(g); }

    int distance(int u, int v);
    bool calc_for(cst(Graph) g);
    void trace_path(int s, int t, Gene* path, bool renew);
} sp_handler;

int ShortestPath::distance(int u, int v) {
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
    for (int u = 1; u < V; u++)
        for (int v = 1; v < V; v++) {
            dist[u][v] = INF;
            trace[u][v] = 0;
        }

    for (int u = 1; u < V; u++) {
        for (auto [idx, edge] : g[u]) {
            int v = edge->other_end(u);
            if (umin(dist[u][v], edge->weight)) trace[u][v] = idx;
        }
    }
    vector<int> medians = random_permutation(V-1);
    for (int m : medians) {
        for (int u = 1; u < V; u++) {
            if (dist[u][m] == INF) continue;
            for (int v = 1; v < V; v++) {
                if (dist[m][v] == INF) continue;
                if (umin(dist[u][v], dist[u][m] + dist[m][v]))
                    trace[u][v] = -m;
            }
        }
    }
    // cout << "\tGot distance matroid with size " << V << '\n';
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
    if (renew) bit::fill(all_of(*path), bit::bit0);
    trace_internal(u,v, path);
}

#endif // FLOYD_H
