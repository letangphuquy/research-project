---
tags:
  - paper
  - Evolutionary
---
Cite: *Yong Liang; Kwong-Sak Leung (2011) Applied Soft Computing 11 (2011), 
Genetic Algorithm with adaptive elitist-population strategies for multimodal function optimization.*
Link: doi:10.1016/j.asoc.2010.06.017
## Summary
- By adjusting population size according to *Principle of Dissimilarity*, AEGA outperforms old approaches in literature in both efficiency and efficacy.
// They cited a numerous of attempts to keep population diversity in literature 
## Method:
- Representation is normalized to $[0,1]^n$ 
- Elitist Crossover:
	- get 2 random child on the line connecting (BLX-0.5): 
		- $c_i = p_i + \mu_1 \times (p_j - p_i)$ 
		- $c_j = p_j + \mu_2 \times (p_i - p_j)$ 
	- based on: Relative Direction, Distance, Objective value; decide whether and which child will replace its parent
	- addresses weakness of distance based policy: "there maybe organisms lie on different peak, being close to each other"
	- popsize may shrink
- Elitist Mutation: 
	- (Mühlenbein’s operator) $c_i = p_i \pm \lambda \times r_m$ 
	- if detect new peak then add the corresponding individual to population.
- Population Control in role of Selection: $\alpha, \beta, \mu, \lambda$ 
	- If an individual lived for $\alpha$ generations, but not improved any better than $\beta \times$ best-so-far, it is deleted
	- If at a point popsize expands to $\lambda \times$ original-size, it shrinks down to $\mu \times$
	- Tinh thần: "Give chances to low-fit, but don't waste to much time on ineffective ones"
## Result
- Well-designed experiment: careful and thorough test functions
- Writing Style: Reference as a standard
- not only in Multimodal or Multi-objective function, Diversity will elevates simple and general optimization problem.
- For highly "toxic" landscape (refer to my solution for Steiner Tree Problem) where most of the search space is invalid, AEGA is not that easy to be applicable?
## References
- [[CHC Adaptive Search]]
- [[Crowding]]
- [[Fitness Sharing]]
