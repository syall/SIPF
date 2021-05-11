#include "GateNode.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
using namespace std;

string compile_circuit(
    string qasmFileName,
    vector<pair<pair<int, int>, vector<int>>> &mappings,
    vector<vector<pair<int, int>>> &swaps,
    vector<GateNode*> &gates_circuit)
{
    string circuit = "";

    // Read Header of QASM File
    {
        fstream qasmFile;
        qasmFile.open(qasmFileName, ios::in);
        if (qasmFile.is_open()){
            string line;
            while (getline(qasmFile, line)){
                // cout << "Line: " << line << endl;
                if (line.rfind("OPENQASM", 0) == 0 ||
                    line.rfind("include", 0) == 0 ||
                    line.rfind("qreg", 0) == 0 ||
                    line.rfind("creg", 0) == 0 ||
                    line.rfind("//", 0) == 0)
                {
                    circuit += line + "\n";
                }
                else if (line.length() == 0)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            qasmFile.close(); //close the file object.
        }
        else
        {
            cout << "Error opening file \"" << qasmFileName << "\"" << endl;
            exit(1);
        }
    }

    // Iterate Mappings
    for (
        unsigned int mappings_index = 0;
        mappings_index < mappings.size();
        mappings_index++)
    {
        // Write Initial Mapping Comment
        vector<int> initial_mapping = mappings[mappings_index].second;
        circuit += "//Location of qubits: ";
        for (unsigned int logical_qubit = 0; logical_qubit < initial_mapping.size(); logical_qubit++)
        {
            circuit += to_string(initial_mapping[logical_qubit]);
            if (logical_qubit != initial_mapping.size() - 1) {
                circuit +=  ",";
            }
        }
        circuit += "\n";

        // Write Gates up to Upper Bound
        int lower_bound = mappings[mappings_index].first.first;
        int upper_bound = mappings[mappings_index].first.second;
        for (
            int gate_index = lower_bound;
            gate_index <= upper_bound && gate_index < (int)gates_circuit.size();
            gate_index++)
        {
            GateNode* gate = gates_circuit[gate_index];
            circuit += gate->name + " ";
            // If gate is single
            if (gate->control == UNDEFINED_QUBIT)
            {
                circuit += "q[" + to_string(gate->target) + "]";
            }
            // If gate is double
            else
            {
                circuit +=
                    "q[" + to_string(gate->control) + "], " +
                    "q[" + to_string(gate->target) + "]";
            }
            circuit += ";\n";
        }

        // Insert swaps if not the last mapping
        if (mappings_index < mappings.size() - 1)
        {
            vector<pair<int, int>> swap_gates = swaps[mappings_index];
            for (pair<int, int> swap_gate : swap_gates)
            {
                // SWAP Gate using 3 CNOT Gates
                string control = to_string(swap_gate.first);
                string target = to_string(swap_gate.second);
                circuit += "//Swap " + control + " and " + target + "\n";
                // cx q[control], q[target]
                circuit += "cx ";
                circuit += "q[" + control + "]";
                circuit += ", ";
                circuit += "q[" + target + "]";
                // cx q[target], q[control]
                circuit += "cx ";
                circuit += "q[" + target + "]";
                circuit += ", ";
                circuit += "q[" + control + "]";
                // cx q[control], q[target]
                circuit += "cx ";
                circuit += "q[" + control + "]";
                circuit += ", ";
                circuit += "q[" + target + "]";
            }
        }


    }

    return circuit;
}
