---
tags:
  - paper
  - Heuristics
---
Cite: *H. Takahashi and A. Matsuyama, "An approximate solution for the Steiner problem in graphs", Math. Japonica 24 (1980), 573-577*
## Summary
An $2\times(1-\frac{1}{k})$-APX solution to Steiner, analogous to Prim in case $k = |V|$
## Method
Algorithm outline:
- Consider a starting node $v_1$ from the set of special vertices.
- Let $T_i = (V_i, E_i)$ be the tree constructed in $i$-th phase. 
- Initially, $V_1 = {v_1}, E_1 = \emptyset$.
- From $T_i$, construct $T_{i+1}$ by finding $v_{i+1} = \arg\min d(V_i, v) \mid v \in S \setminus V_i$ (the nearest unadded special vertex), then add the shortest path connecting $T_i$ to $v_i+1$ by setting: 
	- $V_{i+1} = V_i \cup \{\text{nodes in PATH}(V_i, v_{i+1})\}$  
	- $E_{i+1} = E_i \cup \{\text{edges in PATH}(V_i, v_{i+1})\}$   
Proof of why the ratio is $2 - \frac{2}{k}$ and an example showing that the bound is tight:
- Each shortest path added is compared to a distance between pair of special vertices
## References
Links to "classical" algorithm: Dijkstra, Kruskal, Prim
[[Dreyfus, Wagner's Subset DP for SPG]]
