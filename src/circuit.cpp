#include "GateNode.hpp"
#include "QASMparser.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <utility>
#include <ctype.h>
using namespace std;

static int
parse_max_qubits(string line)
{
	string max_qubits_string = "";
	bool found = false;
	for (char c : line)
	{
		if (isdigit(c))
		{
			found = true;
			max_qubits_string += c;
		}
		else if (found == true)
		{
			break;
		}
	}

	return stoi(max_qubits_string);
}

static GateNode*
parse_gate(string line, set<int> &seen)
{
	// GateNode Members
	GateNode * v = new GateNode;
	v->name = "";
	v->control = UNDEFINED_QUBIT;
	v->target = UNDEFINED_QUBIT;
	v->controlChild = 0;
	v->targetChild = 0;
	v->controlParent = 0;
	v->targetParent = 0;

	unsigned int i = 0;

	// Parse Name
	for (; i < line.length(); i++)
	{
		char c = line[i];
		if (isspace(c))
		{
			break;
		}
		v->name += c;
	}

	// Ignore Whitespace
	while (isspace(line[i]))
	{
		i++;
	}

	// Parse First Argument
	string first_arg_string = "";
	for (; i < line.length(); i++)
	{
		char c = line[i];
		if (isdigit(c))
		{
			first_arg_string += c;
		}
		else if (c == ']')
		{
			i++;
			break;
		}
	}
	int first_arg = stoi(first_arg_string);
	seen.insert(first_arg);

	// If no second argument, set target and return
	if (line[i] != ',')
	{
		v->target = first_arg;
		return v;
	}

	// Otherwise, set control
	v->control = first_arg;

	// Ignore Whitespace
	while (isspace(line[i]) || line[i] == ',')
	{
		i++;
	}

	// Parse Second Argument
	string second_arg_string = "";
	for (; i < line.length(); i++)
	{
		char c = line[i];
		if (isdigit(c))
		{
			second_arg_string += c;
		}
		else if (c == ']')
		{
			break;
		}
	}
	int second_arg = stoi(second_arg_string);
	v->target = second_arg;
	seen.insert(second_arg);

	assert(v->control != v->target);

	return v;
}

pair<vector<vector<int>>, vector<GateNode*>>
preprocess_circuit(string qasmFileName, int &num_logical_qubits) {

	// Gates
	vector<GateNode*> gates_circuit;

	// Qubits
	set<int> seen;

	// Max Qubits
	int max_qubits = 0;

	// Parse QASM File
	fstream qasmFile;
	qasmFile.open(qasmFileName, ios::in);
	if (qasmFile.is_open())
	{
		string line;
		while (getline(qasmFile, line))
		{
			// Lines to Ignore
			if (line.rfind("OPENQASM", 0) == 0 ||
				line.rfind("include", 0) == 0 ||
				line.rfind("creg", 0) == 0 ||
				line.rfind("//", 0) == 0 ||
				line.length() == 0)
			{
				continue;
			}
			// Max Qubits
			else if (line.rfind("qreg", 0) == 0)
			{
				max_qubits = parse_max_qubits(line);
			}
			// Gate
			else
			{
				gates_circuit.push_back(parse_gate(line, seen));
			}
		}
		qasmFile.close();

		// Set Number of Qubits
		num_logical_qubits = seen.size();
	}
	else
	{
		cerr << "Error opening file \"" << qasmFileName << "\"" << endl;
		exit(1);
	}

	// Live Ranges with 2D Matrix Access: Row * Number of Qubits + Col
	vector<vector<int>> live_ranges(num_logical_qubits * num_logical_qubits);

	for (int i = 0; i < (int)gates_circuit.size(); i++)
	{
		GateNode* v = gates_circuit[i];

		// If gate is single
		if (v->control == UNDEFINED_QUBIT)
        {
			live_ranges[v->target * num_logical_qubits + v->target].push_back(i);
        }
        // If gate is double
		else
		{
			live_ranges[v->control * num_logical_qubits + v->target].push_back(i);
			live_ranges[v->target * num_logical_qubits + v->control].push_back(i);
		}
	}

	assert(num_logical_qubits <= max_qubits);

	return pair<vector<vector<int>>, vector<GateNode*>>(
		live_ranges,
		gates_circuit);
}


int latest_intersection(vector<vector<int>> live_ranges, pair<int, int> qubits, pair<int, int> range, int num_logical_qubits)
{
	vector<int> live_range_entries = live_ranges[qubits.first * num_logical_qubits + qubits.second];

	int lower_bound = range.first;
	int upper_bound = range.second;

	for (int i = live_range_entries.size() - 1; i >= 0; i--)
	{
		if (live_range_entries[i] >= upper_bound)
		{
			continue;
		}
		if (live_range_entries[i] > lower_bound)
		{
			return live_range_entries[i];
		}
	}

	return upper_bound - 1;
}

void destroy_gates(vector<GateNode*> gates_circuit)
{
	for (unsigned int i = 0; i < gates_circuit.size(); i++)
	{
		delete gates_circuit[i];
	}
}
