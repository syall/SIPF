#!/bin/bash

# Output Directory
output=feedback/

# feedback_circuits/circuit/*/(*).qasm Capture Regex
qasmregex="./feedback_circuits/circuits/.*/(.*).qasm"

# feedback_circuits/couplings/(*).txt Capture Regex
txtregex="./feedback_circuits/couplings/(.*).txt"

# Assertion Regex
assertionregex="^mapper:"

# Hide excessive trap messages
trap "" SIGABRT

# Make Output Directory
mkdir -p $output

# Make Record File
record=$output/record.txt
: > $record

# For every size category of QASM
for size in ./feedback_circuits/circuits/*; do
    # For every QASM in the size category
    for circuitfile in $size/*.qasm; do
        # For every Coupling Graph
        for couplingfile in ./feedback_circuits/couplings/*.txt; do
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
            # Run mapper on QASM and Coupling Graph to Output File
            time (./mapper $circuitfile $couplingfile 2>&1) \
                1> $output/$circuitname--$couplingname.txt \
                2>> $record
            OUTPUT=`head -n 4 $output/$circuitname--$couplingname.txt`
            if [[ $OUTPUT =~ $assertionregex ]]
            then
                echo "No Mappings Possible" >> $record
            else
                echo "$OUTPUT" >> $record
            fi
        done
    done
done
