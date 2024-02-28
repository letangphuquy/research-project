#if !defined(SOLUTION_H)
#define SOLUTION_H

#include "template.hpp"
#include "bitlib/bitlib.hpp"
#include "dsu.hpp"
#include "mst.hpp"
#include "graph.hpp"
#include "problem.hpp"
#include <queue>

// Solution's representation integrates tightly with problem 
// bit i ON : choose i-th edge in global list.

// WARNING: Dangerous reference passing around for better speed? (reduce copy time).
const auto OPER_XOR = std::bit_xor<WordType>();
const auto OPER_FLIP = std::bit_not<WordType>();
const auto OPER_OR = std::bit_or<WordType>();
class Solution
{
private:
    string address,id;
    int version;
    Gene gene;
    Graph* pheno = nullptr;
    int objval; 
    int count;
    bool objval_updated;
    void set_version(int ver) { version = ver; id = address + std::to_string(version); }
    int sum_edges(void);
    vector<vector<int>> get_components(cst(vector<int>) nodes);
    void connect_components(cst(vector<vector<int>>) comps);

    static Gene temp_gene;
public:
    Solution(Gene dna) {
        temp_gene.resize(num_edges);
        address = std::to_string((unsigned long long) (void**) this); // https://stackoverflow.com/questions/7850125/convert-this-pointer-to-string
        set_version(0);
        count = 0;
        objval = 0;
        objval_updated = false;
        set_gene(dna);
    }
    Solution(): Solution(Gene(num_edges, bit::bit1)) {}
    void get_graph_instance(void);
    void force_update();
    
    int get_objval(void);
    bool operator< (Solution rhs) {  return get_objval() < rhs.get_objval(); }
    void set_gene(cst(Gene) new_gene) { gene = new_gene; force_update(); }
    Gene inversion() {  Gene R(num_edges); bit::transform(all_of(gene), R.begin(), OPER_FLIP); return R; }
    Solution& reduce(Real r_fluctuate);
    Solution& make_span(); // terminals only
    Solution& make_span_wide(Real r_drop); // some distinct components, also
    Solution& mutate(Real r_change);
    Solution& mutate_hard(Real r_change);
    int augment(Real r_change, int num_iter, bool is_random_rate);
    pair<Solution, Solution> crossover(Solution& pal);
    int count_edges() { return count; }
    int distance_to(Solution& rhs) {
        bit::transform(all_of(gene), begin(rhs.gene), begin(temp_gene), OPER_XOR);
        return bit::count(all_of(temp_gene), bit::bit1);
    }
    Real difference(Solution& rhs) { return (Real) distance_to(rhs) / num_edges; }
    // bool operator== (Solution& rhs) { return difference(rhs) <= THRESHOLD; }
    bool operator== (Solution& rhs) { return distance_to(rhs) == 0; }
    friend std::ostream& operator<< (std::ostream& stream, Solution solution);
};
Gene Solution::temp_gene = Gene();
typedef vector<Solution> Social;

void Solution::get_graph_instance() {
    if (Graph::get_instance_owner() == id) return ;
    pheno = Graph::get_public_instance(id);
    pheno->resize(num_nodes);
    pheno->assign_subgraph(&gene);
}
void Solution::force_update() {
    objval_updated = false;
    count = gene.size() ? bit::count(all_of(gene), bit::bit1) : 0;
    set_version(version + 1);
    get_graph_instance();
}

int Solution::sum_edges(void) {
    int sum = 0;
    cc_handler.init(num_nodes);
    // std::cerr << gene.size() << " vs. " << edges.size() << '\n';
    Iterate(gene, [&] (int idx) {
        auto& [u,v,w] = edges[idx];
        cc_handler.merge_set(u,v);
        sum += w;
    });
    return sum;
}

int Solution::get_objval(void) {
    if (objval_updated) return objval;
    objval_updated = true;
    objval = sum_edges();
    for (int i = 1; i < num_terminals; i++)
        if (!cc_handler.same_set(terminals[i], terminals[0]))
            return objval = INF;
    return objval;
}

Solution& Solution::reduce(Real r_fluctuate = 0) {
    static vector<bool> is_removed;
    set_gene(mst_handler.calc_for(gene, r_fluctuate));
    pheno->construct_adjacency_list();
    pheno->compute_degree();
    is_removed.assign(num_nodes+1, false);
    std::queue<int> leaves;
    for (int u = 1; u <= num_nodes; u++) {
        if (pheno->is_leaf(u)) leaves.push(u);
    }
    while (!leaves.empty()) {
        int u = leaves.front(); leaves.pop();
        if (is_terminal[u]) continue;
        for (auto [idx, edge] : (*pheno)[u]) {
            auto [fr, to, wei] = *edge;
            int v = fr^to^u;
            if (is_removed[v]) continue;
            pheno->remove_leaf_edge(v, u, idx);
            gene[idx].set(0); // moved out here due to weird bug
            if (pheno->is_leaf(v)) leaves.push(v);
        }
    }
    force_update();
    return *this;
}

vector<int> to_comp_id;
vector<vector<int>> Solution::get_components(cst(vector<int>) nodes) {
    sum_edges();
    int num_comps = 0;
    vector<vector<int>> comps;
    to_comp_id.assign(num_nodes + 1, -1);
    for (int u : nodes) {
        int rt = cc_handler.find_root(u);
        int comp_id;
        if (to_comp_id[rt] == -1) {
            comp_id = num_comps++;
            comps.emplace_back();
            to_comp_id[rt] = comp_id;
        } else comp_id = to_comp_id[rt];
        comps[comp_id].push_back(u);
    }
    return comps;
}

void Solution::connect_components(cst(vector<vector<int>>) comps) {
    auto labels = random_permutation(size(comps));
    for (int i = 1; i < size(comps); i++) {
        int u = labels[i]-1;
        int j = random_int(0, i-1);
        int v = labels[j]-1;
        u = random_element(comps[u]);
        v = random_element(comps[v]);
        sp_handler.trace_path(u, v, &gene, false);
    }
}

Solution& Solution::make_span() {
    connect_components(get_components(terminals));
    return *this;
}
vector<bool> is_node_involved;
Solution& Solution::make_span_wide(Real r_drop = 0) {
    vector<int> nodes(terminals);
    is_node_involved.assign(num_nodes+1, false);
    Iterate(gene, [&] (int idx) {
        possibly(r_drop, doing_nothing, 
            [&] {
                auto& [u,v,w] = edges[idx];
                is_node_involved[u] = is_node_involved[v] = true;
            });
    });
    for (int u = 1; u <= num_nodes; u++)
        if (is_node_involved[u] && !is_terminal[u]) nodes.push_back(u);
    connect_components(get_components(nodes));
    return *this;
}

Solution& Solution::mutate_hard(Real r_change = R_CHANGE) {
    int num_changes = std::max(2, int(num_edges * r_change));
    for (int _ = 0; _ < num_changes; _++) {
        int idx = random_int(0, num_edges-1);
        gene[idx].flip();
        mst_handler.change_bias(idx);
    }
    return this->make_span_wide(0.5).reduce(R_FLUCTUATE);
}

Solution& Solution::mutate(Real r_change = R_CHANGE) {
    int num_adds = std::max(2, int(num_edges * r_change));
    if (5 * count_edges() <= num_edges - num_adds) {
        while (num_adds > 0) {
            int idx = random_int(0, num_edges-1);
            if (!gene[idx]) {
                gene[idx].set(true);
                mst_handler.change_bias(idx);
                num_adds--;
            }
        }
    }
    else 
    {
        permute(edges_order);
        for (auto idx : edges_order)
        if (!gene[idx]) {
            gene[idx].set(true);
            mst_handler.change_bias(idx);
            if ((--num_adds) <= 0) break;
        }
    } 
    return this->reduce(R_FLUCTUATE);
}

void report_local_search() {
    cout << "LS success rate: " << CNT_LS_SUCC << " / " << CNT_LS_CALL 
        << ": " << ((Real) CNT_LS_SUCC / CNT_LS_CALL) << '\n';
}

int Solution::augment(Real r_change, int num_iter, bool is_random_rate = false) {
    Solution temp;
    CNT_LS_CALL += num_iter;
    int cnt = 0;
    for (int _ = 0; _ < num_iter; _++) {
        temp.mutate(is_random_rate ? random(0, r_change) : r_change);
        // Fuck, this bug clouded me. So my LS is very ineffective (1% instead of 99% :() 
        if (temp < (*this)) {
            (*this) = temp;
            ++cnt;
        }
        else temp.set_gene(gene); // rollback
    }
    CNT_LS_SUCC += cnt;
    return cnt;
}

pair<Solution,Solution> Solution::crossover(Solution& pal) {
    pair<Solution,Solution> children;
    bit::transform(all_of(gene), begin(pal.gene), begin(temp_gene), OPER_OR);
    Iterate(temp_gene, [&] (int idx) {
        if (gene[idx] != pal.gene[idx])
            possibly(0.5, [&] { temp_gene[idx].flip(); });
    });
    auto assign_solution = [&] (Solution& child) {
        child.set_gene(temp_gene);
        child.make_span_wide().reduce();
    };
    assign_solution(children.first);
    // pa XOR ma XOR child1 = child2
    bit::transform(all_of(gene), begin(pal.gene), begin(temp_gene), OPER_XOR);
    bit::transform(all_of(temp_gene), begin(children.first.gene), begin(children.second.gene), OPER_XOR);
    assign_solution(children.second);
    return children;
}

#define IS_SMALL_INSTANCE(x) ((x) <= 150)
#define IS_MEDIUM_INSTANCE(x) ((x) <= 1000)

std::ostream& operator<< (std::ostream& stream, Solution solution) {
    int n_edges = solution.count_edges();
    if (IS_SMALL_INSTANCE(n_edges)) {
        stream << "{";
        Iterate(solution.gene, [&] (int idx) {
            auto& [u,v,w] = edges[idx];
            stream << "(" << u << ',' << v << ") ";
        });
        stream << "}\n";
    } else
    if (IS_MEDIUM_INSTANCE(num_edges)) {
        stream << solution.gene.debug_string() << "\n";
    } else {
        stream << "|Size = " << n_edges << "|\n";
    }
    return stream; 
}

#endif // SOLUTION_H
