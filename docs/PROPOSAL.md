# Proposal: Qubit Allocation as a combination Subgraph Isomorphism and Token Swapping Algorithm by Partitioning with a Failure Heuristic

Steven Yuan

Professor Zhang

16:198:516 Programming Languages and Compilers II

4/11/2021

## Introduction

Qubit allocation for quantum circuits on hardware has been an open research problem with the onset of normalizing quantum compiler methods. The problem is modeled as the qubit mapping problem, where the input is a quantum circuit with quantum logical gates and a coupling graph defining the quantum hardware qubit relationships, and the output is an initial mapping of logical qubits to physical qubits and a modified quantum circuit. Common assumptions of the qubit mapping problem are that coupling graphs are undirected between qubits, and SWAP gates are inserted to resolve logical and physical relations.

```text
Qubit Mapping Problem

Input: quantum circuit C, coupling graph G
Output: initial mapping of qubits M and modified quantum circuit C_f
```

The approach modifies existing research of framing the qubit allocation problem as a combination of subgraph isomorphism and token swapping \[2]. The proposed algorithm named subgraph isomorphism partitioning with failing heuristic (SIPF) partitions maximal isomorphic sublists instead of incrementally building them, navigating the decreasing potential partitions with a failure heuristic.

## The Algorithm

The SIPF algorithm uses WeightedDAF and TokenSwaps and are detailed in three algorithms:

- Algorithm 1: SIPF
- Algorithm 2: WeightedDAF
- Algorithm 3: TokenSwaps

### Algorithm 1: SIPF

SIPF is the framework that combines the subgraph isomorphism and token swapping algorithms. Starting with a left and right indices of 0 and the size of the input quantum circuit, WeightedDAF repeatedly partitions the circuit using the indices. Then, once the partitions have been made, each pair of partitions is joined through TokenSwaps. Finally, the algorithm returns the initial mapping of the first partition and the modified circuit.

```text
Algorithm 1: SIPF

Input: quantum circuit C, coupling graph G
Output: initial mapping of qubits M_i and modified quantum circuit C_m

L ← left index ← 0
R ← right index ← size(C)

P ← partitions ← []
while L ≠ R:
  (B ← upper bound, M ← mapping[logical → physical]) ← WeightedDAF(C, G, L, R)
  P ← [P, (L, B, M)]
  L ← B

p_prev ← previous partition ← head(P)
M_i ← mapping[logical → physical] M of p_prev

C_m ← [C_m, ∀g∈C L of p_prev ≤ index(g) < B of p_prev]
for p of tail(P):
  C_m ← [C_m, TokenSwaps(M of p_prev, M of p, G)]
  C_m ← [C_m, ∀g∈C L of p ≤ index(g) < B of p]
  p_prev ← p

Return M_i, C_m
```

### Algorithm 2: WeightedDAF

WeightedDAF is a modified version of the DAF algorithm \[1] that finds mappings of the subcircuit logical relation graph in the coupling graph.

```text
Algorithm 2: WeightedDAF

Input: quantum circuit C, coupling graph G, left index L, right index R
Output: upper bound B and maximal isomorphic sublist mapping M

C' ← ∀g∈C L ≤ index(g) < R

C'_live ← live ranges of logical qubits

C'_DAG ← Build a DAG from gates in C' based on logical relations
          - Use heuristic |candidate set(v)| / degree(v) to choose root vertex

CS ← Build a candidate space using C', C'_DAG, and G
      - Find physical qubit candidates for logical qubits using degrees
      - Filter CS with C'_DAG and C'_DAG^-1

H ← Failure heuristic ← (0, mapping[logical → set[logical]] ← EMPTY)

B ← upper bound B ← R

M ← mapping[logical → physical] ← EMPTY

H, M ← Backtrack recursively in the CS using C', C'_DAG, CS, M
        - Adaptive Match Ordering with Candidate-Size and Path-Size Heuristics
        - Pruning by Fail Sets
        - Keep track of maximum H and failing vertices for each partial mapping

if M is not a complete mapping:
  for q_l of logical qubits of H:
    for q_c of conflict qubits of q_l:
      I ← index of last gate between q_l and q_c using C'_live
      B ← minimum(B, I)
  B, M ← WeightedDAF(C, G, L, B)

Return B, M
```

Starting with the quantum circuit and left and right indices, a subcircuit is formed from the gates between those indices right exclusive. Then, the live range of the logical qubits in the subcircuit is calculated for later use in the failure heuristic.

A directed acyclic graph (DAG) of the subcircuit is built using the minimum heuristic for a vertex v `|candidate set (v)| / degree(v)` to choose a root, aiming to have few candidates and large number of edges to fail early. The DAG is used to build a candidate space, finding physical qubit candidates for the logical qubits in the DAG, then filtered overestimated candidates using the DAG and inverse DAG.

The candidate space is searched with recursive backtracking. The search uses adaptive matching ordering with minimizing candidate-size and path-size heuristics to search smaller branches first. Also, pruning is done by keeping track of fail sets to avoid redundant sibling searches for vertices that are not the source of the logical relation conflict. The failure heuristic is updated by recording the maximum size of a partial mapping and logical qubits that mapped to that size with the set of vertices that caused the logical qubit to fail.

Finally, if the search returns a complete mapping, then the upper bound and mapping are returned. Otherwise, a recursive WeightedDAF is called with an upper bound based on the failure heuristic, using the smallest index of the latest gates between the failed logical qubit and their conflict qubits, ensuring that the earliest logical qubit relation conflict is searched from its latest gate.

### Algorithm 3: TokenSwaps

TokenSwaps is an implementation of the colored token swapping problem, minimizing the swaps of colored tokens between adjacent colored vertices from an initial graph to a final graph.

```text
Algorithm 3: TokenSwaps

Input: previous mapping M_prev, current mapping M, coupling graph G
Output: List of swaps S from M_prev to M

q_prop ← set ← M_prev logical qubits - M logical qubits

C ← cost matrix calculating minimum distance between each physical qubit in G

S ← Search possible swaps recursively with M_prev, q_prop, C that decrease
     distance between:
     - logical qubit's physical qubit in M_prev to M
     - logical qubit's physical in q_prop to empty qubit

Return S
```

Starting from a previous mapping of logical to physical qubits and a current mapping, the goal is to find a set of minimum swaps related to size that change the previous mapping to the current mapping. First, the set of logical qubits that are in the previous mapping but not in the current mapping are kept in a set to propagate forward as a special case. Then, a cost matrix for minimum distances between vertices in the coupling graph G. Finally, the swaps are recursively searched with a heuristic that minimizes the distance between the logical qubits' physical qubits in previous and current mappings until the distance is 0, where logical qubits that are in both mappings are 0 once the swaps are reached and logical qubits to propagate are 0 once the logical qubit is mapped to an unused qubit in the current mapping.

## Pros

In the BMT algorithm \[2], the search had to be bound for maximal subgraph isomorphic sublists due to the combinatorial complexity of incrementally building the lists with each additional control relation. In contrast, the SIPF algorithm focuses on whether a set of gates (effectively control relations) has a complete mapping at one time, then using the failure heuristic to search the next subset. The combinatorial explosion of mappings in the BMT algorithm is avoided in SIPF by searching recursively from a possible maximal sublist with the failure heuristic which decreases the search space each iteration as opposed to increasing the search space.

In addition, the use of the DAF algorithm \[1] is a faster subgraph isomorphism algorithm that performs polynomial-time operations to prune the search space before searching and search-time filtering and ordering, which is an improvement over the recursive partitioning of control relations with no meaninful heuristics.

## Cons

For circuits that have small maximal partitions, the SIPF algorithm may be slower than the BMT algorithm \[2] in that the search may become equivalent to decrementing per gate, essentially the reverse of BMT with a more expensive algoritm DAF. The benefits of recursively incrementing is that the following search is fast although there are many choices, but SIPF would be slow in that a complete subgraph, graph, and candidate space is calculated per search.

Algorithms that frame qubit allocation as a combination of subgraph isomorphism and token swapping focus on optimizing the number of gates added with maximal partitions. However, the approach does not guarantee depth optimality, as inserting SWAP gates in partitions may not be the most efficient when taking parallelism into account. Even if the gates are optimal within partitions, the overall circuit may not be optimal.

## Hypothesis

Originally, my idea had been to blindly binary partition WeightedDAF using middle indices until a more optimal mapping could not be found. However, developing the failure heuristic which takes into account the latest gate of the earliest logical qubit relation conflicts provides an upper bound which always decreases the partition. My hypothesis is that this will decrease the number of searches to find a complete mapping by pruning off the gates that conflict later in the circuit.

Another hypothesis is that the SIPF algorithm will probably work fastest on circuits with large maximal partitions, as less partitions would need to be produced. Because of this, I believe that the solution will run faster for large coupling graphs with many adjacent physical qubits, as the chances of having a relation between physical qubits that fit the logical qubit requirements is more likely. On the other hand, circuits that require many swaps will probably be slower to allocate due to the number of searches increasing.

## References

- Myoungji Han, Hyunjoon Kim, Geonmo Gu, Kunsoo Park, and Wook-Shin Han. 2019. Efficient Subgraph Matching: Harmonizing Dynamic Programming, Adaptive Matching Order, and Failing Set Together. In *Proceedings of the 2019 International Conference on Management of Data* (*SIGMOD '19*). Association for Computing Machinery, New York, NY, USA, 1429–1446. DOI:https://doi.org/10.1145/3299869.3319880
- Marcos Yukio Siraichi, Vinícius Fernandes dos Santos, Caroline Collange, and Fernando Magno Quintão Pereira. 2019. Qubit allocation as a combination of subgraph isomorphism and token swapping. *Proc. ACM Program. Lang. 3*, OOPSLA, Article 120 (October 2019), 29 pages. DOI:https://doi.org/10.1145/3360546
