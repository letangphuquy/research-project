---
tags:
  - paper
  - Heuristics
---
Cite: *Michel Minoux (1990) Efficient Greedy Heuristics For Steiner Tree Problems Using Reoptimization And Super Modularity, INFOR: Information Systems and Operational Research, 28:3, 221-233, DOI: <10.1080/03155986.1990.11732136>*  
**Efficient Greedy Heuristics For Steiner Tree Problems Using Re-optimization And Super Modularity**
Link: [Local](file:///E:/Computer%20Science/Research/Genetic%20and%20Evolutionary%20Computation/minoux1990.pdf)
## Summary
- An efficient algorithm for repeated calculation of  [[Distance MST Heuristics (Kou'81)]] in case of adding new node.
- Great solver for non-adjacency version
## Method:
- Pre-requisite(s): MST on complete distance subgraph. Denote by $G(Y) = (Y,U(Y))$ the corresponding complete distance subgraph of $Y$. For each $G(Y)$ we care about its Minimal Spanning Tree $T$.
- Algorithm 1: Find $s_0 = arg\,min{\Delta(s) = c(T \cup \{s\})} - c(T)$. Then add $s_0$ repeatedly until Delta is non-negative (while $\Delta(s_0) < 0$ do).
- Efficient re-optimization: From a MST $T$ for $(Y,U(Y))$, calculate MST $T'$ for $(Y', U(Y'))$ (with $Y' = Y \cup \{s\}$) in **only** $O(|Y|\cdot\text{degree})$ (which averages to $O(|Y|)$) instead of using Prim to compute MST from scratch in $O(|V|^2)$ .
	- To accomplish this complexity, suppose the newly inserted node $s$ have degree $p$. Repeat the following procedure $p$ times: add an edge incident to $s$, remove the largest edge on the new cycle ("chain") formed by the insertion. Out of many options, pick the edge that minimizes the difference.
- Based on super-modularity property of associated function, use principle of "Accelerated Greedy", optimize to get Algorithm 2
## Result
- A well-known heuristics applied in many Local Search algorithm for SPG.
## References
