#!/bin/bash

# Output Directory
results=lab1/

# circuit/*/(*).qasm Capture Regex
qasmregex="./circuits/.*/(.*).qasm"

# couplings/(*).txt Capture Regex
txtregex="./couplings/(.*).txt"

# Hide excessive trap messages
trap "" SIGABRT

echo "Lab 1 Start ============================"

# Make Output Directory
mkdir -p $results

# For every size category of QASM
for size in ./circuits/*; do
    # For every QASM in the size category
    for circuitfile in $size/*.qasm; do
        # For every Coupling Graph
        for couplingfile in ./couplings/*.txt; do
            # Capture QASM name
            if [[ $circuitfile =~ $qasmregex ]]
            then
                circuitname="${BASH_REMATCH[1]}"
            else
                echo "$circuitfile doesn't match"
            fi
            # Capture Coupling Graph name
            if [[ $couplingfile =~ $txtregex ]]
            then
                couplingname="${BASH_REMATCH[1]}"
            else
                echo "$couplingname doesn't match"
            fi
            # Output to stdout
            echo "Testing $circuitname on $couplingname"
            # Run mapper on QASM and Coupling Graph to Output File
            ./mapper $circuitfile $couplingfile > \
                     $"$results/$circuitname--$couplingname.txt" \
                     2>&1
        done
    done
done

echo "Lab 1 Done ============================="
