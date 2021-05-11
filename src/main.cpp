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

int main(int argc, char** argv)
{
	char *qasmFileName = NULL;
	char *couplingMapFileName = NULL;
	// int latency1 = 1;
	// int latency2 = 1;
	// int latencySwp = 1;

	// Parse command-line arguments:
	for(int iter = 1; iter < argc; iter++)
	{
		if (!strcmp(argv[iter], "-latency"))
		{
			// latency1 = atoi(argv[++iter]);
			++iter;
			// latency2 = atoi(argv[++iter]);
			++iter;
			// latencySwp = atoi(argv[++iter]);
			++iter;
		}
		else if (!qasmFileName)
		{
			qasmFileName = argv[iter];
		}
		else if (!couplingMapFileName)
		{
			couplingMapFileName = argv[iter];
		}
		else
		{
			assert(false);
		}
	}

	// Preprocess Circuit
	int num_logical_qubits = -1;
	pair<vector<vector<int>>, vector<GateNode*>> preprocessed = preprocess_circuit(
		qasmFileName,
		num_logical_qubits);
	vector<vector<int>> live_ranges = preprocessed.first;
	vector<GateNode*> gates_circuit = preprocessed.second;

	// Parse the coupling map; put edges into a set
	int num_physical_qubits = -1;
	set<pair<int, int> > couplings;
	buildCouplingMap(
		couplingMapFileName,
		couplings,
		num_physical_qubits);
	assert(num_physical_qubits >= num_logical_qubits);

	// Produce Mappings
	vector<pair<pair<int, int>, vector<int>>> mappings = sipf(
		couplings,
		num_logical_qubits,
		num_physical_qubits,
		live_ranges,
		gates_circuit);

	// Calculate Swaps
	vector<vector<pair<int, int>>> swaps = calculate_swaps(
		mappings,
		couplings,
		num_logical_qubits,
		num_physical_qubits);

	// Compile Circuit
	string circuit = compile_circuit(qasmFileName, mappings, swaps, gates_circuit);

	// Output Circuit
	cout << circuit << endl;

	// Clean up memory
	destroy_gates(gates_circuit);

	return 0;
}
