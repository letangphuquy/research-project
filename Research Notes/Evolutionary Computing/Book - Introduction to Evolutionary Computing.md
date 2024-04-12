#book
*to review knowledge, what you learnt from a book*
## Chapter 3
*(I've only read from 3.5 onwards)*
### Summary
- Operation of an EA
- Table 3.6: Natural evolution vs. Artificial evolution points out and remind us about important details of an EA.
### Details
- Operation can be roughly divided to 2 phases: 
	- exploration: search (in wide area) to step to **unvisited** regions
	- exploitation: search (in vicinity) to improve solutions in **known** region 
	- Problems: Ineffective search, premature convergence. Question: how to balance?
- Anytime behavior: Extending number of generations too much doesn't see any significant benefits as the trend graph takes shape of $1-\ln$ 
- No Free Lunch theorem: 
> no black-box algorithm outperform random walk when averaged over "all" problems
## Chapter 4
### Summary
- Real-valued representation embeds mathematical, statistical analysis in its representation and in its variant operators *beautifully*
### Details
- Genetic Programming (GP) does not need mutation? I doubt if any operators that have *phenotypic meaning*, for example based on syntax and meaning of the algorithm implemented can produce significantly better results. Check this [1]
### Actions
- Set $\texttt{POP\_SIZE}$ larger while $\texttt{NUM\_GEN}$ roughly the same (100,200 or maximum of 500)
### References
[1]: https://sci-hub.ee/10.1007/978-3-540-24854-5_70.
