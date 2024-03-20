Time of writing: 20th March 24
Assignment table: [Google Sheet](https://docs.google.com/spreadsheets/u/1/d/1VgDHo3Nm8Cg9ozc1T_uLgJjLiadaPPsYM2FZvc_kQBk/edit#gid=1599746791)
This contains all related notes, concerning a research project
## Overview (- What)
- Problem? SPG, classical NP-hard problem, application in network design, routing algorithm?
- Approach? Use Genetic algorithm (GA) to heuristics search the solution. 64-bit integer components (`bitset`) for representation of genotype.
- Project Management? Clear goals + Regular meetup and communication + Direct feedback from Teacher
## Goal (- Which)
**Focused Task: Writing + Empirical Result**
*Other:*
- Reading & Taking Notes (for solid knowledge and sources for really innovative ideas)
## Problems
- Writing
	- Need to convey my idea explicitly and crystal clear for my teacher, who writes it.
	- Proof-read, writing skills, add explanation for design decisions made.
- Algorithm design: there're weak holes
	- ineffective mutation: bias newly added edges aren't useful. Most of new edges are invalid for example in case of hard complete graphs, and the performance are solely due to `make_span` (shortest paths connector)
	- too conservative survival in SGA (although it resembles CHC adaptive search). some individuals need to die off. Refers to Steady-State or AEGA (state of the art)
	- IGA: reset procedure is wasteful
	- `POP_SIZE, NUM_GEN` is appropriate? (too big)
		- Tinkering and tweaking parameters is painstaking process? 
- Implementation: Potential bug in reducing
## Ideas
- Addressing issues in Designing of Algorithm:
	- mutation: delete a random chain (with defined length) in phenotype, then `make_span`
	- read diversity techniques
	- IGA reset:
		- Fitness Sharing-like to suppress similar groups?
		- archive local optima(s) found and reuse them somewhere
	- old book (Modern Heuristic Search method 1996): refer Reactive Tabu Search to see any inspirations.
## Discussion
- Conflicts, Resolve and Suggestion:
	- Recently professor point out my error in conveying research idea to a writer.
	- For writing, knowledge need to be well-represented: transparent, concise and clear, so reviewers, editors and new-to-domain readers be more comfortable (thus the paper is accepted easier)
## Conclusion
**Current Deadline: 03/23/24**
Need to:
- Read "Preprocessing the Steiner Problem in Graphs", before 22/3. Take note
	- Plans for Reduction Test testing, debug, and implementation.
- Read Chapter 1,2 of "Introduction to Evolutionary Computing", in this week
- *Only after then*, try out new mutation and do mild benchmarks
Sync task list with Sheet



