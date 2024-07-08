#include <stdio.h>
#include <string.h>

#include "graph.h"

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

void add_edge(int u, int v, int weight, int graph[MAX_NODES][MAX_NODES])
{
    graph[u][v] = weight;
    graph[v][u] = weight;
}

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
