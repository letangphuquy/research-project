#if !defined(INPUT_H)
#define INPUT_H

#include "template.hpp"
#include "problem.hpp"

void parse_graph(string src, string dest) {
    // From PACE / DILMACS format to the format in io_format.md
    // read from tmpf src, write to dest
}

void clear_input();
void input_preprocessing();

template<typename... Args> void write_now(FILE* file, const char* format, Args... args) {
    fprintf(file, format, args...);
    fflush(file);
}

void read_input(string inpf) {
    // into those varriables defined at inpf
    cout << "READING " << inpf << '\n';
    clear_input();
    freopen(inpf.c_str(), "r", stdin);
    FILE* tmpf = fopen("temp.txt", "w+");
    string line;
    
    auto ignore_line = [&] () {
        // fprintf(tmpf, "%s\n", line);
        write_now(tmpf, "%s\n", line.c_str());
        fscanf(tmpf, "%*[^\n]\n"); // ignore
    };

    auto read_and_assign = [&] (const char* keyword, int* var) {
        if (line.rfind(keyword, 0) == 0) {
            *var = atoi(line.c_str());
            ignore_line();
            return true;
        }
        return false;
    };
    const char* phrases[3] = {"Nodes", "Edges", "Terminals"};
    int* ref[3] = {&num_nodes, &num_edges, &num_terminals};
    while (getline(cin, line)) {
        // cout << "\treading " << line << '\n';
        if (line.empty() or line.size() <= 0) continue;
        if (line[0] == 'E') {
            int u,v,w;
            write_now(tmpf, "%s\n", line.c_str());
            fscanf(tmpf, "%*s %d %d %d", &u, &v, &w);
            edges.push_back(Edge(u,v,w));
        } else
        if (line[0] == 'T') {
            int si;
            write_now(tmpf, "%s\n", line.c_str());
            fscanf(tmpf, "%*s %d", &si);
            terminals.push_back(si);
        } else {
            bool did = false;
            for (int i = 0; i < 3; i++)
                if (read_and_assign(phrases[i], ref[i])) did = true;
            if (!did) ignore_line();
        }
    }
    fclose(tmpf);
    input_preprocessing();
}

void clear_input(void) {
    num_nodes = num_edges = num_terminals = 0;
    edges.clear();
    terminals.clear();
}

void input_preprocessing(void) {
    printf("I read: |V| = %d, |E| = %d, |S| = %d\n", num_nodes, num_edges, num_terminals);
    
    sort(all_of(edges));
    for (auto [u,v,w] : edges) {
        cout << "E " << u << ' ' << v << ' ' << w << '\n';
    }
}

#endif // INPUT_H
