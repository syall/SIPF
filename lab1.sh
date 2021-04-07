#!/bin/bash

# Output Directory
output=lab1/

# circuit/*/(*).qasm Capture Regex
qasmregex="./circuits/.*/(.*).qasm"

# couplings/(*).txt Capture Regex
txtregex="./couplings/(.*).txt"

# Check Initial Mapping Exists Regex
checkregex="Initial mapping exists"

# Hide excessive trap messages
trap "" SIGABRT

# Make Output Directory
mkdir -p $output

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
            OUTPUT=$(./mapper $circuitfile $couplingfile 2>&1 | tee $output/$circuitname--$couplingname.txt)
            if [[ ! $OUTPUT =~ $checkregex ]]
            then
                echo $circuitfile on $couplingfile mapping doesn\'t match
            fi
        done
    done
done
