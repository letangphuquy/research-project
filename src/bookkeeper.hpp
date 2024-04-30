#if !defined(BOOK_KEEPER_H)
#define BOOK_KEEPER_H

#include "array.hpp"

class BookKeep {
private:
    Array<int> timer, counter;
    int curTime = 0;
public:
    BookKeep(int n = 1) {
        curTime = 0;
        timer.allocate(n);
        counter.allocate(n);
    }
    int size(void) const { return timer.maxSize; }
    void tick() { ++curTime; }
    int get(int i) { return timer[i] >= curTime ? counter[i] : -1; }
    void inc(int i) {
        if (timer[i] < curTime) { timer[i] = curTime; counter[i] = 0; }
        ++counter[i];
    }
} marker;

#endif // BOOK_KEEPER_H
