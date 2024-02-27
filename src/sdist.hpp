#if !defined(SDIST_H)
#define SDIST_H

#include "template.hpp"
#include "floyd.hpp"
#include "problem.hpp"

class SpecialDistance {
private:
    // static const int N_MAX = 4097;
    int V;
    int dist[N_MAX][N_MAX];

public:
    SpecialDistance() { }
    SpecialDistance(cst(Graph) g) { calc_for(g); }

    int distance(int u, int v);
    void calc_for(cst(Graph) g);
} SD_handler;

int SpecialDistance::distance(int u, int v) { return dist[u][v]; }

void SpecialDistance::calc_for(cst(Graph) g) {
    this->V = g.size() + 1;
    DBGn(V);
    for (int u = 0; u < V; u++)
        for (int v = 0; v < V; v++) dist[u][v] = INF;

    for (int u = 1; u < V; u++) {
        for (auto [idx, edge] : g[u]) {
            auto [fr, to, wei] = *edge;
            int v = fr^to^u;
            umin(dist[u][v], sp_handler.distance(u,v));
        }
    }
    for (int m : terminals) {
        for (int u = 1; u < V; u++) {
            if (dist[u][m] == INF) continue;
            for (int v = 1; v < V; v++) {
                if (dist[m][v] == INF) continue;
                umin(dist[u][v], std::max(dist[u][m], dist[m][v]));
            }
        }
    }
    // cout << "\tGot Special Distance with size " << V << '\n';
}

#endif // SDIST_H
