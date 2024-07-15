#include "user_interface.h"

/**
 * @brief Sends a command to the node.
 *
 * The function checks the TTL value of the packet. If TTL is 0, the packet is rejected and a message is logged.
 * a message is written to the log. Otherwise, the TTL is decremented by 1.
 * Next, the node address is created and the packet is compressed for sending. If the compression
 * fails, an error message is logged. Otherwise.
 * The packet is sent over the socket.
 *
 * @param packet Pointer to the packet to be sent.
 * @param client_socket The client socket to send the packet.
 */
void send_command_to_node(packet_t *packet, int client_socket)
{
    if (packet->mac_packet.ttl == 0)
    {
        log_message("SERVER", MSG_TYPE_NOT_VALID_DATA, "TTL expired, packet dropped");
        return;
    }

    packet->mac_packet.ttl--;

    struct sockaddr_in node_address;
    node_address.sin_family = AF_INET;
    node_address.sin_port = htons(CLIENT_BASE_PORT + packet->mac_packet.mac_sender);
    node_address.sin_addr.s_addr = INADDR_ANY;

    char compressed_data[sizeof(packet_t)];
    size_t compressed_size = sizeof(packet_t);

    int compress_result = compress_data((char *)packet, sizeof(packet_t), compressed_data, &compressed_size);

    if (compress_result)
    {
        log_message("SERVER", MSG_TYPE_ERROR, "Compression failed");
        return;
    }

    int sent_bytes = sendto(client_socket, compressed_data, compressed_size, 0, (struct sockaddr *)&node_address, sizeof(node_address));

    if (sent_bytes == -1)
    {
        log_message("SERVER", MSG_TYPE_ERROR, "sendto() failed");
    }
}

/**
 * @brief Creates and sends a message from one node to another.
 *
 * The function creates a packet with the given parameters, including the network graph,
 * and sends it to a node. The graph is copied into the packet before sending.
 *
 * @param src Source node sending the message.
 * @param dest The destination node to which the message is sent.
 * @param graph The adjacency matrix of the network graph.
 * @param size_graph The size of the graph (number of nodes).
 * @param message The message to be sent.
 * @param client_socket The client socket to send the packet.
 */
void create_and_send_message(const int src, const int dest, int graph[MAX_NODES][MAX_NODES], const int size_graph, const char *message, int client_socket)
{
    packet_t packet = create_packet(src, dest, TTL_LIMIT, src, dest, message);

    memcpy(packet.network_graph, graph, size_graph * size_graph * sizeof(int));

    send_command_to_node(&packet, client_socket);
}

/**
 * @brief Creates and sends a broadcast message to the specified node.
 *
 * The function creates a packet with the given parameters, including the network graph,
 * and sends it to a node. The graph is copied into the packet before sending.
 *
 * @param src Source node sending the message.
 * @param graph The adjacency matrix of the network graph.
 * @param size_graph The size of the graph (number of nodes).
 * @param message The message to be sent.
 * @param client_socket The client socket to send the packet.
 */
void create_and_send_broadcast(const int src, int graph[MAX_NODES][MAX_NODES], const int size_graph, const char *message, int client_socket)
{
    packet_t packet = create_packet(src, BROADCAST_NODE, TTL_LIMIT, src, BROADCAST_NODE, message);

    memcpy(packet.network_graph, graph, size_graph * size_graph * sizeof(int));

    send_command_to_node(&packet, client_socket);
}

/**
 * @brief Outputs a list of console commands.
 *
 * Function outputs a list of commands available to the user.
 *
 */
void print_help()
{
    printf("Available commands:\n");
    printf("  send <source_node> <dest_node> <message>  - Send a message from source_node to dest_node\n");
    printf("  broadcast <source_node> <message>         - Broadcast a message from source_node to all nodes in range\n");
    printf("  stop <node_id>                            - Stops the node\n");
    printf("  help                                      - Display this help message\n");
    printf("  Ctrl+C                                    - Exit the server program\n");
}
