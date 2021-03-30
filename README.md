# CS516-Project-1: Qubit Mapping Problem

Steven Yuan

Professor Zhang

16:198:516 Programming Languages and Compilers II

5/3/2021

## Lab 1: Find an Complete Initial Mapping for a Circuit

### Due: 3/31/2021

The Qubit Mapping Problem's complexity comes from logical qubit relations from two-qubit gates not matching coupling graph constraints, so SWAP gates need to be inserted to fix incompatible relations.

However, circuits could have a complete mapping of logical to physical qubits if the coupling graph is able to satisfy all of the logical qubit relations.

Lab 1 is focused on finding whether a complete initial mapping of a circuit to a coupling graph exists and outputting an initial mapping.

### Implementation

Lab 1 uses a subgraph isomorphism algorithm based on the DAF algorithm by [Han, et al. SIGMOD'19](https://dl.acm.org/doi/10.1145/3299869.3319880) (see `DAF.md` for brief notes from a conference presentation), but without candidate space refinement using DAG-graph dynamic programming, adaptive matching order using candidate-size and path-size order heuristics, and pruning by failing sets.

- Candidate space refinement using DAG-graph dynamic programming had been attempted but was removed due to solutions being pruned, probably due to incorrect implementation.
- Adaptive matching order using candidate-size and path-size order heuristics were not attempted as the search space of circuits to coupling graphs are extremely small relative to the datasets in the paper. The complexity of implementing the feature outweighs the performance benefits.
- Pruning by failing sets were not attempted but would be the most useful feature to implement. Coupling graphs typically have recurring patterns, so pruning off entire classes of partial mappings to coupling graph patterns would increase performance tremendously.

The general framework of the algorithm is as follows:

```text
Input: query graph q, data graph G
Output: A mapping M if ∃M of q in G

qDAG <- Build a DAG of q using q and G, using the vertex v with the minimum
        |candidate set(v)|/degree(v) heuristic as the root

CS   <- Build a Candidate Space using q, qDAG, and G, consisting of a set of
        candidate vertices u ∈ G for each vertex v ∈ q, and edges between those
        candidate vertices

M    <- Empty mapping ∀v ∈ q

M    <- Backtrack recursively in the CS using q, qDAG, CS, M
```

### Usage

Run `make` (or `make debug` for debug symbols) to produce the `mapper` executable.

To use the `mapper` executable, two arguments are required: `mapper circuit_file coupling_file`

- `circuit_file`: Path to a OpenQASM Circuit `.qasm` file (examples found in the `circuits/*/` directories)
- `coupling_file`: Path to a Coupling Graph `.txt` file (examples found in the `couplings/` directory)

If an initial mapping cannot exist since the number of logical qubits is larger than the number of physical qubits, an assertion message is outputted.

If an initial mapping does not exist, then `No complete initial mapping exists` will be outputted.

If an initial mapping does exist, then `Initial mapping exists` and the logical qubit to physical qubit mappings (`logical_qubit physical_qubit`) will be outputted, such as:

```text
$ mapper circuits/small/4gt13_92.qasm couplings/qx2.txt
Initial mapping exists
0 0
1 1
2 3
3 4
4 2
```

For convenience, a bash script `lab1.sh` runs `mapper` on every circuit and coupling graph combination between `circuits/` and `couplings/`, outputting files to `lab1/` named `circuit--coupling.txt`.
