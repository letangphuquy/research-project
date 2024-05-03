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
    DataType *arr = nullptr;
    int maxSize, curSize;
    Array() { free(); }
    ~Array() { free(); }
    Array(const Array& other) { (*this) = other; }
    Array& operator= (const Array& other) {
        if (this == &other) return *this;
        if (other.maxSize < 0) {
            std::cerr << "WTF?? ";
            DBG(curSize) DBG(maxSize) DBG(other.curSize) DBGn(other.maxSize)
            std::cerr << other[0] << '\n';
            // for (int i = 0; i < 200; i++) std::cerr << other[i] << ' '; std::cerr << '\n';
            // other.debug();
            // debug();
            // exit(1);
        }
        allocate(other.maxSize);
        for (int i = 0; i < other.curSize; i++) push_back(other[i]);
        return *this;
    }
    void free() {
        if (arr != nullptr) { delete []arr; arr = nullptr; }
        curSize = maxSize = 0;
    }
    void clear() { curSize = 0; }
    void allocate(int size) {
        free();
        size = std::max(1, size);
        arr = new DataType[size];
        for (int i = 0; i < size; i++) arr[i] = DataType();
        maxSize = size;
    }
    bool resize(int size) {
        curSize = 0;
        if (maxSize >= size) return false;
        allocate(size); return true;
    }
    int __index(int i) const { return std::max(0, std::min(curSize - 1 , i)); }
    const DataType& operator[] (const int i) const { return arr[__index(i)]; }
    DataType& operator[] (const int i) { return arr[__index(i)]; }
    // void set(int i, const DataType& value) { arr[__index(i)] = value; }
    void push_back(const DataType& item) {
        if (curSize < maxSize) arr[curSize++] = item;
    }
    void push_back(const Array<DataType>& other) {
        for (int i = 0; i < other.curSize; i++) push_back(other[i]);
    }
    void pop_back() { if (curSize > 0) --curSize; }
    void remove(int i) {
        if (0 <= i && i < curSize) {
            std::swap(arr[i], arr[curSize-1]);
            pop_back();
        }
    }
    void debug() const {
        for (int i = 0; i < curSize; i++) std::cerr << arr[i] << ' '; std::cerr << '\n';
    }
};

#endif // ARRAY_H
