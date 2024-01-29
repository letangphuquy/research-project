#if !defined(TEMPLATE_H)
#define TEMPLATE_H

#include <algorithm>
#include <vector>
#include <chrono>
#include <random>
#include <math.h>
#include <iostream>
#include <utility>
#include <string>
#include <bitlib/bitlib.hpp>

using std::vector, std::cin, std::cout, std::string, std::pair;

// Shorthand syntax
#define all_of(v) (v).begin(), (v).end()
#define cst(T) const T&

template<class A, class B> bool umin(A& var, cst(B) val) {
	return (val < var) ? (var = val, true) : false;
}
template<class A, class B> bool umax(A& var, cst(B) val) {
	return (var < val) ? (var = val, true) : false;
}

template<typename T> void populate_2d_array(T**& arr, int nrows, int ncols, T fill_val = T()) {
	arr = new T* [nrows];
	for (int i = 0; i < nrows; i++) {
		arr[i] = new T[ncols];
		for (int j = 0; j < ncols; j++)
			arr[i][j] = fill_val;
	}
}

template <typename T> void free_2d_array(T** arr, int nrows) {
    for (int i = 0; i < nrows; i++)
        delete[] arr[i];
    delete[] arr;
}

// Data types
typedef long long Int;
typedef long double Real;
const Real EPS = 1e-9;
bool equals(cst(Real) x, cst(Real) y) { return std::abs(x-y) <= EPS; }
using WordType = uint64_t;
using Gene = bit::bit_vector<WordType>;

// Debugging and Benchmarking
typedef std::chrono::high_resolution_clock::time_point TimeVar;
#define duration(a) std::chrono::duration_cast<std::chrono::nanoseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()
/*
Usage
	TimeVar tBegin = timeNow();
	TimeVar tEnd = timeNow();
	Real measurement = duration(tEnd-tBegin);
*/

std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
template<class X, class Y> Int random_int(const X& l, const Y& r) {
    return std::uniform_int_distribution<Int>(l,r)(rng);
}
Real random_num(Real l, Real r) {
    return std::uniform_real_distribution<Real>(l,r)(rng);
}

#define DBG(x) cerr << #x << " = " << x << ' ';
#define DBGn(x) cerr << #x << " = " << x << endl;

// Constants

const Int INF = 1e18;
const Real PI = acos(-1);

#endif // TEMPLATE_H