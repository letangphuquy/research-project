---
tags:
  - paper
  - Evolutionary
---
Cite: *Tuan-Anh Do, Ha-Bang Ban, Minh Tu Le et al. , 30 October 2023, PREPRINT (Version 1) available at Research Square
Priority-Based Genetic Algorithm for the Clustered Steiner Tree Problem*
Link: [https://doi.org/10.21203/rs.3.rs-3507907/v1]
## Summary
- Solving each level of a two-levels algorithm similarly: Using classical [[Shortest Path Heuristic (takahashi80)]], but with modifications allows for traversing nodes in pre-defined order.
- Leveraging efficient, elegant chromosome representation: priority and order for SPH coded as real-valued vector
## Method:
- Problem Definition: Cluster Steiner Tree gives: $G = (V,E,c)$ and terminal (special, required, ... ) set of vertices $R$. Additionally, a partition $R_1, R_2, R_3, \dots, R_k$ is given. The problem asks to find a *Steiner* tree - a subtree $T(V_T, E_T)$ spans $R$ with minimal cost. Moreover:
	- The local subtree $T_i$ containing $R_i$ is disjoint from any other $T_j$ 
- Each solution is evaluated with a 2 level algorithm:
	- Find a local subtree $T_i$ for a cluster $R_i$ 
	- Treat each subtree as a node, then connect the clusters in the same spirit above.
- Representation: We can see each real value as: how "important" is a node, for it to be processed before others?
	- Efficient! Information is used extensively 
	- Max pri. in each cluster determine which cluster will be assigned subtree first
	- The value itself determine in each cluster, which order 'll be used to construct the subtree
	- Avg pri. in each cluster determine priority of "big node"
	- Thus, allows for more exploration
- Crossover: BLX
- Mutation: Polynomial mutation (Deb et al)
- Experimental setup and quantitative metrics: Friedman, Aligned Friedman, Quade
## Result
- Performance outperformed that of their old publications. But algorithm still consumes much time.
- Writing Style
- Application: Smart choose of topic, constrained and detailed problems are much more needed for enterprises customers.
- Hole(s) (not really): Optimal value not shown.
## References
- TO-INSERT