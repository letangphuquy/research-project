---
tags:
  - paper
---
Cite: *Eshelman, Larry J. (1991). [Foundations of Genetic Algorithms] Volume 1 , (), 265–283.  
The CHC Adaptive Search Algorithm: How to Have Safe Search When Engaging in Nontraditional Genetic Recombination*
Link: doi:10.1016/b978-0-08-050684-5.50020-3
## Summary
- Conservatism selection operator is most suitable for disruptive crossover operator. Discussions and reasons why he does something are all based on math basis. 
- Many guidelines on how to preserve diversity and get out of local optimum. Altogether, make the CHC Adaptive Search algorithm
## Method:
- "Disruptive recombination operators are *useful*". Deep conversation on Crossover: Long and short defining length, investigations on H1X and H2X. Decided to use Uniform crossover: HUX
- Selection: choose $\texttt{popsize}$ best from generation $t\,(P_t)$ **and** from new-born children (candidate for generation $t+1$). "Cross-generational" selection
- Algorithm outline:
	- Generate $\texttt{pool}$ as a permutation of $P_t$ . Then any 2 consecutive parent is recombined
	- $C_t$ are children from the above proc.
	- Select best from $P_t \cup C_t$
- Diversity maintenance methods:
	- "Incest Avoiding": if Hamming distance between two parents $\le \text{threshold}$, then remove them. The difference threshold will be decreased gradually. 
	- "Soft Restart" in the role of Mutation: Pick the best element as template for new population, then each individual other than the template will have $(\texttt{diverge\_rate = 0.35}) \times \texttt{num\_bits}$ flipped   
## Result
- Outperforms GA on test function. Old way of testing: Tweaking parameters to see what's most suitable for each test.
- Writing Style: a bit discussion-like, wall-of-text and lack of illustrations
- Application: The author shows one extension for permutation representation
## References
- [[Genetic Algorithms]]
- "Old" references coming from GECCO & literature