#include <signal.h>
#include <sys/wait.h>

#include "graph.h"
#include "constants.h"
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

void send_command_to_node(Packet *packet)
{
    if (packet->type == PACKET_TYPE_MAC)
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

        int sent_bytes = sendto(client_socket, packet, sizeof(Packet), 0, (struct sockaddr *)&node_address, sizeof(node_address));

        if (sent_bytes == -1)
        {
            log_message("SERVER", MSG_TYPE_ERROR, "sendto() failed");
        }
    }
}

void create_and_send_command(const int src, const int dest, int graph[MAX_NODES][MAX_NODES], const int size_graph, const char *message)
{
    Packet packet = create_mac_packet(src, dest, TTL_LIMIT, message);

    memcpy(packet.network_graph, graph, size_graph * size_graph * sizeof(int));

    send_command_to_node(&packet);
}

int main()
{
    signal(SIGINT, handle_signal);

    int graph[MAX_NODES][MAX_NODES];
    int matrix_size = 10;

    initialize_graph(MAX_NODES, graph);
    add_edges(matrix_size, graph);

    int distances[MAX_NODES];
    int predecessors[MAX_NODES];

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
        printf("Enter command (send <source_node> <dest_node> <message>): ");
        fgets(command, sizeof(command), stdin);

        int src_node, dest_node;
        char message[MAX_MESSAGE_LENGTH];
        if (sscanf(command, "send %d %d %[^\n]", &src_node, &dest_node, message) == 3)
        {
            create_and_send_command(src_node, dest_node, graph, MAX_NODES, message);
        }
        else
        {
            printf("Invalid command format.\n");
        }
    }

    handle_signal(SIGINT);
    return EXIT_SUCCESS;
}
