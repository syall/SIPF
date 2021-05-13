# SIPF

Steven Yuan

Professor Zhang

16:198:516 Programming Languages and Compilers II

5/12/2021

## Overview

SIPF is an algorithm that frames Qubit Allocation as a combination of the Subgraph Isomorphism and Token Swapping Algorithms by Partitioning with a Failure Heuristic.

## Documents

- [Report](./docs/REPORT.md)
- [Report (pdf)](./docs/REPORT.pdf)
- [Presentation](https://youtu.be/-6RpNGGDgp4)
- [Proposal](./docs/PROPOSAL.md)
- [Lab 1](./docs/LAB1.md)

## Usage

### Dependencies

At least C++11.

### Building

- `make`: compile the `mapper` binary
- `make debug`: compile the `mapper` binary with debug flags
- `make clean`: remove compiled files

### Input

`mapper path/to/circuit.qasm path/to/coupling.txt [-optimal]`

- `path/to/circuit.qasm`: QASM circuit file (see `qelib1.inc` for available quantum gates)
- `path/to/coupling.txt`: coupling graph file (see `couplings/` for example architectures)
- `-optimal`: optimal allocator (optional)

Examples:

- `mapper circuits/3qubits/or.qasm couplings/qx2.txt`
- `mapper circuits/5qubits/4gt13_92.qasm couplings/2x3.txt -optimal`

### Output

Modified Transformed Circuit with metadata and comments to standard output.

Example:

```text
$ mapper circuits/3qubits/or.qasm couplings/qx2.txt
//Number of Swaps: 0
//Number of Mappings: 1
//Depth: 8
//Number of Gates: 17
OPENQASM 2.0;
include "qelib1.inc";
qreg q[5];
//Location of qubits: 0,1,2
x q[0];
tdg q[1];
h q[2];
cx q[2], q[1];
tdg q[0];
t q[1];
tdg q[2];
cx q[0], q[1];
sdg q[2];
cx q[2], q[0];
t q[1];
t q[0];
cx q[2], q[1];
tdg q[1];
cx q[2], q[0];
cx q[0], q[1];
h q[2];
```

## Benchmarks

### Set up Submodules

Run `git submodule update --init --recursive` to pull the git submodules:

- `enfield`
- `jsoncpp`
- `QUEKO-benchmark`

### Generic Benchmarks

#### Running

Modify and run `feedback.sh` with the variables:

- `output`: define output directory
- Line 48: toggle the `-optimal` flag

#### Evaluating

In the `output/` directory's `record.txt` file, remove any tests that were `Assertion failed` or `No Mappings Possible`.

Run `node report.js path/to/record.txt` to get the columns and values: circuit and coupling, depth, number of gates, real time, and user + sys time.

### Enfield Compiler

#### Compilation

Follow the instructions in the `README.md` file of the `enfield` repository (path to `jsoncpp` is path to the git submodule `jsoncpp`).

#### Running Enfield

Modify and run `./feedback-enfield.sh` with the variables:

- `output`: define output directory
- Line 48: modify input arguments to the `efd` program
  - `efd` is in `enfield/build/tools/efd`
  - `-i`: input QASM file
  - `-alloc`: define qubit allocator
  - `-stats`: show statistics
  - `-arch-file`: input coupling graph file (in JSON)

#### Evaluating Enfield

In the `output/` directory's `record.txt` file, remove any tests that were `Killed` or `ERROR`.

Run `node report-enfield.js path/to/record.txt` to get the columns and values: circuit and coupling, depth, number of gates, real time, and user + sys time.

### Lab 1 Benchmarks

Checkout the `lab1-review` branch.

Rebuild the `mapper` binary.

Run:

- `lab1.sh` for generic initial mappings
- `lab1-queko.sh` for `QUEKO-benchmark` initial mappings
