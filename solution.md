# Solution Outline

## Context

Daniel Rehfeldt, Thorsten Koch have discovered and implemented very efficent exact solver for SPG. However, the approach is way too complicated. (Rehfeldt, D., Koch, T. Implications, conflicts, and reductions for Steiner trees. Math. Program. 197, 903–966 (2023). <https://doi.org/10.1007/s10107-021-01757-5>)

In this project, we attempt to solve by approximation, (more specifically GA and Evol. Comp. ).
The goal: a **blazingly fast** solver that returns an acceptable result.

## Outline

Concern: Population diversity, exploitation vs. exploration balance. Un-stuck out of local optima?
Currently adopting:

- Elites + Longest Distance
- Dynamic $p_cross, r_mut$ (rate of change)
- Wild Migration (soft population reset)
- Minor parameters reset

Goal: Research AEGA as well as references cited in there (Yong Liang, Kwong-Sak Leung,
Genetic Algorithm with adaptive elitist-population strategies for multimodal function optimization,
Applied Soft Computing,
Volume 11, Issue 2,
2011,
Pages 2017-2034,
ISSN 1568-4946,
<https://doi.org/10.1016/j.asoc.2010.06.017>.)

- Solution Representation & Operators:
  - Binary String
  - Reduce: Performing MST and cut non-Steiner leaves
  - Mutate: Add few random edges then Reduce()
  - Make Span: Connect components in random order using Shortest Paths
    - (Wide): Also connects non-Steiner components

- Candidate: 4 types of heuristics
  - Random
  - Stem
  - Shortest
  - MST
- GA phase:
  - "Wild Migrations" to diversify
  - Ellitism + Longest Distance, direct promote to mating pool \\ LS
  - Selection (for recombination) (classical RWS) \\ k-best
  - Crossover: Uniform Xover, then Make Span
  - Mutation: Fixed P_Cross but adaptive rate of change (steps)
  - Survival: Remove obvious duplication and keep POP_SIZE fittest \\ LS for elite + LD

## Random ideas

Tangible:

- Better evaluation of fitness? New objective value as there maybe many very different solutions with same f
- Simulated Annealing (after GA)
- Modulize
- Constant factor
- AEGA-scheme
- Node-depth encoding

Bull**** (or not):

- Init population will gone through Local Search
- Each individual can have a set of different gene scheme in their DNA? (binary, real vector, ES, ...)
- Hybrid Reprensation: encode meta-heuristic information like $\alpha, \beta, \mu, \lambda$ of AEGA right in the gene. The "heuristic" will evolve with the population?.
  - How to fine-tune soft parameter auto- and program- tically?

## Reminders

- Read: Introduction to Evolutionary Algorithm (Book), ...
- Statistics processing & Paper before end of Feb

## Paper outline

```txt
REDUCTION
- degree test
- bottleneck distance test
- consecutive edge
- edge reduction
- node contraction

problem:
- effective re-computation of: replace one edge, query min?
* distance MST
OR
* shortest path heuristics

- key-node: add/ remove one, recompute heuristics
- key-path: 

O(|V| + |E|)

  Abstract
Introduction
  Steiner
  Evolutionary Algorithm
Problem formulation
Related Work
Method
 Solution Representation & Operator
  efficient bitset
  Local search VNS
  Randomness in calculation
 Proposal of 4 algorithms:
  SGA (naive GA)
  RGA (SGA + 2 Elitist and 3 Longest Distance for each)
  IGA (RGA + Dynamic Probability + distance sampling) 
  HGA (IGA with + reduction test/ better, deterministics Neighborhood Structure for Local Search)
Experimental Study
 benchmark 4 algorithms
 runtime, input time
 optimal value
 for HGA, also the reduction ratio (size |V| and size |E|)
Conclusion
```
