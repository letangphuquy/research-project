#if !defined(SOLUTION_ARRAY_H)
#define SOLUTION_ARRAY_H

#include "template.hpp"
#include "interfaces/ISolution.hpp"
#include "genotypeArray.hpp"

#include "dsu.hpp"
#include "mst.hpp"
#include "graph.hpp"
#include "problem.hpp"
#include <queue>

#define INF 1'000'000'000

class Solution: public ISolution
{
private:
    Genotype chromosome;
    pair<bool,int> objVal; 

    void selfCorrect() {}
public:
    int age;
    Solution() {
        chromosome = Genotype(num_nodes);
        objVal = {false, INF}; age = 0;
    }
    void fromBitString(Gene gene) {
        chromosome.clear();
        Iterate(gene, [&] (int idx) { chromosome.append(idx); });
        selfCorrect();
    }
    void fromVector(cst(vector<int>) vec) {
        chromosome.clear();
        for (auto x : vec) chromosome.append(x);
        selfCorrect();
    }
    int getObjval(void);
    bool operator< (Solution rhs) { return getObjval() < rhs.getObjval(); }
    void mutate(Real pMutate);
    void mutate(void) { mutate(P_MUTATION); };
    pair<Solution, Solution> crossover(const Solution& mate);
};

int Solution::getObjval() {
    if (!objVal.first) { 
        objVal.second = INF;
    }
    return objVal.second;
}

pair<Solution, Solution> Solution::crossover(const Solution& mate) {
    return std::make_pair(Solution(), Solution());
}

void mutate(Real pMutate) {

}

std::ostream& operator<< (std::ostream& stream, Solution solution) {
    return stream;
}


#endif // SOLUTION_ARRAY_H
