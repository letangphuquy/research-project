*Compiled from .docx to .md via https://word2md.com/* (Math formula not included)
# Notes

# Fields of Interest:

- Evolutionary Algorithms
- Deep learning
- IC design
- Environmental problem, climate change, …
- Blockchain
- \* Chấm bài Scratch bằng cách so khớp hình ảnh

## How to find paper?

- Check journal quality via: [https://jcr.clarivate.com/jcr/home](https://jcr.clarivate.com/jcr/home) or [https://www.scopus.com/home.uri](https://www.scopus.com/home.uri) (impact factor, SCIE?, year, ISSN, …)

_Note\*:_

Format, presentation issue & special symbols in this file: h1, h2, local link, website link, name of paper, #skill, "Ref", …

# To-read

- [https://www.sciencedirect.com/science/article/pii/0022000088900463](https://www.sciencedirect.com/science/article/pii/0022000088900463) (How easy is Local Search)
- [https://books.google.com.vn/books?hl=en&lr=&id=DyzhHMOhI-8C&oi=fnd&pg=PP1&dq=local+search&ots=kRhYizdHnZ&sig=UGk3AIPeSEwwfWVugRsdYg9-tjQ&redir\_esc=y#v=onepage&q=local%20search&f=false](https://books.google.com.vn/books?hl=en&lr=&id=DyzhHMOhI-8C&oi=fnd&pg=PP1&dq=local+search&ots=kRhYizdHnZ&sig=UGk3AIPeSEwwfWVugRsdYg9-tjQ&redir_esc=y#v=onepage&q=local%20search&f=false) (Stochastic Local Search: Foundations and Applications)
- [https://link.springer.com/book/10.1007/978-3-540-35854-1](https://link.springer.com/book/10.1007/978-3-540-35854-1) (Theoretical Aspects of Local Search)
- [https://sci-hub.se/10.1016/j.asr.2019.01.043](https://sci-hub.se/10.1016/j.asr.2019.01.043) (A Multi-objective Binary-encoding Differential Evolution Algorithm for Proactive Scheduling of Agile Earth Observation Satellites)
- [https://books.google.com.vn/books?hl=en&lr=&id=6CzoBwAAQBAJ&oi=fnd&pg=PR9&dq=local+search&ots=VJ\_hzBZRQh&sig=tXdhm\_XaDQt8yCy4YjHiODhAsJg&redir\_esc=y#v=onepage&q=local%20search&f=false](https://books.google.com.vn/books?hl=en&lr=&id=6CzoBwAAQBAJ&oi=fnd&pg=PR9&dq=local+search&ots=VJ_hzBZRQh&sig=tXdhm_XaDQt8yCy4YjHiODhAsJg&redir_esc=y#v=onepage&q=local%20search&f=false) (Meta-Heuristics: Advances and Trends in Local Search Paradigms for Optimization)
- [https://www.sciencedirect.com/science/article/pii/S0898122108005221](https://www.sciencedirect.com/science/article/pii/S0898122108005221) (A hybrid quantum chaotic swarm evolutionary algorithm for DNA encoding)
- [file:///E:/Computer%20Science/Research/Genetic%20and%20Evolutionary%20Computation/bartz-beielstein2014.pdf](/E:/Computer%20Science/Research/Genetic%20and%20Evolutionary%20Computation/bartz-beielstein2014.pdf) (Evolutionary Algorithms)
- [https://link.springer.com/chapter/10.1007/978-3-319-91086-4\_5](https://link.springer.com/chapter/10.1007/978-3-319-91086-4_5)
- Almost forgotten Local Search series: [https://sci-hub.se/10.1002/cplx.20300](https://sci-hub.se/10.1002/cplx.20300) (A Genetic Algorithm with Local Search Strategy for Improved Detection of Community Structure), [https://sci-hub.se/10.1016/s0045-7949(03)00183-4](https://sci-hub.se/10.1016/s0045-7949(03)00183-4) (Convergence enhanced genetic algorithm with successive zooming method for solving continuous optimization problems)

# Reading

Plans:

+ alongside with mentor's questions, I do my own research to grasp a solid knowledge of the field.

+ do chap.2 exercise [the book]

This week's concern: Steiner Tree Problem and prototyping algorithms to evaluate (it's hard to compete with giants right from the beginning, should 'stand on their shoulders' instead)

Past discussion topics:

- Overview EA
- Encoding (from phenotype to genotype) and its impacts
- applications of local search on different phases of EAs, its effect, why? When? And how?

## Introduction to Evolutionary Algorithms

Link: [E:\Computer Science\Research\Genetic and Evolutionary Computation\81369\_S. Tamilselvi\_Introduction to Evolutionary Algorithms.pdf](/E:/Computer%20Science/Research/Genetic%20and%20Evolutionary%20Computation/81369_S.%20Tamilselvi_Introduction%20to%20Evolutionary%20Algorithms.pdf)

Sub-link: [https://books.google.com.vn/books?hl=en&lr=&id=rHQf\_2Dx2ucC&oi=fnd&pg=PR8&ots=xNGc0txlzJ&sig=pNUhMEoZz7SW-JFvc-ckusM2G44&redir\_esc=y#v=onepage&q&f=false](https://books.google.com.vn/books?hl=en&lr=&id=rHQf_2Dx2ucC&oi=fnd&pg=PR8&ots=xNGc0txlzJ&sig=pNUhMEoZz7SW-JFvc-ckusM2G44&redir_esc=y#v=onepage&q&f=false)

Chapter 1 & 2 provided a big picture about EA. Chapter 3 dives in problems that are certain to appear when dealing with EAs, and need to be resolved before applying in real-world problem.

When we talk about coding, we also talk about variant operators. They must follow 2 guidelines:

- Preservation of Statistics
- Diversity of Population

**Section: Heuristics & EC for Steiner**

## Approximation Algorithms: Introduction to Network Design (doc)

Link: [https://spectra.mathpix.com/article/2022.03.00852/approximation-algorithms-introduction-to-network-design#fn7](https://spectra.mathpix.com/article/2022.03.00852/approximation-algorithms-introduction-to-network-design#fn7)

_Note: this is lecture-styled_

- First point: Problems on directed graphs are generally harder than on undirected ones.
- Simple algorithms, but hard proofs (math inequality). APX-hard: Find the ratio compared to the optimal solution.

Algo(s): SteinerMST 2-OPT, Greedy (needs to calculate all-pair shortest path),

## A fast algorithm for Steiner trees

Link: [https://sci-hub.se/10.1007/bf00288961](https://sci-hub.se/10.1007/bf00288961)

Better illustrate the SteinerMST (because it's new) with figures. Easier to understand than wall-of-text proof and page-long mathematical formulation. #skill

Algo outline: Given G(V,E) and set S of terminals

- Compute distance graph .
- For complete subgraph of D containing only terminals, find a MST named T.
- Replace edges in T by actual shortest paths in G, receive graph H
- T := MST(H). Then cut all non-terminal leaves

Core of the idea: attempt to "coerce" Steiner problem to MST problem – turned out to be a good approximation (50% of the optimal, blazingly fast)

**Section: Exact solver for Steiner**

## The Steiner problem in graphs

Cite: Dreyfus, S.E.; Wagner, R.A. (1971). "The Steiner problem in graphs"

Link: [https://web.vu.lt/mif/s.jukna/tropical/Dreyfus-Wagner.pdf](https://web.vu.lt/mif/s.jukna/tropical/Dreyfus-Wagner.pdf)

Algorithm outline:

- Observation: By connecting a new node q to a set of "already-connected" nodes, the "old set" can be splitted to 2 parts.
- DP bitmask 😊
- : with being an intmediary node, splitting set D into E,F
- = Steiner path connects m to D

## Dynamic Programming for Minimum Steiner Trees

Link: [https://sci-hub.se/10.1007/s00224-007-1324-4](https://sci-hub.se/10.1007/s00224-007-1324-4)

Improve Dreyfus and Wagner's work by introducing flexible base . Complicated.

## Implications, conflicts, and reductions for Steiner trees (Ref)

Link: [E:\Computer Science\Research\Genetic and Evolutionary Computation\s10107-021-01757-5.pdf](/E:/Computer%20Science/Research/Genetic%20and%20Evolutionary%20Computation/s10107-021-01757-5.pdf)

By first defining a stronger bottleneck concept, through rigorous mathematical proof.

Stronger inequality  More reduction  Increase performance of the exact solver for SPG.

Branch-and-cut framework. Steiner problem reduced to MIP (Mixed Integer Programming) solver. SCIP-JACK. Source code: [https://github.com/dRehfeldt](https://github.com/dRehfeldt)

Huge research projects with many new proposals (also great empirical result on the DIMACS 2014 and PACE 2018 challenges). Cited greate sources.

\*Starred

## Genetic Algorithm with adaptive elitist-population strategies for multimodal function optimization (Ref)

Link: [E:\Computer Science\Research\Genetic and Evolutionary Computation\Adaptive\_population\_-App\_Soft\_Comp\_2011.pdf](/E:/Computer%20Science/Research/Genetic%20and%20Evolutionary%20Computation/Adaptive_population_-App_Soft_Comp_2011.pdf)

Review related works:

- Elitism (tinh tuyển): premature convergence, not work as expected (as a global searcher)
- Parallel evolving, subpopulation, niching, sharing function. They cited a lot of ref. here.

Their algorithm:

- Increase population diversity by considering both distances and relative ascending order ( **principle of dissimilarity** )
- Elitist crossover operator (adopt the principle introduced). Mutation crossover operator. Population control (in the role of selection)

Experimental methodology (#skill):

- Outstanding performance on the test suite including classic functions.
- Comparisions using test suite consisting functions with different characteristics.
- Compare between different algorithms, then compare between different parameter settings for a single algorithm.

## Wireless Sensor Network Optimization: Multi-Objective Paradigm (Ref)

Link: [https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4541950/](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4541950/)

- Problems of MOO in WSN. Classification of optimization objectives.
- Reviews of different solutions and approaches

## On Representation and Genetic Operators in Evolutionary Algorithms

Link:

Sub-links:

- [https://www.obitko.com/tutorials/genetic-algorithms/encoding.php](https://www.obitko.com/tutorials/genetic-algorithms/encoding.php)
- [https://scholar.google.com.vn/scholar?q=encoding+in+evolutionary+algorithm&hl=en&as\_sdt=0&as\_vis=1&oi=scholart](https://scholar.google.com.vn/scholar?q=encoding+in+evolutionary+algorithm&hl=en&as_sdt=0&as_vis=1&oi=scholart)

# Summary

## Reliable Adaptive Data Aggregation Route Strategy for a Trade-off between Energy and Lifetime in WSNs

Link: [https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4208209/](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4208209/)

Major contributions:

1. Discrete particle swarm optimization. Prufer encoding scheme. Uniform crossover. Random two-point exchange mutation
2. Phenotype sharing function
3. Introduce penalty function to convert constrained optimization problem to non-constrained one

Fitness value = (1 + D(i)) \* (1 + N(i)) (dominance measure & neighbor density)

Fourth penalty function: (1 + ln(1 + pf)) \* F(i), pf is E(G') / (E(G\*)): "Give chance to these particles that violate the constraints to have their optimal information inherited in later iterations". A great diversity promotion technique!

## Christofides Algorithm

## Lin-Kernighan heuristic

Extends 2-opt to 3-opt, k-opt. Search by picking a random group of k disjoint edges (2\*k nodes), then detach and re-connect them in a different way to form a better tour.

## An edge elimination test for the steiner problem in graphs

Link: [https://sci-hub.se/10.1016/0167-6377(89)90005-9](https://sci-hub.se/10.1016/0167-6377(89)90005-9) (local: [E:\Computer Science\Research\Genetic and Evolutionary Computation\duin1989.pdf](/E:/Computer%20Science/Research/Genetic%20and%20Evolutionary%20Computation/duin1989.pdf))

Edge reduction: từ 2 bài test MST và Least Cost, phát triển thành SD (Special Distance)

Induced subgraph, distance graph (metric). S\_ij là khoảng cách bottleneck min trên mọi đường đi đặc biệt (chỉ đi qua các đỉnh đặc biệt) trên D(G).

## Node-depth Encoding for Evolutionary Algorithms

Link: [https://sci-hub.se/10.1007/978-3-540-24854-5\_70](https://sci-hub.se/10.1007/978-3-540-24854-5_70) (local: [Genetic and Evolutionary Computation\delbem2004\_Node-Depth Encoding for Evolutionary Algorithms Applied to Network Design.pdf](/E:/Computer%20Science/Research/Genetic%20and%20Evolutionary%20Computation/delbem2004_Node-Depth%20Encoding%20for%20Evolutionary%20Algorithms%20Applied%20to%20Network%20Design.pdf))

- Tổng quát: Encoding phụ thuộc vào từng bài toán. Performance của EA phụ thuộc vào cách ta encode. Vì cách encode quyết định lượng thông tin truyền tải qua thế hệ sau bảo toàn lại được bao nhiêu, các thao tác crossover, v.v.. có đủ hiệu quả không. Điều đó dẫn tới việc hội tụ có nhanh không, có dễ bị rơi vào local optima …

- Cách làm được đề xuất hiệu quả hơn hẳn so với GAPE (Prufer Encoding) !!

- Cách làm: Ta chỉ quan tâm các đỉnh lá. Sắp xếp các đường đi từ root tới các lá này theo thứ tự từ điển, và chỉ lưu lại phần khác biệt so với đường đi nằm liền trước. Danh sách nút ta thu được chứa đúng n nút! Đồng thời, cũng lưu lại độ sâu của chúng.

- Giải thích: LCA của 2 đỉnh trên cây là LCP của hai xâu biểu diễn đường đi !!

- Thao tác đột biến: (a) Cắt nhánh cây này, ghép vào cây khác (b) như a, nhưng đồng thời cũng quay gốc cây con bị cắt đi.

## Improving genetic algorithms' performance by local search for continuous function optimization

Link: [https://sci-hub.se/10.1016/j.amc.2007.05.068](https://sci-hub.se/10.1016/j.amc.2007.05.068)

Một cách áp dụng local search đơn giản, áp dụng trên tập nghiệm tạo bởi SGA !!, nhưng kết quả khá tốt (reach được optimal trên những function có graph tương đối đơn giản)

Có dẫn một số referrence khá thú vị.

## The AI gambit: leveraging artificial intelligence to combat climate change—opportunities, challenges, and recommendations
Josh Cowls1,2 · Andreas Tsamados1 · Mariarosaria Taddeo1,2 · Luciano Floridi1,2

AI: giúp hiểu rõ hơn tình trạng của môi trường, giúp đưa ra các giải pháp (theo kiểu bài toán tối ưu)

Tuy nhiên cốt lõi vẫn nằm ở ý thức từ mỗi cá thể.

Article dẫn rất nhiều nguồn.

AI carbon footprint. Bias trong training data (thiếu tổng quát). Privacy concerns.