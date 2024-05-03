#if !defined(GENOTYPE_ARRAY)
#define GENOTYPE_ARRAY

#include "array.hpp"

class Genotype : public Array<int> {
public:
    Genotype(int length = 1) { allocate(length); }
    // can't dynamic cast because of incomplete type
    Genotype(cst(Array<int>) arr) { 
        allocate(arr.maxSize);
        for (int i = 0; i < arr.curSize; i++) push_back(arr[i]);
    }
    int get(int i) const { return (*this)[i]; }
    void set(int i, int value) { arr[i] = value; }
    void append(int value) { push_back(value); }
    // void remove(int i) { genes.remove(i); }
    // void clear() { genes.clear(); }
    int size(void) const { return curSize; }
    int capacity(void) const { return maxSize; }
};

#endif // GENOTYPE_ARRAY
