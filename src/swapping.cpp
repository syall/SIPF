#include <vector>
#include <set>
#include <queue>
#include <utility>
#include <algorithm>
using namespace std;

static vector<vector<int>>
create_distance_matrix(
    vector<set<int>> &coupling_graph,
    int num_physical_qubits)
{

    // Cost Matrix
    vector<vector<int>> cost_matrix(
        num_physical_qubits,
        vector<int>(
            num_physical_qubits,
            num_physical_qubits + 1));
    for (int i = 0; i < num_physical_qubits; i++)
    {
        for (int neighbor : coupling_graph[i])
        {
            cost_matrix[i][neighbor] = 1;
        }
    }

    // Distance Matrix
    vector<vector<int>> distance_matrix(
        num_physical_qubits,
        vector<int>(
            num_physical_qubits,
            num_physical_qubits + 1));
    for (int i = 0; i < num_physical_qubits; i++)
    {
        for (int j = 0; j < num_physical_qubits; j++)
        {
            distance_matrix[i][j] = cost_matrix[i][j];
        }
        distance_matrix[i][i] = 0;
    }

    // Predecessor Matrix
    vector<vector<int>> pred_matrix(
        num_physical_qubits,
        vector<int>(
            num_physical_qubits,
            UNDEFINED_QUBIT));

    for (int k = 0; k < num_physical_qubits; k++)
    {
        for (int i = 0; i < num_physical_qubits; i++)
        {
            for (int j = 0; j < num_physical_qubits; j++)
            {
                if (distance_matrix[i][k] + distance_matrix[k][j] < distance_matrix[i][j])
                {
                    distance_matrix[i][j] = distance_matrix[i][k] + distance_matrix[k][j];
                    pred_matrix[i][j] = k;
                }
            }
        }
    }

    return distance_matrix;
}

static bool
swap_qubits(
    vector<int> &mapping1,
    vector<int> &mapping2,
    vector<int> actual_mapping,
    int cost,
    vector<pair<int, int>> &swaps,
    vector<set<int>> &coupling_graph,
    vector<vector<int>> &distance_matrix,
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
            unsigned int index =
                find(test_mapping.begin(), test_mapping.end(), neighbor) -
                test_mapping.begin();
            int i_cost = distance_matrix[mapping1[i]][mapping2[i]];
            int new_i_cost = distance_matrix[neighbor][mapping2[i]];
            int neighbor_cost = index != test_mapping.size()
                ? distance_matrix[neighbor][mapping2[index]]
                : coupling_graph.size() / 2;
            int new_neighbor_cost = index != test_mapping.size()
                ? distance_matrix[mapping1[i]][mapping2[index]]
                : coupling_graph.size() / 2;
            int current_cost = cost
                // i's swap
                - i_cost
                + new_i_cost
                // neighbor's swap
                - neighbor_cost
                + new_neighbor_cost;
            if (current_cost > cost)
            {
                continue;
            }
            int temp = test_mapping[i];
            test_mapping[i] = neighbor;
            if (index != test_mapping.size())
            {
                test_mapping[index] = temp;
            }
            swaps.push_back(pair<int, int>(temp, neighbor));
            if (swap_qubits(
                test_mapping,
                mapping2,
                actual_mapping,
                current_cost,
                swaps,
                coupling_graph,
                distance_matrix,
                num_logical_qubits,
                depth - 1))
            {
                actual_mapping = test_mapping;
                return true;
            }
            swaps.pop_back();
            if (index != test_mapping.size())
            {
                test_mapping[index] = neighbor;
            }
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

    // Build distance matrix
    vector<vector<int>> distance_matrix = create_distance_matrix(
        coupling_graph,
        num_physical_qubits);

    // Total Swaps
    vector<vector<pair<int, int>>> swaps(mappings.size() - 1);

    // Iterate mappings by pairs
    for (unsigned int index = 0; index < mappings.size() - 1; index++)
    {
        vector<pair<int, int>> local_swaps;
        vector<int> mapping1 = mappings[index].second;
        vector<int> mapping2 = mappings[index + 1].second;
        vector<int> actual_mapping = mapping1;

        // Initial Cost
        int cost = 0;
        for (int i = 0; i < num_logical_qubits; i++)
        {
            if (mapping1[i] == UNDEFINED_QUBIT)
            {
                continue;
            }
            cost += distance_matrix[mapping1[i]][mapping2[i]];
        }

        // Find smallest swaps between 2 mappings
        // 4-approximation Cost Lower Bound = Cost / 2 (Miltzow et al. 2016)
        for (int depth = cost / 2; true; depth++)
        {
            if (swap_qubits(
                mapping1,
                mapping2,
                actual_mapping,
                cost,
                local_swaps,
                coupling_graph,
                distance_matrix,
                num_logical_qubits,
                depth))
            {
                // Assign Swaps
                swaps[index] = local_swaps;

                // Propagate Values from Mapping 1 to Mapping 2
                for (int i = 0; i < num_logical_qubits; i++)
                {
                    if (mapping2[i] == UNDEFINED_QUBIT)
                    {
                        mapping2[i] = actual_mapping[i];
                    }
                }
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
