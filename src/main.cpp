#include "GateNode.hpp"
#include "QASMparser.h"
#include "util.cpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int UNDEFINED_QUBIT = -1;

#include "circuit.cpp"
#include "sipf.cpp"
#include "swapping.cpp"
#include "compiler.cpp"

int main(int argc, char** argv) {
	char * qasmFileName = NULL;
	char * couplingMapFileName = NULL;
	int latency1 = 1;
	int latency2 = 1;
	int latencySwp = 1;

	//Parse command-line arguments:
	for(int iter = 1; iter < argc; iter++) {
		if(!strcmp(argv[iter], "-latency")) {
			latency1 = atoi(argv[++iter]);
			latency2 = atoi(argv[++iter]);
			latencySwp = atoi(argv[++iter]);
		} else if(!qasmFileName) {
			qasmFileName = argv[iter];
		} else if(!couplingMapFileName) {
			couplingMapFileName = argv[iter];
		} else {
			assert(false);
		}
	}

	//Build dependency graph for the quantum circuit's gates; put dependency graph's roots into a set
	set<GateNode*> firstGates;
	int numLogicalQubits = -1;
	int numGates = -1;
	pair<vector<vector<int>>, vector<GateNode*>> preprocessed =
		preprocess_circuit(qasmFileName, firstGates, numLogicalQubits, numGates);
	vector<vector<int>> live_ranges = preprocessed.first;
	vector<GateNode*> gates_circuit = preprocessed.second;
	/*
	for (unsigned int q1 = 0; q1 < numLogicalQubits; q1++)
	{
		for (unsigned int q2 = 0; q2 < numLogicalQubits; q2++)
		{
			cout << "Gates between " << q1 << " and " << q2 << ":";
			for (auto iter = live_ranges[q1 * numLogicalQubits + q2].begin();
				 iter != live_ranges[q1 * numLogicalQubits + q2].end();
				 iter++)
			{
				cout << " " << *iter;
			}
			cout << endl;
		}
	}
	*/

	//Parse the coupling map; put edges into a set
	int numPhysicalQubits = 0;
	set<pair<int, int> > couplings;
	buildCouplingMap(couplingMapFileName, couplings, numPhysicalQubits);
	assert(numPhysicalQubits >= numLogicalQubits);

	//student code goes here?
	vector<pair<pair<int, int>, vector<int>>> mappings = sipf(firstGates, couplings, numLogicalQubits, numPhysicalQubits, live_ranges, gates_circuit);
    /*
    {
        cout << "Mappings with Ranges:" << endl;
        for (auto entry : mappings)
        {
            pair<int, int> range = entry.first;
            cout << "Range [" << range.first << "," << range.second << "]" << endl;
            vector<int> mapping = entry.second;
            cout << "//Location of qubits: ";
            for (int logical_qubit = 0; logical_qubit < numLogicalQubits; logical_qubit++)
            {
                cout << mapping[logical_qubit];
                if (logical_qubit != numLogicalQubits - 1) {
                    cout << ",";
                }
            }
            cout << endl;
        }
        exit(0);
    }
    */

	vector<vector<pair<int, int>>> swaps = calculate_swaps(mappings, couplings, numPhysicalQubits);

	string circuit = compile_circuit(qasmFileName, mappings, swaps, gates_circuit);
	cout << circuit << endl;

	//Exit the program:
	return 0;
}
