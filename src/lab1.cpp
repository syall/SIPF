#include <set>
#include "GateNode.hpp"
using namespace std;

static const int UNDEFINED_QUBIT = -1;

static bool pcr_lab1_helper(set<GateNode *> &frontier, set<pair<int, int>> &couplings, int **mapping, int numLogicalQubits, int numPhysicalQubits, set<GateNode *> &seen);

/**
 * Partitioning of Control Relations (PCR)
 * @param frontier Input: Set of Next Candidates
 * @param couplings Input: Coupling Graph
 * @param numLogicalQubits Input: Number of Logical Qubits to be mapped
 * @param numPhysicalQubits Input: Maximum number of Physical Qubits to be mapped
 * @return mapping of logical to physical qubits if possible, otherwise NULL
 */
int *pcr_lab1(set<GateNode *> &frontier, set<pair<int, int>> &couplings, int numLogicalQubits, int numPhysicalQubits)
{
    // Initial Empty Mapping
    int *mapping = (int *)malloc(numLogicalQubits * sizeof(int));
    for (int i = 0; i < numLogicalQubits; i++)
    {
        mapping[i] = UNDEFINED_QUBIT;
    }

    // Scheduled Control Relations
    set<GateNode *> seen;

    // Recursively Search for complete initial mapping
    if (pcr_lab1_helper(frontier, couplings, &mapping, numLogicalQubits, numPhysicalQubits, seen))
    {
        return mapping;
    }
    // No possible complete initial mapping
    else
    {
        free(mapping);
        return NULL;
    }
}

/**
 * Partitioning of Control Relations (PCR) Helper
 * @param frontier Input: Set of Next Candidates
 * @param couplings Input: Coupling Graph
 * @param mapping Input: Current Mapping of Qubits
 * @param numLogicalQubits Input: Number of Logical Qubits to be mapped
 * @param numPhysicalQubits Input: Maximum number of Physical Qubits to be mapped
 * @param seen Input: Set of GateNodes seen
 * @returns whether complete initial mapping is possible
 */
static bool pcr_lab1_helper(set<GateNode *> &frontier, set<pair<int, int>> &couplings, int **mapping, int numLogicalQubits, int numPhysicalQubits, set<GateNode *> &seen)
{
    // If no more relations to schedule, success
    if (frontier.size() == 0)
    {
        for (int i = 0; i < numLogicalQubits; i++)
        {
            if ((*mapping)[i] == UNDEFINED_QUBIT)
            {
                return false;
            }
        }
        return true;
    }

    for (GateNode *gate : frontier)
    {
        // Skip Gate if dependencies are not scheduled
        // If Gate is target only
        if (gate->control == UNDEFINED_QUBIT)
        {
            // If target parent is not NULL and not seen before, skip Gate
            if (gate->targetParent != NULL && seen.find(gate->targetParent) == seen.end())
            {
                continue;
            }
        }
        // If Gate is target and control
        else
        {
            // If target parent is not NULL and not seen before, skip Gate
            if (gate->targetParent != NULL && seen.find(gate->targetParent) == seen.end())
            {
                continue;
            }
            // If control parent is not NULL and not seen before, skip Gate
            if (gate->controlParent != NULL && seen.find(gate->controlParent) == seen.end())
            {
                continue;
            }
        }

        // Remove Gate from Frontier
        set<GateNode *> new_frontier = frontier;
        new_frontier.erase(gate);

        // Add Gate to Seen
        set<GateNode *> new_seen = seen;
        new_seen.insert(gate);

        // Add Gate Successors to Frontier if not NULL
        // If Gate is target only
        if (gate->control == UNDEFINED_QUBIT)
        {
            if (gate->targetChild != NULL)
            {
                new_frontier.insert(gate->targetChild);
            }
        }
        // If Gate is target and control
        else
        {
            if (gate->targetChild != NULL)
            {
                new_frontier.insert(gate->targetChild);
            }
            if (gate->controlChild != NULL)
            {
                new_frontier.insert(gate->controlChild);
            }
        }

        // Search with Scheduled Gate
        // If Gate is target only
        if (gate->control == UNDEFINED_QUBIT)
        {
            // If target qubit is already mapped
            if ((*mapping)[gate->target] != UNDEFINED_QUBIT)
            {
                // Search with new Frontier, Seen, and same Mapping
                return pcr_lab1_helper(new_frontier, couplings, mapping, numLogicalQubits, numPhysicalQubits, new_seen);
            }
            // If target qubit is not mapped
            else
            {
                // New Mapping
                set<int> unmapped_qubits;
                for (int i = 0; i < numPhysicalQubits; i++)
                {
                    unmapped_qubits.insert(i);
                }
                int *new_mapping = (int *)malloc(numLogicalQubits * sizeof(int));
                for (int i = 0; i < numLogicalQubits; i++)
                {
                    new_mapping[i] = (*mapping)[i];
                    if (new_mapping[i] != UNDEFINED_QUBIT)
                    {
                        unmapped_qubits.erase(new_mapping[i]);
                    }
                }

                // For each available unmapped qubit
                for (auto x : unmapped_qubits)
                {
                    // Set target qubit
                    new_mapping[gate->target] = x;

                    // Search with new Frontier, Seen, Mapping
                    if (pcr_lab1_helper(new_frontier, couplings, &new_mapping, numLogicalQubits, numPhysicalQubits, new_seen))
                    {
                        free(*mapping);
                        *mapping = new_mapping;
                        return true;
                    }
                    // Continue to next unmapped qubit
                    else
                    {
                        new_mapping[gate->target] = UNDEFINED_QUBIT;
                    }
                }

                // If target qubit could not be mapped
                free(new_mapping);
                return false;
            }
        }
        // If Gate is target and control
        else
        {
            // If both target and control are already mapped
            if ((*mapping)[gate->target] != UNDEFINED_QUBIT && (*mapping)[gate->control] != UNDEFINED_QUBIT)
            {
                pair<int, int> edge = (*mapping)[gate->target] <= (*mapping)[gate->control]
                                          ? make_pair((*mapping)[gate->target], (*mapping)[gate->control])
                                          : make_pair((*mapping)[gate->control], (*mapping)[gate->target]);
                // If target and control are not near in the coupling graph
                if (couplings.find(edge) != couplings.end())
                {
                    return false;
                }
                else
                {
                    // Search with new Frontier, Seen, and same Mapping
                    return pcr_lab1_helper(new_frontier, couplings, mapping, numLogicalQubits, numPhysicalQubits, new_seen);
                }
            }
            // If only target is already mapped
            else if ((*mapping)[gate->target] != UNDEFINED_QUBIT)
            {
                // New Mapping
                set<int> unmapped_qubits;
                for (int i = 0; i < numPhysicalQubits; i++)
                {
                    unmapped_qubits.insert(i);
                }
                int *new_mapping = (int *)malloc(numLogicalQubits * sizeof(int));
                for (int i = 0; i < numLogicalQubits; i++)
                {
                    new_mapping[i] = (*mapping)[i];
                    if (new_mapping[i] != UNDEFINED_QUBIT)
                    {
                        unmapped_qubits.erase(new_mapping[i]);
                    }
                }

                // For each available unmapped qubit near the target qubit
                for (auto x : unmapped_qubits)
                {
                    pair<int, int> edge = x <= (*mapping)[gate->target]
                                              ? make_pair(x, (*mapping)[gate->target])
                                              : make_pair((*mapping)[gate->target], x);
                    if (couplings.find(edge) == couplings.end())
                    {
                        continue;
                    }

                    // Set control qubit
                    new_mapping[gate->control] = x;

                    // Search with new Frontier, Seen, Mapping
                    if (pcr_lab1_helper(new_frontier, couplings, &new_mapping, numLogicalQubits, numPhysicalQubits, new_seen))
                    {
                        free(*mapping);
                        *mapping = new_mapping;
                        return true;
                    }
                    // Continue to next unmapped qubit
                    else
                    {
                        new_mapping[gate->control] = UNDEFINED_QUBIT;
                    }
                }

                // If control qubit could not be mapped
                free(new_mapping);
                return false;
            }
            // If only control is already mapped
            else if ((*mapping)[gate->control] != UNDEFINED_QUBIT)
            {
                // New Mapping
                set<int> unmapped_qubits;
                for (int i = 0; i < numPhysicalQubits; i++)
                {
                    unmapped_qubits.insert(i);
                }
                int *new_mapping = (int *)malloc(numLogicalQubits * sizeof(int));
                for (int i = 0; i < numLogicalQubits; i++)
                {
                    new_mapping[i] = (*mapping)[i];
                    if (new_mapping[i] != UNDEFINED_QUBIT)
                    {
                        unmapped_qubits.erase(new_mapping[i]);
                    }
                }

                // For each available unmapped qubit near the control qubit
                for (auto x : unmapped_qubits)
                {
                    pair<int, int> edge = x <= (*mapping)[gate->control]
                                              ? make_pair(x, (*mapping)[gate->control])
                                              : make_pair((*mapping)[gate->control], x);
                    if (couplings.find(edge) == couplings.end())
                    {
                        continue;
                    }

                    // Set target qubit
                    new_mapping[gate->target] = x;

                    // Search with new Frontier, Seen, Mapping
                    if (pcr_lab1_helper(new_frontier, couplings, &new_mapping, numLogicalQubits, numPhysicalQubits, new_seen))
                    {
                        free(*mapping);
                        *mapping = new_mapping;
                        return true;
                    }
                    // Continue to next unmapped qubit
                    else
                    {
                        new_mapping[gate->target] = UNDEFINED_QUBIT;
                    }
                }

                // If target qubit could not be mapped
                free(new_mapping);
                return false;
            }
            // If both target and control are not mapped
            else
            {
                // New Mapping
                set<int> unmapped_qubits1;
                for (int i = 0; i < numPhysicalQubits; i++)
                {
                    unmapped_qubits1.insert(i);
                }
                int *new_mapping = (int *)malloc(numLogicalQubits * sizeof(int));
                for (int i = 0; i < numLogicalQubits; i++)
                {
                    new_mapping[i] = (*mapping)[i];
                    if (new_mapping[i] != UNDEFINED_QUBIT)
                    {
                        unmapped_qubits1.erase(new_mapping[i]);
                    }
                }

                // For each available unmapped qubit1 map target
                for (auto x1 : unmapped_qubits1)
                {
                    // Set target qubit
                    new_mapping[gate->target] = x1;

                    // For each available other unmapped qubit2 map control
                    set<int> unmapped_qubits2 = unmapped_qubits1;
                    unmapped_qubits2.erase(x1);
                    for (auto x2 : unmapped_qubits2)
                    {
                        pair<int, int> edge = x1 <= x2
                                                  ? make_pair(x1, x2)
                                                  : make_pair(x2, x1);
                        if (couplings.find(edge) == couplings.end())
                        {
                            continue;
                        }

                        // Search with new Frontier, Seen, Mapping
                        if (pcr_lab1_helper(new_frontier, couplings, &new_mapping, numLogicalQubits, numPhysicalQubits, new_seen))
                        {
                            free(*mapping);
                            *mapping = new_mapping;
                            return true;
                        }
                        // Continue to next unmapped qubit
                        else
                        {
                            new_mapping[gate->target] = UNDEFINED_QUBIT;
                        }
                    }

                    new_mapping[gate->target] = UNDEFINED_QUBIT;
                }

                // If both target and control could not be mapped
                free(new_mapping);
                return false;
            }
        }
    }

    // If no complete initial mapping is possible
    return false;
}
