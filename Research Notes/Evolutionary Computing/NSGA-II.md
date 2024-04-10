---
tags:
  - paper
---
Cite: Deb, K., Pratap, A., Agarwal, S., & Meyarivan, T. A. M. T. (2002). 
A fast and elitist multi-objective genetic algorithm: NSGA-II. 
_IEEE transactions on evolutionary computation_, _6_(2), 182-197.
Link: [as suggested in GG Scholar](https://sci2s.ugr.es/sites/default/files/files/Teaching/OtherPostGraduateCourses/DoctoradoSantiagoCompostela/Bibliografia/Deb_NSGAII.pdf)
## Summary
- An $O(MN^2)$ (per generation) EA where $M$ is number of objectives and $N$ is population size.
- Refer to some older approach, which I believe is first steps in researching MOO
	- PAES: Pareto Archiving ES
	- SPEA: strength-Pareto EA
## Method:
- Fast non-dominating sorting: To find non-dominating fronts. Idea is identical to the [Kahn's algorithm for topological sorting](https://en.wikipedia.org/wiki/Topological_sorting) in time $O(MN^2)$ (the cost mostly in computing first front)
- Diversifying method: Improve over famous Fitness Sharing by calculating Crowding distance. Specifically, $$d_i = \sum_{m\in M} \frac{x_u(m) - x_v(m)}{f_{\max}(m) - f_{\min}(
m)}$$
	Where $m$ is an objective, $x_u, x_v$ are two closest points to $x_i$ in terms of $m$-th objective value, and each difference is divided by the range $[f_{\min}, f_{\max}]$ to normalize
- Constraint Handling:
	- *Constrained NSGA--* uses binary tournament to compare feasible and infeasible solutions, then extended the definition to *constrained-domination* relation
	- Ray-Tai-Seow: is implemented for comparison. Idea: emphasize on objective values in selection, on constraint violation value in crossover, and on combination of both elsewise.
## Result
- The algorithms are then compared using the following metrics:
	- Convergence $\gamma$ as average Euclidian distance to a chosen representatives of a known Pareto-optimal solution
	- Diversity (spread) $\Delta$ as sum of differences (between 2 consecutive points) to average distance, taken extreme point into account: $$\Delta = \frac{d_l + d_f + \Sigma (d_i - \bar{d})}{d_l + d_f + (N-1)\times \bar{d}}$$
- Result: Best diversity compared to older algorithms.
	- *linkage issues* in MOO (passing ref.)
- Data visualization: Use of scatter plot is suitable for representing convergence and diversity of Pareto-optimal **set**.
- Population size equals 100, run for 500 generations, or 200 (equivalent to $\le 25000$ Number of Function calls). Mutation index, crossover index and other tuning parameters coming from other algorithms, are all set as recommended.
## References
- [ ] .