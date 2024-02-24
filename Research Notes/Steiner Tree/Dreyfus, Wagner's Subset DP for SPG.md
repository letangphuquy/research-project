---
tags:
  - paper
  - Algorithm
---
Cite: *Dreyfus, S. E., & Wagner, R. A. (1971). _Networks_, _1_(3), 195-207.
The Steiner problem in graphs*
Link: <https://web.vu.lt/mif/s.jukna/tropical/Dreyfus-Wagner.pdf>
## Summary
- Exponential time complexity exact algorithm for SPG.
- $O(n^3 + n^2 \cdot 2^k + n \cdot 3^k)$ where the term $3^k$ appears in subset traversal.
- Very limited practical capability ($3^k$ time and $2^k$ space complexity)
## Method:
Algorithm outline:
- Observation: By connecting a new node $q$ to a set of “already-connected" nodes, the “old set” can be split into 2 parts
- *DP bitmask*: use bitmask (binary representation of an integer) to manage the state - the set of connected special vertices
- Recursively compute 2 (supplementary) values:
	- $S_k(D)$ using $k$ as a pivot node, split the set $D$ into $E,F$ 
	- $S(m,D)$ the Steiner path that connects $m$ to $D$ 
## Result
- Have meanings for research
## References
