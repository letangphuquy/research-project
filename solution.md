### Context
Daniel Rehfeldt, Thorsten Koch have discovered and implemented very efficent exact solver for SPG. However, the approach is way too complicated. (Rehfeldt, D., Koch, T. Implications, conflicts, and reductions for Steiner trees. Math. Program. 197, 903–966 (2023). https://doi.org/10.1007/s10107-021-01757-5)

In this project, we attempt to solve by approximation, (more specifically GA and Evol. Comp. ).
The goal: a **blazingly fast** solver that returns an acceptable result.

Outline:
GA will consist of rounds:
- Choosing candidate (init population)
- Start loop:
  - Choose "best" candidates (survival of the fittest)
  - Crossover & Mutation (searching capability)
  - Selection (for e.g. $(\mu, \lambda)$)
  
To avoid local optima I want to adopt AEGA (Yong Liang, Kwong-Sak Leung,
Genetic Algorithm with adaptive elitist-population strategies for multimodal function optimization,
Applied Soft Computing,
Volume 11, Issue 2,
2011,
Pages 2017-2034,
ISSN 1568-4946,
https://doi.org/10.1016/j.asoc.2010.06.017.)

- Candidate: 
  - Random
  - Heuristics-based
- Init population will gone through Local Search
- GA phase (same as loop above).

TO-DO: Defence outline --> Implement --> Improve