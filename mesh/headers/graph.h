#ifndef GRAPH_H
#define GRAPH_H

#include "stdafx.h"
#include "constants.h"

extern int graph[MAX_NODES][MAX_NODES];
extern int distances[MAX_NODES];
extern bool visited[MAX_NODES];
extern int predecessors[MAX_NODES];

void initialize_graph(const int size);
void add_edge(const int u, const int v, const int weight);
void dijkstra(const int start_vertex, const int size);
void get_path(const int start_vertex, const int end_vertex,
              char *path, const char *vertex_data[MAX_NODES]);

#endif // GRAPH_H