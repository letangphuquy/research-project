---
tags:
  - paper
  - Heuristics
---
Cite: 
```bibtex
@article{MEHLHORN1988125,
title = {A faster approximation algorithm for the Steiner problem in graphs},
journal = {Information Processing Letters},
volume = {27},
number = {3},
pages = {125-128},
year = {1988},
issn = {0020-0190},
doi = {https://doi.org/10.1016/0020-0190(88)90066-X},
url = {https://www.sciencedirect.com/science/article/pii/002001908890066X},
author = {Kurt Mehlhorn},
keywords = {Steiner tree, approximation algorithm, graph algorithm},
```

## Summary
- Efficient implementation of Kou's [[Distance MST Heuristics (Kou'81)]], bounded only by 1 call to a single-source shortest path algorithm and 1 call to MST.
- Efficiency guaranteed by improving the bottleneck phase of Kou's algorithm: Calculate the complete distance graph.
## Method:
- Consider the partition $\{N(v) | v \in S\}$ contains all nodes in $V$ that are closest to $v$ compared to other special vertices. Specifically, $V = \bigcup N(v) \mid v \in S$, $N(s) \cap N(t) = \emptyset \forall s \neq t$, and:  
	- $v \in N(s) \Rightarrow d(v,s) \le d(v,t) \forall t \in S$
- Lemma: Consider auxiliary graph $G_1' = (S, E_1', d_1')$. Then, **every** MST on $G_1'$ is a MST on $G_1$. $G_1'$ is defined as:
	- $E_1'=\{(s,t) \mid s,t \in S \mid \text{ there exists } u \in N(s), v \in N(t) \text{ such that } (u,v) \in E\}$ 
	- $d_1'(s,t)=\min(d_1(s,u) + d(u,v) + d_1(v,t))$ with $u \in N(s), v \in N(t), (u,v) \in E$
- $N(v)$ is computed by a single SSSP call: Add an auxiliary vertex $s_0$ and edges $(s_0, s)$ with weight $0$ to all $s \in S$. Compute, also, $s(v): v \in N(s(v))$. Then, for $E_1'$, iterate through $(u,v) \in E$ and consider the triplet $(s(u), s(v), d_1(s(u), u) + d_1(s(v), v) + d(u,v))$ and update $d_1'(s,t)$ accordingly. (Remind that $|E_1'| \le |E|$)
- To achieve complexity $O(|V|\log |V| + |E|)$ for Shortest Path, the author recommended the use of [[Fredman and Tarjan's implementation for Dijkstra]], using Fibonacci Heap. 
- Maybe Prim is used for MST?
## Result
- For computing the Distance MST Heuristics, the authors reached the best possible complexity with a rather simple algorithm.
- However, because all-pair shortest paths aren't computed, it's not reusable for some other kind of procedures that require the all-pair distances.
- Fibonacci Heap has high constant factor. A more practical implementation should use min heap with time complexity $O((|V| + |E|) \log |E|)$ as stated in [e-maxx.ru 's tutorial](https://cp-algorithms.com/graph/dijkstra_sparse.html)
## References
