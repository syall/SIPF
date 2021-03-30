#!/bin/bash

# QUEKO Directory
quekodir="QUEKO-benchmark/"

# QUEKO Circuit Size List
quekolist=($quekodir/BIGD $quekodir/BNTF $quekodir/BSS)

# QUEKO-benchmark/*/(*).qasm Capture Regex
qasmregex="$quekodir/.*/(.*)QBT_(.*).qasm"

# Ceck Abort Regex
abortregex="mapper:"

# Check Initial Mapping Exists Regex
checkregex="Initial mapping exists"

# Hide excessive trap messages
trap "" SIGABRT

# For every size category of QASM
for size in $quekolist; do
    # For every QASM in the size category
    for circuitfile in $size/*.qasm; do
        # Capture QASM name
        if [[ $circuitfile =~ $qasmregex ]]
        then
            circuitarch="${BASH_REMATCH[1]}"
            circuitname="${BASH_REMATCH[2]}"
        else
            echo "$circuitfile doesn't match"
        fi
        # Set Coupling graph based on QBT
        if [[ $circuitarch == "20" ]]
        then
            couplingfile="./couplings/tokyo.txt"
            couplingname=tokyo
        elif [[ $circuitarch == "16" ]]
        then
            couplingfile="./couplings/aspen4.txt"
            couplingname=aspen4
        else
            continue
        fi
        # Run mapper on QASM and Coupling Graph to Output File
        OUTPUT=$(./mapper $circuitfile $couplingfile 2>&1)
        if [[ $OUTPUT =~ $abortregex ]]
        then
            echo "$circuitarch"QBT_"$circuitname".qasm on $couplingname.txt assert doesn\'t match
        elif [[ ! $OUTPUT =~ $checkregex ]]
        then
            echo "$circuitarch"QBT_"$circuitname".qasm on $couplingname.txt mapping doesn\'t match
        fi
    done
done
