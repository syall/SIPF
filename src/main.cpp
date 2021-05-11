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

	// Preprocess Circuit
	set<GateNode*> firstGates;
	int numLogicalQubits = -1;
	int numGates = -1;
	pair<vector<vector<int>>, vector<GateNode*>> preprocessed =
		preprocess_circuit(qasmFileName, firstGates, numLogicalQubits, numGates);
	vector<vector<int>> live_ranges = preprocessed.first;
	vector<GateNode*> gates_circuit = preprocessed.second;

	// Parse the coupling map; put edges into a set
	int numPhysicalQubits = 0;
	set<pair<int, int> > couplings;
	buildCouplingMap(couplingMapFileName, couplings, numPhysicalQubits);
	assert(numPhysicalQubits >= numLogicalQubits);

	// Produce Mappings
	vector<pair<pair<int, int>, vector<int>>> mappings = sipf(firstGates, couplings, numLogicalQubits, numPhysicalQubits, live_ranges, gates_circuit);

	// Calculate Swaps
	vector<vector<pair<int, int>>> swaps = calculate_swaps(
		mappings,
		couplings,
		numLogicalQubits,
		numPhysicalQubits);

	// Compile Circuit
	string circuit = compile_circuit(qasmFileName, mappings, swaps, gates_circuit);

	// Output Circuit
	cout << circuit << endl;

	return 0;
}
