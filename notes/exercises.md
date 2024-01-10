# My answers
for exercises in the book: [Introduction to Evolutionary Algorithms](file:///E:/Computer%20Science/Research/Genetic%20and%20Evolutionary%20Computation/[Decision%20Engineering%20]%20Xinjie%20Yu,%20Mitsuo%20Gen%20(auth.)%20-%20Introduction%20to%20Evolutionary%20Algorithms%20(2010,%20Springer)%20[10.1007_978-1-84996-129-5]%20-%20libgen.li.pdf)

## Chapter 2
### 2.1
Implement SGA from scratch: [Jobshop SGA implementation](SGA_jobshop.cpp).

Problem statement (classical job-shop scheduling):
- Given $n$ jobs, $m$ machines. Each jobs has exactly $m$ parts that must be completed in order. 
- The $j$-th part of the $i$-th job can only be processed by the machine with id $a_{ij}$ (in $t_{ij}$ unit of time).
- At a time, a machine can only process at most one job (part). Once started, it must finish the task at hand before receiving new one. 

What I did in there?
- Solution representation: Permutation of the multiset $\bigcup_{0 < i < n} \{ i\} \times {m}$, with the meaning: in which order should the jobs' parts be processed? Given the order, one can easily reconstruct the time in which job parts are processed (ie. calculating the objective value).
- Cross-over: Single-point crossover, similar to normal permutation crossing. Take the prefix of one parent, apply the suffix of the other whenever applicable, then fill in the "blank"s.
- Mutation: Simply swap 2 random genes
- $\texttt{pop\_size = 200, p\_c = 0.95, p\_m = 0.05}$. 

### 2.2
"In RWS, is the selection with replacement or without replacement?"

The question is quite confusing to me, because as I understand, RWS is for selecting the "best fit" individuals into mating pool.

The SGA presented in the chapter is, **selection without replacement**

### 2.3
1. Define the crossover rate $p_c$ and the mutation rate $p_m$:
To control and balance the behavior of the algorithm, enable searching capabilities.
2. What will happen if $p_m = p_c = 1$ in an SGA? If $p_m = 1$, all offspring's genes are inverted, thus not able to preserve parents' traits.

### 2.5
All of these notations: $(\mu + \lambda), (\mu, \lambda), (1 + 1), \dots$-ES refers to different selection schemes:
- $(\mu + \lambda)$: From $\mu$ parents, generate $\lambda$ offsprings. For the next generation, select $\mu$ best from those of $\mu + \lambda$ ones.
- $(\mu, \lambda)$: Similar to above, but selects $\mu$ best **only** in the $\lambda$ offsprings generated.

Therefore:
- $(1+1)$-ES: from one parent, creates one offspring and replaces it with the parent if it's better. Someone argued it looks like a "stochastic hill climber" 
- $(1+\lambda)$-ES: $\lambda$ mutants are generated, and then compete with parent.
- $(1,\lambda)$-ES: the parent is always disregarded.

### 2.4
Comparision table

| Criteria        | SGA                                                                                         | ES                                                                                         | EP                                                                                  |
|-----------------|---------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------|
| Solution  repr. | Binary                                                                                      | Real-valued                                                                                | Real-valued                                                                         |
| Steps           | 1. Initialize population  2. RWS (select fittest)  3*. Crossover  4. Mutation  5. Selection | 1. Init.  2. Picks random pair  3. Crossover  4*. Mutation  5. Selection                   | 1. Init.  2. Picks random individual  3*. Mutation, without crossover  4. Selection |
| Crossover       | Single point,  multiple point,  or uniform                                                  | Intermediate (insignificant)                                                               | None!                                                                               |
| Mutation        |                                                                                             | Normal distribution mutation (important exploration technique)                             | Same as ES                                                                          |
| Selection       | new generation replaces old one $(\mu,\mu)$                                                 | $(\mu + \lambda)$-ES, $(\mu, \lambda)$-ES, $(1+1), (1+\lambda), (1,\lambda)$-ES, $\dots$   | $(\mu + \mu)$-ES (kinda)                                                            |