---
tags:
  - paper
  - Local_Search
---
Cite: *Martins, S., Resende, M., Ribeiro, C. _et al.__ **17**, 267–283 (2000). Journal of Global Optimization. 

A Parallel Grasp for the Steiner Tree Problem in Graphs Using a Hybrid Local Search Strategy.*
Link: https://doi.org/10.1023/A:1026546708757 ([Local](file:///E:/Computer%20Science/Research/Steiner%20Tree/A%20Parallel%20Grasp%20for%20the%20Steiner%20Tree%20Problem%20in%20Graphs%20Using%20a%20Hybrid%20Local%20Search%20Strategy.pdf))
## Summary
- GRASP framework: 2-phase metaheuristics
	- Construction phase: Greedy Randomized solution
	- Second phase: Local search on the solutions
- Combine 2 
## Method:
More details:
- Initialization solution: Well-known heuristics
- Adopt 2 neighborhood:
	- Node-based: 
		- Node Insertion, suggested by [[Minoux's Node inserting Heuristics]]
		- Node Removal as the re-computation of MST
	- Path-based: $T\setminus p_{ij} \cup sp_{uv} | u \in C_i, v \in C_j$ (take two key-node $i,j$, remove the current path between them, two disjointed components are $C_i, C_j$ respectively, connect them with a pair of 2 nearest nodes).
## Result
- For test set C, D, the obtained values' re near optimal
## References
For further reading, and awesome Graph feature!