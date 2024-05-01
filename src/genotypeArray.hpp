#if !defined(GENOTYPE_ARRAY)
#define GENOTYPE_ARRAY

#include "array.hpp"

class Genotype : public Array<int> {
public:
    Genotype(int length = 0) {
        free();
        allocate(length);
    }
    Genotype(Array<int> arr) { (*this) = arr; }
    int get(int i) const { return (*this)[i]; }
    void set(int i, int value) { arr[i] = value; }
    void append(int value) { pushBack(value); }
    // void remove(int i) { genes.remove(i); }
    // void clear() { genes.clear(); }
    int size(void) const { return curSize; }
    int capacity(void) const { return maxSize; }
    int operator[] (int i) const { return get(i); }
};

#endif // GENOTYPE_ARRAY
