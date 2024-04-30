#if !defined(ARRAY_H)
#define ARRAY_H

/**
 * @brief Dynamic allocated size array.
 * 
 * Use pointer to dynamically allocate memory
 * Can append data (not exceeding capacity allocated) on-the-fly
 * O(1) access 
 */

template<class DataType> struct Array {
    DataType *arr;
    int maxSize, curSize;
    Array() { free(); }
    void free() {
        if (arr != nullptr) arr = nullptr;
        curSize = maxSize = 0;
    }
    void clear() { curSize = 0; }
    void allocate(int size) {
        arr = new DataType[size];
        maxSize = size;
    }
    int __index(int i) { return (i < 0 ? 0 : (i >= curSize ? curSize - 1 : i)); }
    DataType operator[] (int i) const { return arr[__index(i)]; }
    DataType& operator[] (int i) { return &arr[__index(i)]; }
    void pushBack(const DataType& item) {
        if (curSize < maxSize) arr[curSize++] = item;
    }
};

#endif // ARRAY_H
