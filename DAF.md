# DAF Algorithm

[Presentation: Efficient Subgraph Matching: Harmonizing Dynamic Programming, Adaptive Matching Order, and Failing Set Together](https://av.tib.eu/media/42973)

Input: Query Graph (to embed) q, Data Graph G

Output: All embeddings of q in G

qDAG (query graph DAG) = build DAG from q

- Choose a root with:
  - Large Degree (many edges)
  - Few candidates in data graph (not many choices)
- BFS to vertices

CS (candidate space) = build CS from q, qDAG, G

- Build CS using DAG-Graph Dynamic Programming
  - Given query DAG q' from q
  - v in C'(u) iff v IN C(u) and EXISTS vc IN C'(uc) adjacent to v for every child uc of u
  - Compute C'(u) using bottom up dynamic programming
  - Build a compact CS
    - From initial CS, repeat DAG-Graph DP with q' = qDAG and q' = Reversed qDAG alternately
    - Ideally, repeat until no change occurs
    - Empirically, 3 steps are usually enough
- Complete Search Space
- CS consists of candidate set & edges
  - Candidate Set C(u) FORALL u IN V(q), where C(u) LESSTHANORINCLUDES Cini(u)
  - Edge between v IN C(u) and v' IN C(u') iff (u, u') in E(q) and (v, v') in E(G)

M (mapping) = EMPTY

Backtrack(q, qDAG, CS, M)

- Find all embeddings of q in CS using
  - Adaptive Matching Order
    - Suppose there is a partial mapping M
    - If there are multiple extendable vertices, which should be expanded first?
    - Heuristics
      - Candidate Size Order: Select the vertex with the smallest Candidate Set Size
      - Path Size Order: Select the vertex such that the weight is minimum (not sure
        how to do this)
  - Pruning by Failing Sets
    - Suppose a subtree has failed to match in all possibilities
    - No matter how you change a mapping of one vertex, the mapping always fails
    - Two ingredients
      - Set of query vertices related to the failure
      - Subset M' as a partial embedding
    - M' cannot lead to a full embedding
    - F is a failing set
    - If u is not in the failing set of a child node M, then all sibling nodes of M
      are redundant
- DAG Ordering
  - Edges should be checked first, as the edges are the source of pruning
  - An unmapped vertex is extendable (searchable) if all parents are matched in M
  - ALWAYS choose extendable vertices
  - Extendable Candidates for the Extendable Vertices are the intersection of the
    parents' mapped vertices' neighbors in the candidate set
- Framework
  - Select extendable candidate u
  - Map u to each unvisited extendable candidate to produce a partial embedding
  - Recurse
