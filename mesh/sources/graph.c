#include <stdio.h>
#include <string.h>

#include "graph.h"

int graph[MAX_NODES][MAX_NODES];
int distances[MAX_NODES];
bool visited[MAX_NODES];
int predecessors[MAX_NODES];

void initialize_graph(const int size)
{
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            if (i == j)
                graph[i][j] = 0;
            else
                graph[i][j] = INF;
        }
    }
}

void add_edge(const int u, const int v, const int weight)
{
    graph[u][v] = weight;
    graph[v][u] = weight;
}

void dijkstra(const int start_vertex, const int size)
{
    for (int i = 0; i < size; ++i)
    {
        distances[i] = INF;
        visited[i] = false;
        predecessors[i] = -1;
    }

    distances[start_vertex] = 0;

    for (int i = 0; i < size; ++i)
    {
        int min_distance = INF;
        int u = -1;

        for (int j = 0; j < size; ++j)
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

        for (int v = 0; v < size; ++v)
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

void get_path(const int start_vertex, const int end_vertex,
              char *path, const char *vertex_data[MAX_NODES])
{
    int current = end_vertex;
    char temp_path[1024] = "";
    char buffer[100];

    while (current != -1)
    {
        snprintf(buffer, sizeof(buffer), "%s", vertex_data[current]);
        strcat(temp_path, buffer);

        if (current == start_vertex)
        {
            break;
        }

        current = predecessors[current];

        if (current != -1)
        {
            strcat(temp_path, "->");
        }
    }

    int len = strlen(temp_path);
    for (int i = 0; i < len / 2; ++i)
    {
        char tmp = temp_path[i];
        temp_path[i] = temp_path[len - i - 1];
        temp_path[len - i - 1] = tmp;
    }

    strcpy(path, temp_path);
}
