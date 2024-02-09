---
tags:
  - paper
  - Dynamic_Programming
  - Algorithm
  - Geometry
---
Cite: *Author, Year, Publisher, 
Title.*
Link: in Book [[Advances in Steiner Tree]], pp 27-38
## Summary
- An elegant DP algorithm
## Method:
- Based on observation: each x-coordinate with one terminal has exactly on "bar", and vice versa.
- Move a cutting-line from left to right, in a fashion similar to Sweep Line technique in *Competitive Programming*, then consider different type of configurations made when a horizontal line be cut by the moving vertical line.
- Let $k$ be the number of horizontal lines.
- The DP state consist enough information on the connectivity of the forest, thus calculate-able
- Complexity: Space $O(n\times k5^k)$ and Time $O(n\times k^3 \times (2\cdot 5)^k)$ 
## Result
- Inspired further research
- Low applicability (in direct)
## References
- [[Hanan Grid]]
- [[SP is NP-complete]]