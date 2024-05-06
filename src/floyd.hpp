#if !defined(FLOYD_H)
#define FLOYD_H

#include "template.hpp"
#include "array.hpp"
#include "graph.hpp"

class ShortestPath
{
private:
    // static const int N_MAX = 4097; // agressive for SP & PUC largest test :)
    int V;
    int dist[N_MAX][N_MAX], cnt[N_MAX][N_MAX];
    int trace[N_MAX][N_MAX]; // positive: edge index, negative: mid node index
    Array<int> path_edges;
    void trace_internal(int u, int v);

public:
    ShortestPath() {}
    ShortestPath(cst(Graph) g) { calc_for(g); }

    int distance(int u, int v);
    bool calc_for(cst(Graph) g);
    void trace_path(int s, int t, Bitstr* gene, bool renew);
    void trace_path(int s, int t, Array<int>* gene, bool renew);
} sp_handler;

int ShortestPath::distance(int u, int v) {
    if (u > v) std::swap(u,v);
    if (u < 0 or v >= V) return INF;
    return dist[u][v];
 }

bool ShortestPath::calc_for(cst(Graph) g) {
    this->V = g.size() + 1;
    path_edges.allocate(V);
    if (V > N_MAX) {
        cout << "Graph too large. Couldn't compute D(g)\n";
        return false;
    }
    for (int u = 1; u < V; u++)
        for (int v = 1; v < V; v++) {
            dist[u][v] = (u != v) ? INF : 0;
            trace[u][v] = 0;
            cnt[u][v] = 0;
        }

    for (int u = 1; u < V; u++) {
        for (auto idx : g[u]) {
            auto edge = g.edge(idx);
            int v = edge.other_end(u);
            if (umin(dist[u][v], edge.weight)) { trace[u][v] = idx, cnt[u][v] = 1; }
        }
    }
    vector<int> medians = random_permutation(V-1);
    for (int m : medians) {
        for (int u = 1; u < V; u++) {
            if (dist[u][m] == INF) continue;
            for (int v = 1; v < V; v++) {
                if (dist[m][v] == INF) continue;
                if (umin(dist[u][v], dist[u][m] + dist[m][v]))
                    cnt[u][v] = 0,
                    trace[u][v] = -m;
                // if (dist[u][v] == dist[u][m] + dist[m][v])
                //     cnt[u][v] = std::min(1ll*INF, 1ll * cnt[u][m] * cnt[m][v] + cnt[u][v]);
            }
        }
    }
    // cout << "\tGot distance matroid with size " << V << '\n';
    return true;
}

void ShortestPath::trace_internal(int u, int v) {
    if (dist[u][v] == INF or u == v) return ;
    int id = trace[u][v];
    if (id >= 0) {
        path_edges.push_back(id);
        return ;
    }
    trace_internal(u, -id);
    trace_internal(-id, v);

}
void ShortestPath::trace_path(int u, int v, Bitstr* gene, bool renew = true) {
    if (renew) bit::fill(all_of(*gene), bit::bit0);
    path_edges.clear();
    trace_internal(u,v);
    for (int i = 0; i < path_edges.curSize; i++)
        (*gene)[path_edges[i]].set(1);
}
void ShortestPath::trace_path(int s, int t, Array<int> *gene, bool renew = true) {
    if (renew) gene->clear();
    path_edges.clear();
    trace_internal(s,t);
    gene->push_back(path_edges);
}

#endif // FLOYD_H
