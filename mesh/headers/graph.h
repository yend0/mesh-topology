#ifndef GRAPH_H
#define GRAPH_H

#include "stdafx.h"
#include "constants.h"

void initialize_graph(int size, int graph[MAX_NODES][MAX_NODES]);
void add_edge(int u, int v, int weight, int graph[MAX_NODES][MAX_NODES]);
void add_edges(int matrix_size, int graph[MAX_NODES][MAX_NODES]);
void dijkstra(int graph[MAX_NODES][MAX_NODES], int start_node, int num_nodes, int distances[MAX_NODES], int predecessors[MAX_NODES]);
void print_path(int node, int predecessors[MAX_NODES]);
void print_paths(int start_node, int num_nodes, int predecessors[MAX_NODES]);

#endif // GRAPH_H