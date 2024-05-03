#if !defined(BOOK_KEEPER_H)
#define BOOK_KEEPER_H

#include "array.hpp"

class BookKeep {
private:
    Array<int> timer, counter;
    int curTime;
public:
    BookKeep(int n = 1) { curTime = 0; resize(n); }
    ~BookKeep() { timer.~Array(); counter.~Array(); }
    void resize(int n) {
        if (size() >= n) return ;
        timer.allocate(n);
        counter.allocate(n);
        for (int i = 0; i < n; i++) {
            counter.push_back(0);
            timer.push_back(0);
        }
    }
    void reset(void) {
        for (int i = 0; i < size(); i++) {
            timer[i] = 0;
            counter[i] = 0;
        }
    }
    int size(void) const { return timer.maxSize; }
    void tick() { ++curTime; }
    int get(int i) { return timer[i] >= curTime ? counter[i] : 0; }
    void inc(int i) {
        if (timer[i] < curTime) { timer[i] = curTime; counter[i] = 0; }
        ++counter[i];
        // if (timer[i] < curTime) { timer.set(i, curTime); counter.set(i, 0); }
        // counter.set(i, counter[i] + 1);
    }
    void debug(void) {
        timer.debug();
        counter.debug();
    }
} marker;

#endif // BOOK_KEEPER_H
