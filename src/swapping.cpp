#include <vector>
#include <set>
#include <queue>
#include <utility>
#include <algorithm>
using namespace std;

static bool
swap_qubits(
    vector<int> &mapping1,
    vector<int> &mapping2,
    vector<pair<int, int>> &swaps,
    vector<set<int>> &coupling_graph,
    int num_logical_qubits,
    int depth)
{
    if (depth < 0)
    {
        return false;
    }

    if (mapping1 == mapping2)
    {
        return true;
    }

    for (int i = 0; i < num_logical_qubits; i++)
    {
        if (mapping1[i] == mapping2[i])
        {
            continue;
        }

        vector<int> test_mapping = mapping1;
        for (int neighbor : coupling_graph[mapping1[i]])
        {
            int temp = test_mapping[i];
            test_mapping[i] = neighbor;
            int index = find(test_mapping.begin(), test_mapping.end(), neighbor) - test_mapping.begin();
            test_mapping[index] = temp;
            swaps.push_back(pair<int, int>(temp, neighbor));
            if (swap_qubits(
                test_mapping,
                mapping2,
                swaps,
                coupling_graph,
                num_logical_qubits,
                depth - 1))
            {
                return true;
            }
            swaps.pop_back();
            test_mapping[index] = neighbor;
            test_mapping[i] = temp;
        }

    }

    return false;
}

vector<vector<pair<int, int>>>
calculate_swaps(
    vector<pair<pair<int, int>, vector<int>>> &mappings,
    set<pair<int, int>> &couplings,
    int num_logical_qubits,
    int num_physical_qubits)
{

    // Construct coupling graph
    vector<set<int>> coupling_graph = create_data_graph(
        couplings,
        num_physical_qubits,
        set<int>());

    // Total Swaps
    vector<vector<pair<int, int>>> swaps(mappings.size() - 1);

    // Iterate mappings by pairs
    for (unsigned int index = 0; index < mappings.size() - 1; index++)
    {
        vector<pair<int, int>> local_swaps;
        vector<int> mapping1 = mappings[index].second;
        vector<int> mapping2 = mappings[index + 1].second;

        // Find best swaps between 2 mappings
        for (int depth = 1; true; depth++)
        {
            if (swap_qubits(
                mapping1,
                mapping2,
                local_swaps,
                coupling_graph,
                num_logical_qubits,
                depth))
            {
                swaps[index] = local_swaps;
                break;
            }
            else
            {
                local_swaps.clear();
            }
        }
    }

    return swaps;
}
