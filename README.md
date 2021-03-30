# CS516-Project-1: Qubit Mapping Problem

Steven Yuan

Professor Zhang

16:198:516 Programming Languages and Compilers II

5/3/2021

## Lab 1: Find a Complete Initial Mapping for a Circuit

Due: 3/31/2021

The Qubit Mapping Problem's complexity comes from logical qubit relations from two-qubit gates not matching coupling graph constraints, so SWAP gates need to be inserted to fix incompatible relations.

However, circuits could have a complete mapping of logical to physical qubits if the coupling graph is able to satisfy all of the logical qubit relations.

Lab 1 is focused on finding whether a complete initial mapping of a circuit to a coupling graph exists and outputting an initial mapping if a mapping does exist.

### Implementation

Lab 1 uses a subgraph isomorphism algorithm based on the DAF algorithm by [Han, et al. SIGMOD'19](https://dl.acm.org/doi/10.1145/3299869.3319880) (see `DAF.md` for brief notes from a conference recording), but without candidate space refinement using DAG-graph dynamic programming, adaptive matching order using candidate-size and path-size order heuristics, and pruning by failing sets.

- Candidate space refinement using DAG-graph dynamic programming had been attempted but was removed due to solutions being pruned, probably due to incorrect implementation.
- Adaptive matching order using candidate-size and path-size order heuristics were not attempted as the search space of circuits to coupling graphs are extremely small relative to the datasets in the paper. The complexity of implementing the feature outweighs the performance benefits.
- Pruning by failing sets were not attempted but would be the most useful feature to implement. Coupling graphs typically have recurring patterns, so pruning entire classes of partial mappings to coupling graph patterns would increase performance tremendously.

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

To pull in the `QUEKO-benchmark` repository, run `git pull  on recurse-submodules`.

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

If a logical qubit is mapped to a physical qubit `-1`, it means that the logical qubit is not used in the circuit.

### Evaluation

The `lab1.sh` script runs `mapper` on every circuit and coupling graph combination between `circuits/` and `couplings/` from the Sakai assignment files.

- For each combination, a line will denote which combination is running, and if there isn't a mapping, an error message will be printed.
- The program output for each combination is outputted to files in `lab1/` that are named with the format `circuit on coupling.txt`.

`mapper` is able to find a mapping for:

- vbe_adder_3_after_heavy on tokyo
- 3_17_13 on qx2
- 3_17_13 on tokyo
- ex-1_166 on qx2
- ex-1_166 on tokyo
- ham3_102 on qx2
- ham3_102 on tokyo
- or on qx2
- or on tokyo
- adder on 2x2
- adder on 2x3
- adder on 2x4
- adder on aspen4
- adder on melbourne
- adder on tokyo
- qft_4 on tokyo
- rd32-v0_66 on tokyo
- 4gt13_92 on qx2
- 4gt13_92 on tokyo
- 4mod5-v1_22 on tokyo
- mod5mils_65 on tokyo
- qaoa5 on 2x3
- qaoa5 on 2x4
- qaoa5 on aspen4
- qaoa5 on melbourne
- qaoa5 on qx2
- qaoa5 on tokyo
- barenco_tof_4_after_heavy on tokyo
- tof_4_after_heavy on tokyo
- barenco_tof_5_after_heavy on tokyo
- tof_5_after_heavy on tokyo
- 4gt11_84 on qx2
- 4gt11_84 on tokyo
- 4mod5-v0_19 on tokyo
- 4mod5-v0_20 on tokyo
- 4mod5-v1_24 on tokyo
- miller_11 on qx2
- miller_11 on tokyo
- mod5d1_63 on tokyo
- rd32-v1_68 on tokyo

The `lab1-queko.sh` script runs `mapper` on every circuit in the `QUEKO-benchmark` repository that has a coupling graph available from the Sakai assignment files (`aspen4.txt` for `16QBT` prefix , `tokyo.txt` for `20QBT` prefix).

- Make sure to have pulled in the `QUEKO-benchmark` repository with `git pull  on recurse-submodules`.
- For each combination, a line will denote which combination is running, and if there isn't a mapping, an error message will be printed.
- The program output for each combination is outputted to files in `lab1/` that are named with the format `circuit on coupling.txt`.

`mapper` is able to find a mapping for every combination in the `QUEKO-benchmark` repository.
