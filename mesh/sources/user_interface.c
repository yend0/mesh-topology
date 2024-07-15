#include "user_interface.h"

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

void create_and_send_message(const int src, const int dest, int graph[MAX_NODES][MAX_NODES], const int size_graph, const char *message, int client_socket)
{
    packet_t packet = create_packet(src, dest, TTL_LIMIT, src, dest, message);

    memcpy(packet.network_graph, graph, size_graph * size_graph * sizeof(int));

    send_command_to_node(&packet, client_socket);
}

void create_and_send_broadcast(const int src, int graph[MAX_NODES][MAX_NODES], const int size_graph, const char *message, int client_socket)
{
    packet_t packet = create_packet(src, BROADCAST_NODE, TTL_LIMIT, src, BROADCAST_NODE, message);

    memcpy(packet.network_graph, graph, size_graph * size_graph * sizeof(int));

    send_command_to_node(&packet, client_socket);
}

void print_help()
{
    printf("Available commands:\n");
    printf("  send <source_node> <dest_node> <message>  - Send a message from source_node to dest_node\n");
    printf("  broadcast <source_node> <message>         - Broadcast a message from source_node to all nodes in range\n");
    printf("  stop <node_id>                            - Stops the node\n");
    printf("  help                                      - Display this help message\n");
    printf("  Ctrl+C                                    - Exit the server program\n");
}
