#if !defined(FLOYD_H)
#define FLOYD_H

#include "template.hpp"
#include "graph.hpp"

class ShortestPath
{
private:
    static const int N_MAX = 3000;
    Int** dist;
    int** trace; // positive: edge index, negative: mid node index

public:
    ~ShortestPath() { delete dist; delete trace; }
    ShortestPath() { dist = nullptr; }
    ShortestPath(cst(Graph) g) { calc_for(g); }

    void calc_for(cst(Graph) g) {
        int V = g.size();
        if (V > N_MAX) {
            cout << "Graph too large. Couldn't compute D(g)\n";
            return ;
        }

        populate_2d_array<Int>(dist, V+1, V+1, INF);
        populate_2d_array<int>(trace, V+1, V+1, 0);

        for (int u = 1; u <= V; u++) {
            for (auto [v,w] : g[u]) 
                if (umin(dist[u][v], w)) trace[u][v] = 1;
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

    void get_path(int u, int v) {
        if (dist[u][v] == INF) return ;
        int id = trace[u][v];
        if (id > 0) {
            cout << "Edge " << id << '\n';
            return ;
        }
        get_path(u, -id);
        get_path(-id, v);
    }
} sp_handler;


#endif // FLOYD_H
