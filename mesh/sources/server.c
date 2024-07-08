#include <signal.h>
#include <sys/wait.h>

#include "graph.h"
#include "logger.h"
#include "packet.h"

pid_t node_pids[MAX_NODES];
int client_socket;
struct sockaddr_in server_address;

void start_node(int node_id)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        char node_id_str[4];
        snprintf(node_id_str, 4, "%d", node_id);
        execl("./app-node", "app-node", node_id_str, NULL);
        log_message("SERVER", MSG_TYPE_ERROR, "execl failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        node_pids[node_id] = pid;
    }
    else
    {
        log_message("SERVER", MSG_TYPE_ERROR, "fork failed");
    }
}

void stop_node(int node_id)
{
    kill(node_pids[node_id], SIGTERM);
    waitpid(node_pids[node_id], NULL, 0);
}

void handle_signal(int sig)
{
    for (int i = 0; i < MAX_NODES; ++i)
    {
        if (node_pids[i] > 0)
        {
            stop_node(i);
        }
    }
    close(client_socket);
    exit(EXIT_SUCCESS);
}

void send_packet_to_next_node(Packet *packet)
{
    if (packet->type == PACKET_TYPE_MAC)
    {
        if (packet->mac_packet.ttl <= 0)
        {
            log_message("SERVER", MSG_TYPE_ERROR, "TTL expired, packet dropped");
            return;
        }

        int next_node = packet->route[packet->route_length - packet->mac_packet.ttl];
        packet->mac_packet.ttl--;

        struct sockaddr_in node_address;
        node_address.sin_family = AF_INET;
        node_address.sin_port = htons(CLIENT_BASE_PORT + next_node);
        node_address.sin_addr.s_addr = INADDR_ANY;

        int sent_bytes = sendto(client_socket, packet, sizeof(Packet), 0, (struct sockaddr *)&node_address, sizeof(node_address));

        if (sent_bytes == -1)
        {
            log_message("SERVER", MSG_TYPE_ERROR, "sendto() failed");
        }
        else
        {
            log_message("SERVER", MSG_TYPE_DATA, "Sent MAC packet to node %d, ttl %d", next_node, packet->mac_packet.ttl);
        }
    }
}

void create_and_send_mac_packet(int src, int dest, const char *message)
{
    dijkstra(src, MAX_NODES);
    int route[MAX_NODES];
    int route_length = 0;
    int u = dest;

    while (u != -1)
    {
        route[route_length++] = u;
        u = predecessors[u];
    }

    for (int i = 0; i < route_length / 2; ++i)
    {
        int temp = route[i];
        route[i] = route[route_length - i - 1];
        route[route_length - i - 1] = temp;
    }

    Packet packet = create_mac_packet(src, dest, route_length - 1, message);
    memcpy(packet.route, route, route_length * sizeof(int));
    packet.route_length = route_length;

    send_packet_to_next_node(&packet);
}

int main()
{
    signal(SIGINT, handle_signal);

    initialize_graph(MAX_NODES);

    add_edge(0, 1, 1);
    add_edge(1, 2, 1);
    add_edge(2, 3, 1);

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1)
    {
        log_message("SERVER", MSG_TYPE_ERROR, "Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    for (int i = 0; i < MAX_NODES; ++i)
    {
        start_node(i);
    }

    while (1)
    {
        char command[256];
        printf("Enter command (send <node_id> <message>): ");
        fgets(command, sizeof(command), stdin);

        int node_id;
        char message[MAX_MESSAGE_LENGTH];
        if (sscanf(command, "send %d %[^\n]", &node_id, message) == 2)
        {
            create_and_send_mac_packet(0, node_id, message);
        }
        else
        {
            printf("Invalid command format.\n");
        }
    }

    handle_signal(SIGINT);
    return EXIT_SUCCESS;
}
