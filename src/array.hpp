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
    ~Array() { free(); }
    void free() {
        if (arr != nullptr) { delete []arr; arr = nullptr; }
        curSize = maxSize = 0;
    }
    void clear() { curSize = 0; }
    void allocate(int size) {
        free();
        arr = new DataType[std::max(1,size)];
        for (int i = 0; i < size; i++) arr[i] = DataType();
        maxSize = size;
    }
    bool resize(int size) {
        if (maxSize >= size) return false;
        allocate(size); return true;
    }
    int __index(int i) const { return std::max(0, std::min(curSize - 1 , i)); }
    const DataType& operator[] (const int i) const { return arr[__index(i)]; }
    DataType& operator[] (const int i) { return arr[__index(i)]; }
    // void set(int i, const DataType& value) { arr[__index(i)] = value; }
    void pushBack(const DataType& item) {
        if (curSize < maxSize) arr[curSize++] = item;
    }
    void popBack() { if (curSize > 0) --curSize; }
    void remove(int i) {
        if (0 <= i && i < curSize) {
            std::swap(arr[i], arr[curSize-1]);
            popBack();
        }
    }
    void debug() {
        for (int i = 0; i < curSize; i++) std::cerr << arr[i] << ' '; std::cerr << '\n';
    }
};

#endif // ARRAY_H
