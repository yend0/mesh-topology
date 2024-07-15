#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "graph.h"
#include "constants.h"
#include "logger.h"
#include "packet.h"

void send_command_to_node(packet_t *packet, int client_socket);
void create_and_send_message(const int src, const int dest, int graph[MAX_NODES][MAX_NODES], const int size_graph, const char *message, int client_socket);
void create_and_send_broadcast(const int src, int graph[MAX_NODES][MAX_NODES], const int size_graph, const char *message, int client_socket);
void print_help();

#endif // USER_INTERFACE_H