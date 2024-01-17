#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <set>
#include <cassert>
#include <numeric>

using namespace std;

/*
https://en.wikipedia.org/wiki/Job-shop_scheduling
*/
typedef long long Int;
typedef long double Real;

#define DBG(x) cout << #x << " = " << x << ' ';
#define DBGn(x) cout << #x << " = " << x << '\n';

const Real EPS = 1e-7;

#define all(v) (v).begin(), (v).end()
#define cst(T) const T&

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
template<class X, class Y> Int random_integer(const X& l, const Y& r) {
    return uniform_int_distribution<Int>(l,r)(rng);
}
Real random(cst(Real) l, cst(Real) r) {
    return uniform_real_distribution<Real>(l,r)(rng);
}

const int N = 105;
int num_jobs, num_machines;
int parts[N][N], weight[N][N];

void generate_random_input(vector<string> v_opts) {
    set<string> opts(all(v_opts));
    int LIM = 100;
    int VALUE = 100;
    if (opts.count("small")) LIM = 10; else
    if (opts.count("medium")) LIM = 30;
    if (opts.count("huge")) VALUE = 1e5;
    num_jobs = random_integer(LIM/2, LIM);
    num_machines = random_integer(LIM/2, LIM);
    for (int i = 0; i < num_jobs; i++) {
        iota(parts[i], parts[i] + num_machines, 0);
        shuffle(parts[i], parts[i] + num_machines, rng);
        for (int j = 0; j < num_machines; j++)
            weight[i][j] = random_integer(1, VALUE);
    }
    cout << "INPUT:\n";
    DBG(num_jobs) DBGn(num_machines)
    for (int i = 0; i < num_jobs; i++) {
        cout << "Job number " << i << ": ";
        for (int j = 0; j < num_machines; j++)
            cout << "(" << parts[i][j] << ',' << weight[i][j] << ") ";
        cout << '\n';
    }
}

const int INIT = -1;
class Solution {
private:
    // stores which order tasks will be executed.
    vector<int> permutation;
    int objective_value;
    void init(void) {
        for (int i = 0; i < num_jobs; i++) {
            for (int j = 0; j < num_machines; j++)
                permutation.push_back(i);
        }
    }
    
    void calc_objective_function(void) {
        vector<int> last(num_machines, 0), iter(num_jobs, 0), time_spent(num_jobs, 0);
        for (auto job : permutation) {
            auto& it = iter[job];
            int p = parts[job][it];
            int start = max(time_spent[job], last[p]);
            time_spent[job] = last[p] = start + weight[job][it];            
            it++;
        }
        objective_value = *max_element(all(last));
        assert((objective_value > 0)); // for min-problem
    }
public:
    Solution() { objective_value = INIT; init(); }
    void shuffle(void) {
        std::shuffle(all(permutation), rng);
    }
    void mutate(void) {
        int i = random_integer(0, size(permutation) - 1);
        int j = random_integer(0, size(permutation) - 1);
        swap(permutation[i], permutation[j]);
    }
    friend ostream& operator<<(ostream& os, const Solution& dt);
    void set(int p, int v) { permutation[p] = v; }
    int get(int i) const { return permutation[i]; }
    int get_objective_value(void) {
        if (objective_value == INIT) calc_objective_function();
        return objective_value;
    }
    int recalculate(void) {
        calc_objective_function();
        return objective_value;
    }
    pair<Solution, Solution> operator* (cst(Solution) other) {
        // single-point crossover
        int length = int(size(permutation));
        int pos = random_integer(1, length);
        pair<Solution, Solution> children;
        vector<int> cnt_first(num_jobs, num_machines);
        vector<int> cnt_second(all(cnt_first));
        // inherit prefix
        for (int i = 0; i < pos; i++) {
            children.first.set(i, this->get(i));
            cnt_first[this->get(i)] -= 1;
            children.second.set(i, other.get(i));
            cnt_second[other.get(i)] -= 1;
        }
        // inherit suffix if applicable
        #define NONE -1
        for (int i = pos; i < length; i++) {
            if ((cnt_first[other.get(i)] -= 1) >= 0)
                children.first.set(i, other.get(i));
            else children.first.set(i, NONE);
            if ((cnt_second[this->get(i)] -= 1) >= 0)
                children.second.set(i, this->get(i));
            else children.second.set(i, NONE);
        }
        // supplementary
        for (int j = 0, i = 0; i < length; i++) {
            for (; children.first.get(i) == NONE && j < length; j++)
                if ((cnt_first[other.get(j)] -= 1) >= 0) {
                    children.first.set(i, other.get(j));
                }
        }
        for (int j = 0, i = 0; i < length; i++) {
            for (; children.second.get(i) == NONE && j < length; j++)
                if ((cnt_second[this->get(j)] -= 1) >= 0) {
                    children.second.set(i, this->get(j));
                }
        }
        return children;
    }  
};

ostream& operator<<(ostream& out, const Solution& sol) {
    out << "solution: "; 
    for (int i = 0; i < num_jobs * num_machines; i++) 
        out << sol.get(i) << ' ';    
    out << '\n';
    return out;
}


Solution random_solution(void) {
    const int NUM_ITERS = 50;
    Solution sol;
    for (int t = 0; t < NUM_ITERS; t++) sol.shuffle();
    return sol;
}

vector<Solution> roulette_wheel_selection(vector<Solution>& population) {
    // minimization problem --> let f_i = 1 / f_i then RWS like in max-problem
    int popsize = int(size(population));
    static vector<Real> prob;
    prob.resize(popsize);
    Real sum_f_inv = 0;
    for (int i = 0; i < popsize; i++) {
        prob[i] = (Real) 1 / population[i].get_objective_value();
        sum_f_inv += prob[i];
    }
    for (auto &p_i : prob) p_i /= sum_f_inv;
    // prefix sum array + binary search
    for (int i = 1; i < popsize; i++)
        prob[i] += prob[i-1];
    vector<Solution> mating_pool;
    for (int _ = 0; _ < popsize; _++) {
        Real rate = random(0,1);
        int low = 0, high = popsize-1, pos = high;
        while (low <= high) {
            int mid = (low + high) / 2;
            if (rate <= prob[mid])
                pos = mid, high = mid - 1;
            else 
                low = mid + 1;
        }
        mating_pool.push_back(population[pos]);
    }
    return mating_pool;
}

void simple_genetic_algorithm() 
{
    const int POP_SIZE = 200; 
    const int NUM_GEN = 5000;
    const Real P_CROSS = 0.95;
    const Real P_MUT = 0.05;  
    vector<Solution> population;
    for (int i = 0; i < POP_SIZE; i++)
        population.push_back(random_solution());
    Solution best = population[0];
    for (int _ = 0; _ < NUM_GEN; _++) {
        vector<Solution> mating_pool = roulette_wheel_selection(population);
        vector<Solution> offspring;
        while (size(offspring) < 2 * POP_SIZE) {
            int a = random_integer(0, POP_SIZE-1);
            int b = random_integer(0, POP_SIZE-1);
            if (random(0,1) <= P_CROSS) {
                auto children = population[a] * population[b]; // cross-over
                if (random(0,1) <= P_MUT) children.first.mutate();
                if (random(0,1) <= P_MUT) children.second.mutate();
                offspring.push_back(children.first);
                offspring.push_back(children.second);
            }
        }
        for (auto &sol : offspring) sol.recalculate();
        sort(all(offspring), [&] (Solution a, Solution b) {
            return a.get_objective_value() < b.get_objective_value();
        });
        offspring.resize(POP_SIZE);
        swap(population, offspring);
        if (population[0].get_objective_value() < best.get_objective_value())
            best = population[0];
        // for (int i = 0; i < 5; i++) 
        //     cerr << "\t" << population[i] << " has " << population[i].recalculate() <<  '\n';
        cerr << "Generation: " << _+1 << " -> " << population[0].get_objective_value() << '\n';
    }
    cout << "After " << NUM_GEN << " generations: " 
        << best << " -> " << best.get_objective_value() << '\n';
    cout << "__________________________________________________";
}

int main()
{
/*
    ios_base::sync_with_stdio(0);
    cin.tie(0); cout.tie(0);

    #define task "WF"
    if (fopen(task".inp", "r")) {
        freopen(task".inp", "r", stdin);
        freopen(task".out", "w", stdout);
    }
*/
    freopen("log.txt", "a", stdout);
    vector<string> opts({});
    generate_random_input(opts);
    simple_genetic_algorithm();
}