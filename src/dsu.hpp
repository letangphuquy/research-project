#if !defined(DSU_H)
#define DSU_H
class DisjointSet {
private :
    vector<int> label;
public :
	DisjointSet(int size = 0) { init(size); }
	void init(int n) { label.assign(n+1, -1); }
	void fill(void) { std::fill(all_of(label), -1); }
  	
	int find_root(int u) {
		if (label[u] < 0) return u;
		return label[u] = find_root(label[u]);
	}
  
	bool merge_set(int u, int v) {
    	u = find_root(u), v = find_root(v);
		if (u == v) return false;
		if (-(label[u]) < -(label[v])) { u ^= v; v ^= u; u ^= v; } // swap
		return label[u] += label[v], label[v] = u, true;
	}
  	
  	bool same_set(int a, int b) { 
		return find_root(a) == find_root(b);
	}
} cc_handler; 

#endif // DSU_H
