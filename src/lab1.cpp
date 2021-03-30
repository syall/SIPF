#include <set>
#include <queue>
#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <limits>
#include "GateNode.hpp"
using namespace std;

const int UNDEFINED_QUBIT = -1;

vector<int>
lab1(
    set<GateNode *> &frontier,
    set<pair<int, int>> &couplings,
    int num_logical_qubits,
    int num_physical_qubits);

static vector<set<int>>
create_query_graph(
    set<GateNode *> &frontier,
    int num_logical_qubits);

static vector<set<int>>
create_data_graph(
    set<pair<int, int>> &couplings,
    int num_physical_qubits);

static pair<vector<set<int>>, pair<int, vector<set<int>>>>
create_dag(
    const vector<set<int>> &query_graph,
    const vector<set<int>> &data_graph);

static vector<set<int>>
reverse_dag(
    vector<set<int>> &query_dag);

static pair<vector<set<int>>, vector<set<int>>>
create_candidate_space(
    vector<set<int>> &query_graph,
    vector<set<int>> &candidate_sets,
    vector<set<int>> &query_dag,
    vector<set<int>> &data_graph);

static void
filter_candidates_with_dag(
    vector<set<int>> &candidate_sets,
    vector<set<int>> &candidate_edges,
    vector<set<int>> &dag);

static vector<int>
backtrack_candidate_space(
    vector<set<int>> &query_graph,
    vector<set<int>> &query_dag,
    int dag_root,
    vector<set<int>> &candidate_sets,
    vector<set<int>> &candidate_edges,
    vector<int> &mapping);

static bool
backtrack_candidate_space_helper(
    vector<set<int>> &query_graph,
    vector<set<int>> &query_dag,
    vector<set<int>> &candidate_sets,
    vector<set<int>> &candidate_edges,
    vector<int> &mapping,
    set<int> &frontier,
    set<int> &seen,
    set<int> &mapped,
    vector<set<int>> &parents);

void destroyDependencyGraph(set<GateNode *> &firstGates);

/**
 * Partitioning of Control Relations (PCR)
 * @param frontier Input: Set of Next Candidates
 * @param couplings Input: Coupling Graph
 * @param num_logical_qubits Input: Number of Logical Qubits to be mapped
 * @param num_physical_qubits Input: Maximum number of Physical Qubits to be mapped
 * @return mapping of logical to physical qubits if possible, otherwise NULL
 */
vector<int>
lab1(
    set<GateNode *> &frontier,
    set<pair<int, int>> &couplings,
    int num_logical_qubits,
    int num_physical_qubits)
{
    // Input: query graph q
    vector<set<int>> logical_graph = create_query_graph(
        frontier,
        num_logical_qubits);
    /*
    cout << "Logical Graph:" << endl;
    for (unsigned int q = 0; q < logical_graph.size(); q++)
    {
        cout << q << ":";
        for (int neighbor : logical_graph[q])
        {
            cout << " " << neighbor;
        }
        cout << endl;
    }
    */

    // Input: data graph G
    vector<set<int>> physical_graph = create_data_graph(
        couplings,
        num_physical_qubits);
    /*
    cout << "Physical Graph:" << endl;
    for (unsigned int q = 0; q < physical_graph.size(); q++)
    {
        cout << q << ":";
        for (int neighbor : physical_graph[q])
        {
            cout << " " << neighbor;
        }
        cout << endl;
    }
    */

    // q_D <- BuildDAG(q, G)
    pair<vector<set<int>>, pair<int, vector<set<int>>>> logical_dag_result = create_dag(
        logical_graph,
        physical_graph);
    vector<set<int>> candidate_sets = logical_dag_result.first;
    int dag_root = logical_dag_result.second.first;
    vector<set<int>> logical_dag = logical_dag_result.second.second;
    /*
    cout << "Logical DAG with root " << dag_root << ":" << endl;
    for (unsigned int q = 0; q < logical_dag.size(); q++)
    {
        cout << q << ":";
        for (int neighbor : logical_dag[q])
        {
            cout << " " << neighbor;
        }
        cout << endl;
    }
    */

    // CS <- BuildCS(q, q_D, G)
    pair<vector<set<int>>, vector<set<int>>> candidate_space = create_candidate_space(
        logical_graph,
        candidate_sets,
        logical_dag,
        physical_graph);
    vector<set<int>> candidate_edges = candidate_space.second;
    /*
    cout << "Candidate Sets:" << endl;
    for (unsigned int q = 0; q < candidate_sets.size(); q++)
    {
        cout << q << ":";
        for (int candidate : candidate_sets[q])
        {
            cout << " " << candidate;
        }
        cout << endl;
    }
    cout << "Candidate Edges:" << endl;
    for (unsigned int q = 0; q < candidate_edges.size(); q++)
    {
        cout << q << ":";
        for (int neighbor : candidate_edges[q])
        {
            cout << " " << neighbor;
        }
        cout << endl;
    }
    */

    // M <- EMPTY
    vector<int> mapping(num_logical_qubits, UNDEFINED_QUBIT);

    // Backtrack(q, q_D, CS, M)
    backtrack_candidate_space(
        logical_graph,
        logical_dag,
        dag_root,
        candidate_sets,
        candidate_edges,
        mapping);

    return mapping;
}

/**
 * Create Logical Graph from Dependency Graph
 * @param frontier Input: First Set of Candidates
 * @param num_logical_qubits Input: Number of Logical Qubits
 * @returns graph where indices are vertices and values are adjacent vertices
 */
static vector<set<int>>
create_query_graph(
    set<GateNode *> &frontier,
    int num_logical_qubits)
{
    vector<set<int>> logical_graph(num_logical_qubits);

    // BFS
    queue<GateNode *> search;
    set<GateNode *> seen;
    for (auto gate : frontier)
    {
        search.push(gate);
        seen.insert(gate);
    }
    while (!search.empty())
    {
        GateNode *current = search.front();
        search.pop();

        // Search on Gate
        // If Gate is target only
        if (current->control == UNDEFINED_QUBIT)
        {
            if (current->targetChild != NULL && seen.find(current->targetChild) == seen.end())
            {
                seen.insert(current->targetChild);
                search.push(current->targetChild);
            }
        }
        // If Gate is target and control
        else
        {
            logical_graph[current->target].insert(current->control);
            logical_graph[current->control].insert(current->target);
            if (current->targetChild != NULL && seen.find(current->targetChild) == seen.end())
            {
                seen.insert(current->targetChild);
                search.push(current->targetChild);
            }
            if (current->controlChild != NULL && seen.find(current->controlChild) == seen.end())
            {
                seen.insert(current->controlChild);
                search.push(current->controlChild);
            }
        }
    }

    return logical_graph;
}

/**
 * Create Physical Graph from Coupling Architecture
 * @param couplings Input: Coupling Graph Edges
 * @param num_physical_qubits Input: Number of Physical Qubits
 * @returns graph where indices are vertices and values are adjacent vertices
 */
static vector<set<int>>
create_data_graph(
    set<pair<int, int>> &couplings,
    int num_physical_qubits)
{
    vector<set<int>> physical_graph(num_physical_qubits);

    // Iterate Edges to form Adjacency Lists
    for (auto edge : couplings)
    {
        physical_graph[edge.first].insert(edge.second);
        physical_graph[edge.second].insert(edge.first);
    }

    return physical_graph;
}

static pair<vector<set<int>>, pair<int, vector<set<int>>>>
create_dag(
    const vector<set<int>> &query_graph,
    const vector<set<int>> &data_graph)
{
    vector<set<int>> candidate_sets(query_graph.size());

    pair<int, float> minimum_root(0, numeric_limits<double>::max());
    for (unsigned int root = 0; root < query_graph.size(); root++)
    {
        unsigned int root_degree = query_graph[root].size();
        for (unsigned int v = 0; v < data_graph.size(); v++)
        {
            if (data_graph[v].size() >= root_degree)
            {
                candidate_sets[root].insert(v);
            }
        }
        double heuristic = (double)candidate_sets[root].size() / (double)root_degree;
        if (heuristic < minimum_root.second)
        {
            minimum_root.first = root;
            minimum_root.second = heuristic;
        }
    }

    vector<set<int>> minimum_root_dag(query_graph.size());
    // BFS
    queue<int> search;
    search.push(minimum_root.first);
    set<int> seen;
    while (!search.empty())
    {
        int current = search.front();
        search.pop();
        seen.insert(current);
        for (int neighbor : query_graph[current])
        {
            if (seen.find(neighbor) == seen.end())
            {
                minimum_root_dag[current].insert(neighbor);
                search.push(neighbor);
            }
        }
    }

    return pair<vector<set<int>>, pair<int, vector<set<int>>>>(
        candidate_sets,
        pair<int, vector<set<int>>>(minimum_root.first, minimum_root_dag));
}

static vector<set<int>>
reverse_dag(
    vector<set<int>> &query_dag)
{
    vector<set<int>> reversed_dag(query_dag.size());
    for (unsigned int v = 0; v < query_dag.size(); v++)
    {
        for (auto neighbor : query_dag[v])
        {
            reversed_dag[neighbor].insert(v);
        }
    }
    return reversed_dag;
}

static pair<vector<set<int>>, vector<set<int>>>
create_candidate_space(
    vector<set<int>> &query_graph,
    vector<set<int>> &candidate_sets,
    vector<set<int>> &query_dag,
    vector<set<int>> &data_graph)
{
    // Candidate Space = Sets(query -> set<data>), Edges(data -> set<data>)
    vector<set<int>> candidate_edges(data_graph.size());
    for (unsigned int u1 = 0; u1 < candidate_sets.size(); u1++)
    {
        for (unsigned int u2 = 0; u2 < candidate_sets.size(); u2++)
        {
            // There is an edge between q1 and q2 in the query graph
            if (query_graph[u1].find(u2) == query_graph[u1].end())
            {
                continue;
            }
            // Where d1 and d2 are data graph vertices in candidate sets of u1 and u2
            for (int d1 : candidate_sets[u1])
            {
                for (int d2 : candidate_sets[u2])
                {
                    // There is an edge between d1 and d2 in the query graph
                    if (data_graph[d1].find(d2) != data_graph[d1].end())
                    {
                        candidate_edges[d1].insert(d2);
                        candidate_edges[d2].insert(d1);
                    }
                }
            }
        }
    }

    // q_D^-1
    vector<set<int>> reversed_query_dag = reverse_dag(query_dag);

    // Filter with q_D^-1
    // filter_candidates_with_dag(candidate_sets, candidate_edges, reversed_query_dag);

    // Filter with q_D
    // filter_candidates_with_dag(candidate_sets, candidate_edges, query_dag);

    // Filter with q_D^-1
    // filter_candidates_with_dag(candidate_sets, candidate_edges, reversed_query_dag);

    return pair<vector<set<int>>, vector<set<int>>>(candidate_sets, candidate_edges);
}

static void
filter_candidates_with_dag(
    vector<set<int>> &candidate_sets,
    vector<set<int>> &candidate_edges,
    vector<set<int>> &dag)
{
    for (unsigned int v = 0; v < dag.size(); v++)
    {
        // For every neighbor of v
        for (int neighbor : dag[v])
        {
            // For every candidate of v
            set<int> candidates = candidate_sets[v];
            // If candidate of v c1 is not connected to any neighbor candidate c2, remove c1
            for (int c1 : candidates)
            {
                if (!any_of(candidate_sets[neighbor].begin(), candidate_sets[neighbor].end(),
                            [&candidate_edges, c1](int c2) { return candidate_edges[c1].find(c2) != candidate_edges[c1].end(); }))
                {
                    for (set<int> &candidate_set : candidate_sets)
                    {
                        candidate_set.erase(c1);
                    }
                    candidate_edges[c1].clear();
                    for (set<int> &edges : candidate_edges)
                    {
                        edges.erase(c1);
                    }
                }
            }
        }
    }
}

static vector<int>
backtrack_candidate_space(
    vector<set<int>> &query_graph,
    vector<set<int>> &query_dag,
    int dag_root,
    vector<set<int>> &candidate_sets,
    vector<set<int>> &candidate_edges,
    vector<int> &mapping)
{
    set<int> frontier{dag_root};
    set<int> seen;
    set<int> mapped;
    vector<set<int>> parents(query_dag.size());
    for (unsigned int v = 0; v < query_dag.size(); v++)
    {
        for (int neighbor : query_dag[v])
        {
            parents[neighbor].insert(v);
        }
    }

    if (!backtrack_candidate_space_helper(
            query_graph,
            query_dag,
            candidate_sets,
            candidate_edges,
            mapping,
            frontier,
            seen,
            mapped,
            parents))
    {
        mapping.clear();
    }

    return mapping;
}

static bool
backtrack_candidate_space_helper(
    vector<set<int>> &query_graph,
    vector<set<int>> &query_dag,
    vector<set<int>> &candidate_sets,
    vector<set<int>> &candidate_edges,
    vector<int> &mapping,
    set<int> &frontier,
    set<int> &seen,
    set<int> &mapped,
    vector<set<int>> &parents)
{
    // Base Case
    if (frontier.empty())
    {
        return true;
    }

    // Currently ordered by int compare
    // TODO: Adaptive Matching Order
    // TODO: Pruning by Failing Sets
    for (int current : frontier)
    {
        // If no parents, aka root
        if (parents[current].empty())
        {
            // cout << "Root: " << current << endl;

            set<int> new_frontier = frontier;
            new_frontier.erase(current);
            for (int child : query_dag[current])
            {
                new_frontier.insert(child);
            }

            set<int> new_seen = seen;
            new_seen.insert(current);

            vector<int> new_mapping = mapping;

            set<int> new_mapped = mapped;

            for (int candidate : candidate_sets[current])
            {
                // cout << "Candidate: " << candidate << endl;

                new_mapping[current] = candidate;
                new_mapped.insert(candidate);

                if (backtrack_candidate_space_helper(
                        query_graph,
                        query_dag,
                        candidate_sets,
                        candidate_edges,
                        new_mapping,
                        new_frontier,
                        new_seen,
                        new_mapped,
                        parents))
                {
                    mapping = new_mapping;
                    return true;
                }

                new_mapping[current] = UNDEFINED_QUBIT;
                new_mapped.erase(candidate);
            }

            return false;
        }
        // If parents
        else
        {
            // cout << "Search: " << current << endl;
            // Extendable if parents are matched
            if (any_of(parents[current].begin(), parents[current].end(),
                       [&seen](int parent) { return seen.find(parent) == seen.end(); }))
            {
                // cout << current << " parents aren't mapped yet" << endl;
                continue;
            }
            // Candidates of current vertex =
            // Intersection of parents' mapped candidates' adjacency list
            set<int> candidates_list;
            for (int candidate : candidate_sets[current])
            {
                if (mapped.find(candidate) != mapped.end())
                {
                    continue;
                }
                if (all_of(parents[current].begin(), parents[current].end(),
                           [&candidate_edges, &mapping, candidate](int parent) {
                               return (candidate_edges[mapping[parent]].find(candidate) !=
                                       candidate_edges[mapping[parent]].end());
                           }))
                {
                    candidates_list.insert(candidate);
                }
            }

            if (candidates_list.empty())
            {
                // cout << current << " has no viable candidates" << endl;
                continue;
            }

            set<int> new_frontier = frontier;
            new_frontier.erase(current);
            for (int child : query_dag[current])
            {
                if (seen.find(child) == seen.end())
                {
                    new_frontier.insert(child);
                }
            }

            set<int> new_seen = seen;
            new_seen.insert(current);

            vector<int> new_mapping = mapping;

            set<int> new_mapped = mapped;

            for (int candidate : candidates_list)
            {
                // cout << "Candidate: " << candidate << endl;

                new_mapping[current] = candidate;
                new_mapped.insert(candidate);

                if (backtrack_candidate_space_helper(
                        query_graph,
                        query_dag,
                        candidate_sets,
                        candidate_edges,
                        new_mapping,
                        new_frontier,
                        new_seen,
                        new_mapped,
                        parents))
                {
                    mapping = new_mapping;
                    return true;
                }

                new_mapping[current] = UNDEFINED_QUBIT;
                new_mapped.erase(candidate);
            }

            return false;
        }
    }

    return false;
}

/**
 * Destroy all GateNodes in the Dependency Graph
 * @param firstGates Input: Set of First Candidates
 */
void destroyDependencyGraph(set<GateNode *> &firstGates)
{
    // BFS
    queue<GateNode *> search;
    set<GateNode *> seen;
    for (auto gate : firstGates)
    {
        search.push(gate);
        seen.insert(gate);
    }
    while (!search.empty())
    {
        GateNode *current = search.front();
        search.pop();

        // Search on Gate
        // If Gate is target only
        if (current->control == UNDEFINED_QUBIT)
        {
            if (current->targetChild != NULL && seen.find(current->targetChild) == seen.end())
            {
                seen.insert(current->targetChild);
                search.push(current->targetChild);
            }
        }
        // If Gate is target and control
        else
        {
            if (current->targetChild != NULL && seen.find(current->targetChild) == seen.end())
            {
                seen.insert(current->targetChild);
                search.push(current->targetChild);
            }
            if (current->controlChild != NULL && seen.find(current->controlChild) == seen.end())
            {
                seen.insert(current->controlChild);
                search.push(current->controlChild);
            }
        }

        // Delete GateNode
        delete current;
    }
}
