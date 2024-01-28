#if !defined(TEMPLATE_H)
#define TEMPLATE_H

#include <algorithm>
#include <vector>
#include <chrono>
#include <random>
#include <math.h>
#include <iostream>
#include <utility>

using std::vector, std::cin, std::cout, std::string, std::pair;

/*BITWISE*/
#define p2(n) (1LL << (n))
#define testBit(n, bit) ((n) & p2(bit))
#define flipBit(n, bit) ((n) ^ p2(bit))
#define cntBit(n) (__builtin_popcountll(n))

/*SHORTCUTS*/
#define all_of(v) (v).begin(), (v).end()
#define cst(T) const T&

template<class A, class B> bool umin(A& var, cst(B) val) {
	return (val < var) ? (var = val, true) : false;
}
template<class A, class B> bool umax(A& var, cst(B) val) {
	return (var < val) ? (var = val, true) : false;
}

template<typename T> void populate_2d_array(T** arr, int nrows, int ncols, T fill_val = T()) {
	arr = new T* [nrows];
	for (int i = 0; i < nrows; i++) {
		arr[i] = new T[ncols];
		for (int j = 0; j < ncols; j++)
			arr[i][j] = fill_val;
	}
}

/*NUMERICS*/
typedef long long Int;
typedef long double Real;
//typedef __int128 Big;
//const int MODS[] = {(int) 1e9+7, (int) 1e9+9, 998244353, 2004010501};
const int MOD = 2004010501;

const Real EPS = 1e-9;
bool equals(cst(Real) x, cst(Real) y) { return std::abs(x-y) <= EPS; }

/*DEBUGGING*/
typedef std::chrono::high_resolution_clock::time_point TimeVar;

#define duration(a) std::chrono::duration_cast<std::chrono::nanoseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()
//Usage
//TimeVar tBegin = timeNow();
//TimeVar tEnd = timeNow();
//Real measurement = duration(tEnd-tBegin);
std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
template<class X, class Y> Int random(const X& l, const Y& r) {
    return std::uniform_int_distribution<Int>(l,r)(rng);
}

#define DBG(x) cerr << #x << " = " << x << ' ';
#define DBGn(x) cerr << #x << " = " << x << endl;

/*CONSTANTS*/
const Int INF = 1e18;
const Real PI = acos(-1);



#endif // TEMPLATE_H