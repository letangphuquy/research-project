#include "template.hpp"
#include "testrun.hpp"
#include "input.hpp"
#include "solutionArray.hpp"

/*
STANDALONE IMPLEMENTATION:
NEW ALGORITHM IDEAS:
- New representation: (almost) dynamic array of chosen edges
- Heuristics
- Selection: Tournament (who wins also take diversity into account (sum of bitwise-level difference))
- Crossover: Good old mix
- Mutation: Self-correcting? Or random path-relinking
- Replacement: 
    only 40 from previous generations are kept
        . 1 elite
        . preserve relatively good & recent solutions
    60 childs: 40 fittest and 20 random
*/

/*
ALGORITHM PARAMETERS
redefine some constants in problem.hpp
*/

#define NUM_GEN 100
#define POP_SIZE 100
#define N_ELITE 1
#define N_SEED_PER_ELITE 0
#define N_SEED (N_ELITE * N_SEED_PER_ELITE)

#define TOURNAMENT_SIZE 3
#define P_CROSS 0.9

#define P_MUTATION 0.05
#define R_FLUCTUATE 0.2

#define NUM_TRANSFER 40
#define NUM_FIT_CHILD 40
#define AGE_THRESHOLD 5

Solution heuristics_tree(void) {
    // motivation: truly unbiased by "self-correcting" heuristics
    int root = random_element(terminals);
    vector<int> curEdges;
    Gene edgeInTree(num_edges+2, bit::bit0);
    vector<bool> nodeInTree(num_nodes+2, false);
    auto introduceNode = [&] (int u) {
        nodeInTree[u] = true;
        for (auto [e,_] : graph[u]) {
            if (!edgeInTree[e]) curEdges.push_back(e);
        }
    };
    introduceNode(root);
    while (!curEdges.empty()) {
        int e = random_element_without_replacement(curEdges);
        auto [u,v,w] = edges[e];
        if (nodeInTree[u]) std::swap(u,v);
        if (nodeInTree[u]) continue;
        edgeInTree[e].set(1);
        introduceNode(u);
    }
    Solution sol; sol.fromBitString(edgeInTree);
    return sol;
}

const int STEP = std::min(50, NUM_GEN / 20);
const int MILESTONE = NUM_GEN / 10; // for observing

#define popsize (population.size())
typedef vector<Solution> Social;
void debug_social(Social& pop, string title = "") {
    if (title.size()) cout << title << '\n';
    for (auto pi : pop)
        cout << '\t' << pi << ": " << pi.getObjval() << '\n';
}

vector<int> randTournament(int n, int k) {
    vector<int> tour;
    for (int i = 1; i <= n && k > 0; i++)
        possibly((Real) k / (n-i+1), [&] {
            tour.push_back(i);
            --k;
        });
    return tour;
}

void remove_duplication(Social& pop) {
    sort(all_of(pop));
    pop.erase(std::unique(all_of(pop)), end(pop));
}

Social population;
#define best_value (population[0].getObjval() + added_cost)

int main_algorithm(std::ofstream& out) {
    if (VERBOSE_LOG) cout << "Running algorithm...\n";
    vector<int> record(NUM_GEN+5, -1);
    auto ranking = [&] (Social& society) {
        for (auto &pi : society) pi.getObjval();
        sort(all_of(society));
    };
    auto aging = [&] (void) { for (auto &pi : population) pi.age++; };
    auto& population = ::population;
    population.clear();
    for (int _ = 0; _ < POP_SIZE; _++) population.push_back(heuristics_tree());
    aging();
    for (int igen = 1; igen <= NUM_GEN; igen++) {
        // Selection
        ranking(population);
        vector<int> mating_pool;
        for (int _ = 0; _ < POP_SIZE; _++) {
            auto tournament = randTournament(popsize, TOURNAMENT_SIZE);
            // who wins? currently choose best fit. TO-UPDATE (diversity score)
            mating_pool.push_back(*min_element(all_of(tournament)) - 1);
        }
        // Crossover
        Social offspring;
        for (int i = 0; i < size(mating_pool); i += 2) {
            auto dad = population[mating_pool[i]];
            auto mom = population[mating_pool[i+1]];
            possibly(P_CROSS, 
                [&] {
                    auto [c1,c2] = dad.crossover(mom);
                    offspring.push_back(c1);
                    offspring.push_back(c2);
                },
                [&] {
                    offspring.push_back(dad);
                    offspring.push_back(mom);
                }    
            );
        }
        // Mutation
        for (auto &child : offspring) child.mutate(R_CHANGE);

        // Survival
        ranking(population);
        Social newGeneration(cutVector(population, 0, N_ELITE));
        ranking(offspring);
        auto fitChilds(cutVector(offspring, 0, NUM_FIT_CHILD));
        newGeneration.insert(end(newGeneration), all_of(fitChilds));
        int numRandomChild = POP_SIZE - NUM_TRANSFER - NUM_FIT_CHILD;
        for (int _ = 0; _ < numRandomChild; _++)
            newGeneration.push_back(random_element_without_replacement(offspring));
        int numParents = NUM_TRANSFER - N_ELITE;
        sort(all_of(population), [&] (Solution a, Solution b) { 
            return a.getObjval() * a.age < b.getObjval() * b.age;
        });
        auto selectedParents(cutVector(population, 0, numParents));
        newGeneration.insert(end(newGeneration), all_of(selectedParents));

        swap(population, newGeneration);
        ranking(population);
        remove_duplication(population);
        aging();

        record[igen] = best_value;
        // Report
        if (DEBUG_MODE) {
            if (igen % STEP == 0)
                out << "Generation " << igen << "(" << popsize << "): " << population[0] << " with " << best_value << '\n';
        }
        if (VERBOSE_LOG) {
            if (igen % MILESTONE == 0)
                cout << "At " << igen << " got " << best_value << '\n';
        }
    }
    out << "Final " << population[0] << " with " << best_value;
    
    cout << "Summary: ";
    for (int i = 1; i <= NUM_GEN; i++) cout << record[i] << ' ';
    cout << '\n';
    
    cout << "Final = " << best_value << '\n';
    return best_value;
}

int main()
{
    MapType testset_start;
    SetType included_sets(set_union(SETS_BENCHMARK, SETS_BENCHMARK_ADDITIONAL));
    SetType excluded_sets;
    SetType included_tests;
    SetType excluded_tests;
    for (int i = 0; i < 40; i++) {
        run_tests(
            "NGA", 
            main_algorithm, 
            false, 
            testset_start, 
            included_sets, 
            excluded_sets, 
            included_tests, 
            excluded_tests,
            true
        );
    }
}
