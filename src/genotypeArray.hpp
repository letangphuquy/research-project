#if !defined(GENOTYPE_ARRAY)
#define GENOTYPE_ARRAY

#include "array.hpp"

class Genotype {
private:
    Array<int> genes;
public:
    Genotype(int length = 0) {
        genes.free();
        genes.allocate(length);
    }
    int get(int i) { return genes[i]; }
    void set(int i, int value) { genes[i] = value; }
    void clear() { genes.clear(); }
    void append(int value) { genes.pushBack(value); }
    int size(void) { return genes.curSize; }
    int operator[] (int i) { return get(i); }
};

#endif // GENOTYPE_ARRAY
