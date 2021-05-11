#!/bin/bash

# Output Directory
output=feedback-enfield/

# feedback_circuits/circuit/*/(*).qasm Capture Regex
qasmregex="./feedback_circuits/circuits/.*/(.*).qasm"

# feedback_circuits/couplings/(*).txt Capture Regex
txtregex="./feedback_circuits/couplings/(.*).json"

# Enfield
efd="./enfield/build/tools/efd"

# Hide excessive trap messages
trap "" SIGABRT

# Make Output Directory
mkdir -p $output

# Make Record File
record=$output/record.txt
# : > $record

# For every size category of QASM
for size in ./feedback_circuits/circuits/*; do
    # For every QASM in the size category
    for circuitfile in $size/*.qasm; do
        # For every Coupling Graph
        for couplingfile in ./feedback_circuits/couplings/*.json; do
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
            echo "Testing $circuitname on $couplingname" | tee -a $record
            # Run efd on QASM and Coupling Graph to Record File
            time ($efd -i $circuitfile -arch-file $couplingfile -stats 2>&1) \
                1> $output/$circuitname--$couplingname.txt \
                2>> $record
            CIRCUIT=`head -n -9 $output/$circuitname--$couplingname.txt`
            STATS=`tail -n 9 $output/$circuitname--$couplingname.txt`
            echo "$CIRCUIT" > $output/$circuitname--$couplingname.txt
            echo "$STATS" >> $record
        done
    done
done
