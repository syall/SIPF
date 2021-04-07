# Proposal: Modified Subgraph Isomorphism and Token Swapping Algorithm with DAF

Steven Yuan

Professor Zhang

16:198:516 Programming Languages and Compilers II

4/11/2021

## Overview

Use "Qubit Allocation as a Combination of Subgraph Isomorphism and Token Swapping" by Siraichi et al. OOPSLA 2019 as framework

- For Subgraph Isomorphism, use DAF Algorithm in "Efficient Subgraph Matching: Harmonizing Dynamic Programming, Adaptive Matching Order, and Failing Set Together" by Han et al. SIGMOD'19
- For Token Swapping, probably will use the token swapping in Siraichi et al. OOPSLA 2019

## What is DAF?

- Subgraph Isomorphism Algorithm
- Query graph q: Graph to Embed
- Data graph G: Graph to Embed to
- Query Graph DAG qDAG: DAG of q
- Candidate Space CS: Complete Search Space based on qDAG, q, and G
  - Filters with qDAG and qDAG^-1 even before search to remove false positives
- M: Mapping of Vertices q to G
- Searched with Backtracking
  - Adaptive Matching Order based on Candidate Set Size and Path Set Size
  - Pruning by Failed Sets (IMPORTANT)

```text
Input: query graph q, data graph G
Output: all mappings of q in G

qDAG <- Build a DAG of q using q and G, using the vertex v with the minimum
        |candidate set(v)|/degree(v) heuristic as the root

CS   <- Build a Candidate Space using q, qDAG, and G, consisting of a set of
        candidate vertices u ∈ G for each vertex v ∈ q, and edges between those
        candidate vertices
        - Filter Candidate Space with qDAG and qDAG^-1 to catch false positives

M    <- Empty mapping ∀v ∈ q

Backtrack recursively in the CS using q, qDAG, CS, M
- Adaptive Match Ordering with Candidate-Size and Path-Size Heuristics
- Pruning by Fail Sets
```

## Proposed Modified Subgraph Isomorphism and Token Swapping Algorithm with DAF

- Use DAF for Subgraph Isomorphism
- Use Multiple Binary Searches to find Maximal Partition Mappings
  - Run DAF at every "middle" search
  - Stop when "left" and "right" indices are crossed
- Reduce Maximal Partitions with Token Swapping to insert SWAP Gates
- Return First Mapping, Complete Circuit

```text
Input: Circuit C, Coupling Graph G

Mapping M <- EMPTY

M <- EMPTY
P <- [] # Partitions
L <- 0
R <- Size of C
While all Gates in C are not Partitioned (L is not R)
  M <- Binary Search Maximal Mapping M from [L, R]
       - DAF(C, G, M, L, R) at every "middle" search
  Add M to P
  L <- Index of Gate after M
  M <- EMPTY

Complete Circuit CC <- Reduce P with Token Swapping to insert SWAP Gates

Return M, CC
```

## Benefits and Concerns

Benefits:

- Removes limitation of Maximal Partition Size
- Uses DAF which is fast

Concerns:

- Why not incrementally build partitions like in Siraichi et al. 2019?
  - DAF filters out the candidate space which drastically increases speed
  - If done incrementally one gate at a time, DAF would need to run every time anyways to account for filtered candidates
- Is optimality guaranteed? Guaranteed within partitions
