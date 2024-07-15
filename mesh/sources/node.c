#include <errno.h>

#include "constants.h"
#include "logger.h"
#include "packet.h"

int node_id;
int client_socket;

int find_next_hop(int current_node, int destination_node, int graph[MAX_NODES][MAX_NODES], int size)
{
    int distances[MAX_NODES];
    bool visited[MAX_NODES];
    int predecessors[MAX_NODES];

    for (int i = 0; i < size; i++)
    {
        distances[i] = INF;
        visited[i] = false;
        predecessors[i] = -1;
    }

    distances[current_node] = 0;

    for (int i = 0; i < size; i++)
    {
        int min_distance = INF;
        int u = -1;
        for (int j = 0; j < size; j++)
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

        for (int v = 0; v < size; v++)
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

    if (predecessors[destination_node] == -1)
        return -1;

    int next_hop = destination_node;
    while (predecessors[next_hop] != current_node)
    {
        next_hop = predecessors[next_hop];
    }

    return next_hop;
}

void broadcast_signal(packet_t *packet)
{
    static int processed_broadcasts[MAX_NODES] = {0};

    if (processed_broadcasts[packet->mac_packet.mac_sender])
    {
        log_message("CLIENT", MSG_TYPE_NOT_VALID_DATA, "Duplicate broadcast packet received, packet dropped");
        return;
    }

    processed_broadcasts[packet->mac_packet.mac_sender] = 1;

    for (int i = 0; i < MAX_NODES; i++)
    {
        if (i != node_id && packet->network_graph[node_id][i] != INF && packet->network_graph[node_id][i] <= 3)
        {
            struct sockaddr_in node_address;
            node_address.sin_family = AF_INET;
            node_address.sin_port = htons(CLIENT_BASE_PORT + i);
            node_address.sin_addr.s_addr = INADDR_ANY;

            char compressed_data[sizeof(packet_t)];
            size_t compressed_size = sizeof(packet_t);

            int compress_result = compress_data((char *)packet, sizeof(packet_t), compressed_data, &compressed_size);

            if (compress_result)
            {
                log_message("CLIENT", MSG_TYPE_ERROR, "Compression failed");
                continue;
            }

            int sent_bytes = sendto(client_socket, compressed_data, compressed_size, 0, (struct sockaddr *)&node_address, sizeof(node_address));

            if (sent_bytes == -1)
            {
                log_message("CLIENT", MSG_TYPE_ERROR, "Broadcast sendto() failed to node %d", i);
            }
            else
            {
                log_message("CLIENT", MSG_TYPE_INFO, "Broadcast packet sent to node %d", i);
            }
        }
    }
}

void send_packet(packet_t *packet)
{
    if (packet->mac_packet.ttl == 0)
    {
        log_message("CLIENT", MSG_TYPE_NOT_VALID_DATA, "TTL expired, packet dropped");
        return;
    }

    packet->mac_packet.ttl--;

    if (packet->mac_packet.mac_receiver == BROADCAST_NODE)
    {
        broadcast_signal(packet);
        return;
    }

    int next_node = find_next_hop(node_id, packet->mac_packet.mac_receiver, packet->network_graph, MAX_NODES);

    if (next_node == -1)
    {
        log_message("CLIENT", MSG_TYPE_ERROR, "Next hop not found, packet dropped");
        return;
    }

    struct sockaddr_in node_address;
    node_address.sin_family = AF_INET;
    node_address.sin_port = htons(CLIENT_BASE_PORT + next_node);
    node_address.sin_addr.s_addr = INADDR_ANY;

    char compressed_data[sizeof(packet_t)];
    size_t compressed_size = sizeof(packet_t);

    int compress_result = compress_data((char *)packet, sizeof(packet_t), compressed_data, &compressed_size);

    if (compress_result)
    {
        log_message("CLIENT", MSG_TYPE_ERROR, "Compression failed");
        return;
    }

    int sent_bytes = sendto(client_socket, compressed_data, compressed_size, 0, (struct sockaddr *)&node_address, sizeof(node_address));

    if (sent_bytes == -1)
    {
        log_message("CLIENT", MSG_TYPE_ERROR, "sendto() failed");
    }
    else
    {
        log_message("CLIENT", MSG_TYPE_INFO, "Sent MAC packet from %d to node %d, ttl %d", node_id, next_node, packet->mac_packet.ttl);
    }
}

void handle_signal(int sig)
{
    close(client_socket);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <node_id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    node_id = atoi(argv[1]);

    signal(SIGTERM, handle_signal);

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (client_socket == -1)
    {
        log_message("CLIENT", MSG_TYPE_ERROR, "Error in calling socket()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in node_address;
    node_address.sin_family = AF_INET;
    node_address.sin_port = htons(CLIENT_BASE_PORT + node_id);
    node_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(client_socket, (struct sockaddr *)&node_address, sizeof(node_address)) == -1)
    {
        log_message("CLIENT", MSG_TYPE_ERROR, "Error in calling bind()");
        exit(EXIT_FAILURE);
    }

    int flags = fcntl(client_socket, F_GETFL, 0);
    fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);

    char buffer[sizeof(packet_t)];
    char decompressed_data[sizeof(packet_t)];
    size_t decompressed_size = sizeof(decompressed_data);

    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    while (1)
    {
        int recv_bytes = recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &addr_len);

        if (recv_bytes == -1)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                usleep(100000);
                continue;
            }
            else
            {
                log_message("CLIENT", MSG_TYPE_ERROR, "recvfrom failed");
                continue;
            }
        }

        if (recv_bytes > 0)
        {
            int decompress_result = decompress_data(buffer, sizeof(packet_t), decompressed_data, &decompressed_size);

            if (decompress_result != Z_OK)
            {
                log_message("CLIENT", MSG_TYPE_ERROR, "Decompression failed");
                continue;
            }

            packet_t *packet = (packet_t *)decompressed_data;

            uint16_t app_crc = calculate_crc((const char *)&packet->mac_packet.app_packet.message, sizeof(packet->mac_packet.app_packet.message_length));

            uint16_t mac_crc = calculate_crc((const char *)&packet->mac_packet.app_packet, sizeof(packet->mac_packet.app_packet));

            if (packet->mac_packet.app_packet.crc == app_crc && packet->mac_packet.crc == mac_crc)
            {

                if (packet->mac_packet.mac_receiver == node_id)
                {
                    log_message("CLIENT", MSG_TYPE_INFO, "Message for this node: %s", packet->mac_packet.app_packet.message);
                }
                else if (packet->mac_packet.ttl > 0)
                {
                    send_packet(packet);
                }
                else
                {
                    log_message("CLIENT", MSG_TYPE_NOT_VALID_DATA, "TTL expired, packet dropped");
                }
            }
            else
            {
                log_message("CLIENT", MSG_TYPE_ERROR, "Received packet with invalid CRC. Calculated MAC CRC: %u. Calculated APP CRC: %u", mac_crc, app_crc);
            }
        }
    }

    return EXIT_SUCCESS;
}
