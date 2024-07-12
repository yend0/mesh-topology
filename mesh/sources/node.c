#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

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

void send_mac_packet(Packet *packet)
{
    if (packet->mac_packet.ttl == 0)
    {
        log_message("CLIENT", MSG_TYPE_NOT_VALID_DATA, "TTL expired, packet dropped");
        return;
    }

    int next_node = find_next_hop(node_id, packet->mac_packet.mac_receiver, packet->network_graph, MAX_NODES);

    if (next_node == -1)
    {
        log_message("CLIENT", MSG_TYPE_ERROR, "Next hop not found, packet dropped");
        return;
    }
    packet->mac_packet.ttl--;

    struct sockaddr_in node_address;
    node_address.sin_family = AF_INET;
    node_address.sin_port = htons(CLIENT_BASE_PORT + next_node);
    node_address.sin_addr.s_addr = INADDR_ANY;

    int sent_bytes = sendto(client_socket, packet, sizeof(Packet), 0, (struct sockaddr *)&node_address, sizeof(node_address));
    if (sent_bytes == -1)
    {
        log_message("CLIENT", MSG_TYPE_ERROR, "sendto() failed");
    }
    else
    {
        log_message("CLIENT", MSG_TYPE_DATA, "Sent MAC packet from %d to node %d, ttl %d", node_id, next_node, packet->mac_packet.ttl);
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

    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    while (1)
    {
        Packet packet;
        int recv_bytes = recvfrom(client_socket, &packet, sizeof(Packet), 0, (struct sockaddr *)&sender_addr, &addr_len);

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

        if (recv_bytes == sizeof(Packet))
        {
            uint16_t current_crc = calculate_crc((char *)&packet.mac_packet.message, sizeof(packet.mac_packet.message_length));

            if (packet.mac_packet.crc == current_crc)
            {
                if (packet.type == PACKET_TYPE_MAC)
                {
                    if (packet.mac_packet.mac_receiver == node_id)
                    {
                        log_message("CLIENT", MSG_TYPE_DATA, "Message for this node: %s", packet.mac_packet.message);
                    }
                    else if (packet.mac_packet.ttl > 0)
                    {
                        send_mac_packet(&packet);
                    }
                    else
                    {
                        log_message("CLIENT", MSG_TYPE_ERROR, "TTL expired, packet dropped");
                    }
                }
            }
            else
            {
                log_message("CLIENT", MSG_TYPE_ERROR, "Received non-normal packet type");
            }
        }
    }

    return EXIT_SUCCESS;
}
