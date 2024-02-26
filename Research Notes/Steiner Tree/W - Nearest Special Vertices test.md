---
tags:
  - idea
  - Graph
---
Cite: 
- \[1\]: Duin, C. W., & Volgenant, A. (1989). Reduction tests for the Steiner problem in graphs. _Networks_, _19_(5), 549-567.
- \[2\]: Koch, T., & Martin, A. (1998). Solving Steiner tree problems in graphs to optimality. _Networks: An International Journal_, _32_(3), 207-232.
Link: 
## Summary
- One of the most important reduction test, adopted widely in literature.
## Idea:
**Notation**
Given an undirect, weighted graph $G = (V,E)$ and a cost function $c$ mapping $c : E \mapsto \mathbb{R}^+$. A Steiner Tree for $G$ and a set $R \subseteq V$ of terminal vertices (or 'special', 'required' vertices) is a subgraph $T = (V(Y), Y)$ that ***spans*** $R$ (i.e, $\forall s,t \in R$, there exists a path from $s$ to $t$ in $T$) with $Y \subseteq E$ is the subset of chosen edges and by $V(Y)$ we denote the set of vertices incident to the edge set $Y$.
The Steiner Minimal tree problem asks to find the tree $T^*$ with minimum sum of cost: $T^* = \arg\min c(T)$ with $$c(T) = \sum_{e \in Y} c(e)$$Standard graph notation: 
- $\delta(v)$ is the set of edges incident to $v$. Similarly $\delta(W) = \bigcup_{v \in W} \delta(v)$ 
- For understanding of the term "chord", refer to these lectures: <http://compalg.inf.elte.hu/~tony/Oktatas/TDK/FINAL/Chap%204.PDF> and <http://www.vpscience.org/materials/Graph%20Theory%20(US06CMTH05)_Unit_3_Web.pdf> 
**The test**
It was described differently through the literature, based on authors' interpretation of the idea and intention of their approach to the problem. 
- KM98 described this as distance between 2 subsets of the graph, based on the *induced cut*. More formally: "Consider a connected subgraph $H = (W,F)$ of $G$ such that $R \cap W \neq \emptyset$ and $R \setminus W \neq \emptyset$". Let $e,f$ be the shortest and second shortest edge of the cut *induced* by $W$. More formally: $e = \arg\min_{e' \in \delta(W)} c(e')$ and $f = \arg\min_{f' \in \delta(W) \setminus \{e\}} c(f')$ . Then, $e = [u,v]$ with $u\in W, v\in V\setminus W$ exists in some optimal solution if $$c(f) \ge d_u + c(e) + d_v$$ with $d_u = \min\{d(t,u) \mid t \in R \cap W\}$ and $d_v = \min\{d(t,v) \mid t \in R \setminus W\}$: $d_u, d_v$ are distance from nearest special vertices on both sides to the edge $e$, respectively.
	*Explanation:* Every solution must use some edge across this induced cut, to connect the special vertices lying on two sides. Because the inequality holds, in any solution containing edge other than $e$ (for example $f$), we can replace it with $e$ plus 2 chains connecting to nearest special vertices and obtain a better Steiner tree.
- In [[W - Esbensen's GA for SPG (95)]], he described it as an easy-to-implement test: consider a special vertex $v \in R$ and two vertices $u,w$ the closest and second closest to it. Then, consider $z \in R \setminus \{v\}$ the closest special vertex to $u$, excluding $v$. The edge $[v,u]$ is included in every optimal Steiner tree if: $$c([v,w]) \ge c([v,u]) + c(sp(u,z))$$, the explanation is similar.
- In the original work [[Reduction Tests for SPG (Duin'89)]], he stated it like this:
	- Theorem 1: "For an edge $(i,j)$ to be a Steiner edge there exists $k,k' \in R$ such that $d(k,k') = d(k,i) + c(i,j) + d(j,k')$ and every path in $G \setminus \{i,j\}$ from $k$ to $k'$ contains an edge $(s,t)$ of cost $c([s,t]) \ge d(k,k')$". 
		- Interpretation: $(i,j)$ is on the shortest path from $k$ to $k'$ and every other Steiner tree can replace its edge $(s,t)$ with the path $k \rightarrow i \rightarrow j \rightarrow k'$ to obtain a better solution.
	- Lemma 1 was proposed for efficient checking. It goes like this: 
	  "Edge $(i,j) \in E$ satisfies Theorem 1 $$\Leftrightarrow \underline{b}(k_1,k_2) \ge d(k_1, i) + c(i,j) + d(j, k_2)$$" 
	  with $\underline{b}$ is the bottleneck length in restricted graph $\underline{G} = G \setminus \{(i,j)\}$ and $k_1, k_2$ are chosen, such that $k_1 = \arg\min_{k \in R} \{d(k,i) \mid  d(k,i) < d(k,j)\}$ and $k_2 = \arg\min_{k \in R} \{d(k,j) \mid  d(k,j) < d(k,i)\}$ ($k_1, k_2$ are nearest special vertices lying on different sides of the edge $(i,j)$)
	- Further more, $(i,j)$ must be in a MST $T$ of $G$. We need to check $O(|V|)$ edges, for each we can choose $k_1, k_2$ in $O(|R|)$. The remaining task is to calculate the length of the shortest chord $(v,w)$ that *replaces* $(i,j)$ (that means if we replace edge $(i,j)$ with edge $(v,w)$, then $T$ would still be a tree), and check if: $c(v,w) \ge d(k_1,i) + c(i,j) + d(j,k_2)$
		- The authors stated $O(|V|^2)$ complexity is needed for all branch-chord exchanges? It's not obvious, so I will employ some other algorithm with higher complexity, for example:
			- An $O(|V||E|)$ algorithm: try to delete each MST edge, mark the two component with different colors then iterate through all non-MST edge to identify chords
			- A better $O(|E|\log |V|)$ algorithm: iterate through each chord, then use a data structure to quickly mark all MST edge "replace-able" by it (Heavy-Light Decomposition). A better algorithm would be of $O(|E| + V\log|V|)$ complexity, where chords are added as events on tree and considered in a sweep-line -like fashion (LCA + DS).  
			- Why finding shortest chord instead of re-compute the bottleneck length, or why just consider MST edges is enough? I didn't get the reason and proof behind that.
## Result
- In conjunction with several other tests, the graph size of the problem instances reduced significantly and thus was practiced by many authors.
## References
- [[Reduction Tests for SPG (Duin'89)]]