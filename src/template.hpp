#if !defined(TEMPLATE_H)
#define TEMPLATE_H

#include <algorithm>
#include <vector>
#include <chrono>
#include <random>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <utility>
#include <string>
#include <bitlib/bitlib.hpp>

using std::vector, std::cin, std::cout, std::string, std::pair;

// Shorthand syntax
#define all_of(v) (v).begin(), (v).end()
#define cst(T) const T&

// Data types & Primitives
typedef long long Int;
typedef long double Real;
const Real EPS = 1e-9;
bool equals(cst(Real) x, cst(Real) y) { return std::abs(x-y) <= EPS; }
using WordType = uint64_t;
using Gene = bit::bit_vector<WordType>;

// Other shorthand syntax
template<class A, class B> bool umin(A& var, cst(B) val) {
	return (val < var) ? (var = val, true) : false;
}
template<class A, class B> bool umax(A& var, cst(B) val) {
	return (var < val) ? (var = val, true) : false;
}
// to iterate through set bit in bit::bit_vector

using InLoopAction = std::function<void(int)>;
void Iterate(Gene& gene, InLoopAction action) {
	WordType msk; 
	int block_idx = 0, idx, size = gene.size();
	int n_blocks = size / 64;
	auto it = begin(gene);
	for (; n_blocks --> 0; it += 64, block_idx += 64) {
		for (msk = *it.base(); msk > 0; msk &= msk-1) {
			idx = block_idx + __builtin_ctzll(msk);
			action(idx);
		}
	}
	// Buggy remainer part
	idx = block_idx;
	for (; it != end(gene); it++, idx++)
		if (*it) action(idx);
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



// Random
std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
template<class X, class Y> Int random_int(const X& l, const Y& r) {
    return std::uniform_int_distribution<Int>(l,r)(rng);
}
Real random(Real l = 0, Real r = 1) {
    return std::uniform_real_distribution<Real>(l,r)(rng);
}
using Void = std::function<void()>;
Void doing_nothing = []{};
void possibly(Real prob, cst(Void) func, cst(Void) callback = doing_nothing) {
    if (random(0,1) < prob) func(); else callback();
}

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
Real benchmark(Void task, string name = "") {
	cout << std::fixed << std::setprecision(8); 
	TimeVar t_start = timeNow();
	task();
	TimeVar t_end = timeNow();
	Real measurement = duration(t_end - t_start);
	cout << "Time consumed for";
	if (name.size()) cout << " " << name;
	cout << ":" << (measurement / 1e6) << "ms\n";
	return measurement;
}

string get_date_time(void) {
	std::time_t now_c = std::chrono::system_clock::to_time_t(timeNow());
    std::tm time_info = *std::localtime(&now_c); // You can use std::gmtime for UTC time
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &time_info);
	return buffer;
}

#define PRINT(s,x) s << #x << " = " << x << ' ';
#define PRINTLN(s,x) s << #x << " = " << x << '\n';
#define DBG(x) PRINT(std::cerr, x)
#define DBGn(x) PRINTLN(std::cerr, x);

// Testing
template<typename T> 
	T random_element(const vector<T>& v) {
		return v[random_int(0, (int) v.size()-1)];
	}
template<typename T> 
	T& random_element(vector<T>& v) {
		return v[random_int(0, (int) v.size()-1)];
	}

template<typename T> 
	void permute(vector<T>& v) {
		shuffle(v.begin(), v.end(), rng);
	}

vector<int> array_starts_from(int n, int s) {
	vector<int> v(n);
	iota(v.begin(), v.end(), s);
	return v;
}

vector<int> identity_permutation(int n) {
	return array_starts_from(n,1);
}

vector<int> random_permutation(int n) {
	auto res = identity_permutation(n);
	permute(res);
	return res;
}

// Constants
const int INF = 1e9;
const Real PI = acos(-1);
const Real EULER = exp((Real) 1);
const Real PHI = 1.618033988749;
const int N_MAX = 4097;

#endif // TEMPLATE_H