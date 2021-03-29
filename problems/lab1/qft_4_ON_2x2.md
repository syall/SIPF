# qft_4 on 2x2

## qft_4

```text
OPENQASM 2.0;
include "qelib1.inc";
qreg q[4];
creg c[4];
cx q[0],q[1];
cx q[0],q[2];
cx q[0],q[3];
cx q[1],q[2];
cx q[1],q[3];
cx q[2],q[3];

0-c-c-c-------
1-t-----c-c---
2---t---t---c-
3-----t---t-t-

Logical Graph:
0: 1 2 3
1: 0 2 3
2: 0 1 3
3: 0 1 2

DAG with root 0:
0: 1 2 3
1: 2 3
2: 3
3:
```

## 2x2

```text
4 4
0 1
0 2
1 3
2 3

0-2
| |
1-3
```

## Example

- Get to Gate cx L0, L1
  - Arbitrarily assign L0 to P0
    - Arbitrary assign L1 to P1
    - Get to Gate cx L0, L2
      - L0 already assigned to P0
      - Arbitrarily assign L2 to P2
      - Get to Gate cx L0, L3
        - L0 already assigned to P0
        - No possible assignment for L3, backtrack
      - L2 to P2 assignment invalid
      - No possible assignment for L2, backtrack
    - L1 to P1 assignment invalid
    - Arbitrarily assign L1 to P2
    - Get to Gate cx L0, L2
      - L0 already assigned to P0
      - Arbitrarily assign L2 to P1
      - Get to Gate cx L0, L3
        - L0 already assigned to P0
        - No possible assignment for L3, backtrack
      - L2 to P1 assignment invalid
      - No possible assignment for L2, backtrack
    - L1 to P2 assignment invalid
    - No possible assignment for L1, backtrack
  - L0 to P0 assignment invalid
  - Arbitrarily assign L0 to P1
    - ... Search more invalid assignments
- No possible mapping
