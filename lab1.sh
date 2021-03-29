#!/bin/bash
for circuitfile in ./circuits/small/*.qasm; do
    for couplingfile in ./couplings/*.txt; do
        echo Testing $circuitfile on $couplingfile
        time ./mapper $circuitfile $couplingfile
        echo
    done
done
