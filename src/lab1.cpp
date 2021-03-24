#include <set>
#include "GateNode.hpp"
using namespace std;

static const int UNDEFINED_QUBIT = -1;

void print_gate(GateNode *gate);

/**
 * Partitioning of Control Relations (PCR)
 * @param controlRelations Input: Set of Next Dependency Graph
 * @param couplings Input: Coupling Graph
 * @param mapping Input: Mapping of logical to physical qubits
 * @param numQubits Input: Maximum number of Qubits to be mapped
 * @return mapping of logical to physical qubits if possible, otherwise NULL
 */
int *pcr_lab1(set<GateNode *> &controlRelations, set<pair<int, int> > &couplings, int *mapping, int numQubits)
{
    // Initial Empty Mapping
    if (mapping == NULL)
    {
        mapping = (int *)malloc(numQubits * sizeof(int));
        for (int i = 0; i < numQubits; i++)
        {
            mapping[i] = UNDEFINED_QUBIT;
        }
    }

    // Explore Search Space
    for (GateNode *gate : controlRelations)
    {
        print_gate(gate);
        printf("\n");
    }

    // No mapping is possible
    if (mapping != NULL)
    {
        free(mapping);
    }
    return NULL;
}

void print_gate(GateNode *gate)
{
    cout << "Gate: " << gate->name << endl;
    cout << "  control: " << gate->control << endl;
    cout << "  target: " << gate->target << endl;
    cout << "  control child: " << gate->controlChild << endl;
    cout << "  target child: " << gate->targetChild << endl;
    cout << "  control parent: " << gate->controlParent << endl;
    cout << "  target parent: " << gate->targetParent << endl;
}
