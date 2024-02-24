---
tags:
  - paper
  - Heuristics
---
Cite: L. Kou; G. Markowsky; L. Berman (1981). _A fast algorithm for Steiner trees. , 15(2), 141–145._ <doi:10.1007/bf00288961>
## Summary
- A $2(1-\frac{1}{l})$-APX algorithm for SPG: find MST on complete distance graph then remove unnecessary leaves.
- Idea: Attempt to “coerce” Steiner problem to MST problem
## Method:
Algorithm outline:
- Compute distance graph $D_G$ 
- For complete subgraph of $D$ containing only terminals, find a MST named $T$.
- Replace edges in $T$ by actual shortest paths in $G$, receive graph $H$
- $T \leftarrow MST(H)$. Then cut all non-terminal leaves
- *Note:* in case of multiple shortest paths, ties are broken arbitrarily.
## Result
- Another well-known heuristics
- Writing Style: Illustration figures improved clarity.