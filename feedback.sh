#!/bin/bash

# Output Directory
output=feedback/

# feedback_circuits/circuit/*/(*).qasm Capture Regex
qasmregex="./feedback_circuits/circuits/.*/(.*).qasm"

# feedback_circuits/couplings/(*).txt Capture Regex
txtregex="./feedback_circuits/couplings/(.*).txt"

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
            OUTPUT=`cat $output/$circuitname--$couplingname.txt`
            LINE=$(echo "$OUTPUT" | grep '//Number of Swaps: ')
            SWAPS=$(echo "$LINE" | cut -d ' ' -f 4 | sed 's/.$//')
            MAPPINGS=$(echo "$LINE" | cut -d ' ' -f 8)
            if [ -z "$SWAPS" ]
            then
                echo "No Mappings Possible" >> $record
            else
                echo "$SWAPS Swaps for $MAPPINGS Mappings" >> $record
            fi
        done
    done
done
