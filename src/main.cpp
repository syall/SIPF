#include "GateNode.hpp"
#include "QASMparser.h"
#include "util.cpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#include "lab1.cpp"

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
	buildDependencyGraph(qasmFileName, firstGates, numLogicalQubits, numGates);

	//Parse the coupling map; put edges into a set
	int numPhysicalQubits = 0;
	set<pair<int, int> > couplings;
	buildCouplingMap(couplingMapFileName, couplings, numPhysicalQubits);
	assert(numPhysicalQubits >= numLogicalQubits);


	//student code goes here?
	vector<int> mapping = lab1(firstGates, couplings, numLogicalQubits, numPhysicalQubits);
	if (mapping.empty()) {
		cout << "No complete initial mapping exists" << endl;
	} else {
		cout << "Initial mapping exists" << endl;
		for (int logical_qubit = 0; logical_qubit < numLogicalQubits; logical_qubit++)
		{
			cout << logical_qubit << " " << mapping[logical_qubit] << endl;
		}
	}

	destroyDependencyGraph(firstGates);

	//Exit the program:
	return 0;
}
