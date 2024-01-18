
#include <bits/stdc++.h>
using namespace std;

template<class A, class B> bool minimize(A& var, const B& val) {
    if (val < var) return var = val, true;
    return false;
}

const int INF = 1e9;
const int N = 305;
int num_nodes, num_edges;
struct Edge {
    int to, weight;
};
vector<Edge> adj[N];
int num_terminals;
vector<int> terminals;
int get_term_id[N];

// all-pair shortest path
int dis[N][N];
bool is_edge[N][N]; // whether d(i,j) is a single edge
void calc_shortest_paths(void) {
    for (int u = 1; u <= num_nodes; u++) {
        for (int v = 1; v <= num_nodes; v++) {
            dis[u][v] = (u == v) ? 0 : INF;
            is_edge[u][v] = false;
        }
        for (auto [v,w] : adj[u])
            if (minimize(dis[u][v], w)) is_edge[u][v] = true;
    }
    // Floyd - Warshall algorithm
    for (int mid = 1; mid <= num_nodes; mid++) {
        for (int u = 1; u <= num_nodes; u++) if (dis[u][mid] < INF)
            for (int v = 1; v <= num_nodes; v++) if (dis[mid][v] < INF)
                minimize(dis[u][v], dis[u][mid] + dis[mid][v]);
    }
    for (int u = 1; u <= num_nodes; u++) {
        for (int v = 1; v <= num_nodes; v++) {
            cout << dis[u][v] << ' ';
        }
        cout << '\n';
    }
}

const int N_TERMINALS = 18;
const int MASKS = 1 << N_TERMINALS;

int FULL;
int *split[MASKS], *steiner[MASKS];

bool done[MASKS];
void calc_dp(int msk) {
    if (done[msk]) return ;
    done[msk] = true;
    cerr << "calculate set " << msk << '\n';
    for (int u = 1; u <= num_nodes; u++) {
        split[msk][u] = steiner[msk][u] = INF;
        // int id = get_term_id[u];
        // if (id != -1 && (msk>>id&1))
        //     split[msk][u] = steiner[msk][u] = 0;
    }
    if (__builtin_popcount(msk) == 1) {
        int one = terminals[__builtin_ctz(msk)];
        for (int u = 1; u <= num_nodes; u++) {
            split[msk][u] = steiner[msk][u] = dis[u][one];
        }
        return ;
    }
    int conf[num_nodes + 5], best[num_nodes + 5];
    memset(best, -1, sizeof(best));
    for (int u = 1; u <= num_nodes; u++) {
        for (int sub = msk-1; sub > 0; sub--) {
            sub &= msk;
            if (!sub) break;
            // if (sub&1) continue; // a-symmetry: for two complementary set only one contains S_i
            if (minimize(split[msk][u], steiner[sub][u] + steiner[msk^sub][u])) {
                conf[u] = sub;
            }       
        }
        for (int v = 1; v <= num_nodes; v++) {
            if (minimize(steiner[msk][v], dis[v][u] + split[msk][u])) {
                best[v] = u;
            }
        }
    }
    for (int u = 1; u <= num_nodes; u++) {
        if (best[u] != -1)
            cerr << "\t" << u << " connected to " << best[u] << " which in turn splitted to " << conf[best[u]]
                << "[" << steiner[msk][u] << "]" << '\n'; 
    }
}

void input(string inpfile = "") {
    if (!inpfile.empty())
        freopen(inpfile.c_str(), "r", stdin);
    cin >> num_nodes >> num_edges;
    for (int u,v,w, i = 1; i <= num_edges; i++) {
        cin >> u >> v >> w;
        adj[u].push_back({v,w});
        adj[v].push_back({u,w});
    }
    cin >> num_terminals;
    terminals.resize(num_terminals);
    for (auto &t_i : terminals) cin >> t_i;

    if (num_terminals > N_TERMINALS) {
        printf("Number of terminals exceeded solving capability\n");
        exit(EXIT_SUCCESS);
    }
    FULL = (1 << num_terminals);
    for (int msk = 0; msk < FULL; msk++) {
        split[msk] = new int[num_nodes + 5];
        steiner[msk] = new int[num_nodes + 5];
    }
    memset(get_term_id, -1, sizeof(get_term_id));
    for (int i = 0; i < num_terminals; i++)
        get_term_id[terminals[i]] = i;
}

void solve() {
    calc_shortest_paths();
    for (int msk = 0; msk < FULL; msk++) calc_dp(msk);
    int mst = INF; //minimum Steiner tree :)
    int q = *terminals.begin();
    int rem = (FULL-1) ^ (1<<0);
    for (int p = 1; p <= num_nodes; p++)
        minimize(mst, dis[q][p] + split[rem][p]);
    cout << mst;
}

int main()
{
    // func_signature(5);  Test Lib: error in VS Code
    input();
    solve();
}