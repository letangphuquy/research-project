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
    int get(int i) const { return genes[i]; }
    void set(int i, int value) { genes[i] = value; }
    void clear() { genes.clear(); }
    void append(int value) { genes.pushBack(value); }
    int size(void) const { return genes.curSize; }
    int capacity(void) const { return genes.maxSize; }
    int operator[] (int i) const { return get(i); }
};

#endif // GENOTYPE_ARRAY
