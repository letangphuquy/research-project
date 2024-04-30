#if !defined(SOLUTION_INTERFACE_H)
#define SOLUTION_INTERFACE_H

#include <utility>
using std::pair;

// template<class Genotypic> 
class ISolution 
{
// private:
//     Genotypic chromosome;
public:
    ISolution() {}
    virtual int getObjval(void) = 0; // whether to use cache on objective value computation is up to implementator
    // virtual bool operator< (const ISolution& rhs) = 0;
    virtual void mutate(void) = 0;
    // virtual pair<Solution, Solution> crossover(const Solution& mate);
};

#endif // SOLUTION_H