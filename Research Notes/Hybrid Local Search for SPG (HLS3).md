---
tags:
  - Steiner
  - Heuristics
  - Graph
  - paper
---
Cite: *Poggi, Marcus & Ribeiro, Celso & Uchoa, Eduardo & Werneck, Renato. (2001). 
Hybrid Local Search for the Steiner Problem in Graphs.* 

## Summary
- Apply 3 neighborhood heuristics structure, and optimize time complexity for an efficient Local Search algorithm.
## Method:
- Define:
	- Steiner nodes
	- Key-nodes as terminal nodes or
- Adopt 3 neighborhood structures: 
	- node-based (add or remove a Steiner node) 
	- path-based (replacing path connects two key-nodes)
	- key-node based (add or remove a crucial node)
	- in all three neighborhood, shortest path is used to re-connect disjoined components
- Efficient computation of repeated [[Shortest Path Heuristic]] (SPH) (discovered by Takahashi and Matsuyama in 1980): Does not reset the label of Dijkstra between different iterations, thus giving better average complexity.
- The HLS3 algorithm combined 3 heuristics for local search cyclically:
	- When A reach local optima, change to B
	- Change B to C, and C to A like above
	- Termination condition: 
## Result
- Runs very fast
- Can reach to optimal for easy test, good ratio for large artificial test.
## References
3. [[Path and Vertex exchange for Steiner]]
4. [[Greedy Heuristics using Reoptimization and Supermodularity]]
5. [[Shortest Path Heuristic]]
6. [[Local Search for Steiner]] 