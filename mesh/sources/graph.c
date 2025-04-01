#include <stdio.h>
#include <string.h>

#include "graph.h"

/**
 * @brief Adds edges to the graph as an adjacency matrix.
 *
 * The function adds edges between nodes in a matrix representing the graph.
 * It creates edges between the current node and its right, bottom, and
 * diagonal neighbors (bottom right and bottom left).
 *
 * @param matrix_size Matrix size (number of nodes in row and column).
 * @param graph The adjacency matrix representing the graph.
 */
void add_edges(int matrix_size, int graph[MAX_NODES][MAX_NODES])
{
    for (int row = 0; row < matrix_size; row++)
    {
        for (int col = 0; col < matrix_size; col++)
        {
            int node = row * matrix_size + col;

            // Right neighbor
            if (col + 1 < matrix_size)
            {
                int right_neighbor = row * matrix_size + (col + 1);
                add_edge(node, right_neighbor, 1, graph);
            }

            // Down neighbor
            if (row + 1 < matrix_size)
            {
                int down_neighbor = (row + 1) * matrix_size + col;
                add_edge(node, down_neighbor, 1, graph);
            }

            // Bottom-right diagonal neighbor
            if (row + 1 < matrix_size && col + 1 < matrix_size)
            {
                int bottom_right_neighbor = (row + 1) * matrix_size + (col + 1);
                add_edge(node, bottom_right_neighbor, 1, graph);
            }

            // Bottom-left diagonal neighbor
            if (row + 1 < matrix_size && col - 1 >= 0)
            {
                int bottom_left_neighbor = (row + 1) * matrix_size + (col - 1);
                add_edge(node, bottom_left_neighbor, 1, graph);
            }
        }
    }
}

/**
 * @brief Initializes the graph by filling it with INF values.
 *
 * Function sets the weights of edges between nodes. For nodes that reference themselves,
 * is set to weight 0, for all others - INF (infinity).
 *
 * @param size Size of the graph.
 * @param graph The adjacency matrix for initialization.
 */
void initialize_graph(int size, int graph[MAX_NODES][MAX_NODES])
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (i == j)
                graph[i][j] = 0;
            else
                graph[i][j] = INF;
        }
    }
}

/**
 * @brief Adds an edge between two nodes in a graph.
 *
 * The function sets the weight of edges between nodes u and v in the adjacency matrix.
 * The edges are added in both directions so that the graph is undirected.
 *
 * @param u First node.
 * @param v Second node.
 * @param weight Weight of the edges.
 * @param graph The adjacency matrix representing the graph.
 */
void add_edge(int u, int v, int weight, int graph[MAX_NODES][MAX_NODES])
{
    graph[u][v] = weight;
    graph[v][u] = weight;
}

/**
 * @brief Removes a node from the graph.
 *
 * The function sets the weight of edges coming from the node being removed to INF,
 * thereby excluding it from the graph.
 *
 * @param node_id The node to be deleted.
 * @param graph The adjacency matrix representing the graph.
 */
void remove_node(int node_id, int graph[MAX_NODES][MAX_NODES])
{
    for (int i = 0; i < MAX_NODES; i++)
    {
        graph[node_id][i] = INF;
        graph[i][node_id] = INF;
    }
}

/**
 * @brief Implements Dijkstra's algorithm for finding shortest paths.
 *
 * The function calculates the shortest distances from the initial node to all other nodes of the graph.
 * nodes of the graph using Dijkstra's algorithm. The predecessors of the nodes are also stored.
 *
 * @param graph The adjacency matrix representing the graph.
 * @param start_node The starting node for computing shortest paths.
 * @param num_nodes Total number of nodes in the graph.
 * @param distances Array to store the shortest distances from the start node.
 * @param predecessors An array to store the predecessors of nodes.
 */
void dijkstra(int graph[MAX_NODES][MAX_NODES], int start_node, int num_nodes, int distances[MAX_NODES], int predecessors[MAX_NODES])
{
    bool visited[MAX_NODES];

    for (int i = 0; i < num_nodes; i++)
    {
        distances[i] = INF;
        visited[i] = false;
        predecessors[i] = -1;
    }

    distances[start_node] = 0;

    for (int i = 0; i < num_nodes; i++)
    {
        int min_distance = INF;
        int u = -1;

        for (int j = 0; j < num_nodes; j++)
        {
            if (!visited[j] && distances[j] < min_distance)
            {
                min_distance = distances[j];
                u = j;
            }
        }

        if (u == -1)
            break;

        visited[u] = true;

        for (int v = 0; v < num_nodes; v++)
        {
            if (graph[u][v] != INF && !visited[v])
            {
                int alt = distances[u] + graph[u][v];
                if (alt < distances[v])
                {
                    distances[v] = alt;
                    predecessors[v] = u;
                }
            }
        }
    }
}

/**
 * @brief Recursively outputs the path from the given node to the starting node.
 *
 * The function uses an array of predecessors to trace the path.
 * If the predecessor of a node is -1, it means it is the starting node.
 * Otherwise, the function recursively calls itself for the node's predecessor,
 * then outputs the current node.
 *
 * @param node The node for which you want to output the path.
 * @param predecessors An array of predecessors containing the path information.
 */
void print_path(int node, int predecessors[MAX_NODES])
{
    if (predecessors[node] == -1)
    {
        printf("%d", node);
        return;
    }
    print_path(predecessors[node], predecessors);
    printf(" -> %d", node);
}

/**
 * @brief Outputs paths from the starting node to all other nodes in the graph.
 *
 * The function traverses all nodes and outputs the path from the starting node to each node,
 * using an array of predecessors. If no path exists, the message "No path" is output.
 *
 * @param start_node The node from which paths start.
 * @param num_nodes The total number of nodes in the graph.
 * @param predecessors Array of node predecessors.
 */
void print_paths(int start_node, int num_nodes, int predecessors[MAX_NODES])
{
    for (int i = 0; i < num_nodes; i++)
    {
        if (i != start_node)
        {
            printf("Path from %d to %d: ", start_node, i);
            if (predecessors[i] == -1)
            {
                printf("No path\n");
            }
            else
            {
                print_path(i, predecessors);
                printf("\n");
            }
        }
    }
}
