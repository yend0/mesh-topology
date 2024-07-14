#include <signal.h>
#include <sys/wait.h>

#include "graph.h"
#include "constants.h"
#include "logger.h"
#include "packet.h"

pid_t node_pids[MAX_NODES];
int client_socket;
struct sockaddr_in server_address;

void start_node(const int node_id)
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

void stop_node(const int node_id)
{
    if (node_pids[node_id] > 0)
    {
        kill(node_pids[node_id], SIGTERM);
        waitpid(node_pids[node_id], NULL, 0);
        node_pids[node_id] = 0;
    }
    else
    {
        log_message("SERVER", MSG_TYPE_ERROR, "Node %d is not running", node_id);
    }
}

void handle_signal(const int sig)
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

void send_command_to_node(packet_t *packet)
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

    int sent_bytes = sendto(client_socket, packet, sizeof(packet_t), 0, (struct sockaddr *)&node_address, sizeof(node_address));

    if (sent_bytes == -1)
    {
        log_message("SERVER", MSG_TYPE_ERROR, "sendto() failed");
    }
}

void create_and_send_command(const int src, const int dest, int graph[MAX_NODES][MAX_NODES],
                             const int size_graph, const char *message, const int message_id)
{
    packet_t packet = create_packet(src, dest, TTL_LIMIT, src, dest, message_id, message);

    memcpy(packet.network_graph, graph, size_graph * size_graph * sizeof(int));

    send_command_to_node(&packet);
}

int main()
{
    signal(SIGINT, handle_signal);

    int graph[MAX_NODES][MAX_NODES];
    int matrix_size = 10;

    int distances[MAX_NODES];
    int predecessors[MAX_NODES];

    int message_id = 0;

    initialize_graph(MAX_NODES, graph);
    add_edges(matrix_size, graph);

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
        printf("Enter command (send <source_node> <dest_node> <message> | stop <node_id>): ");
        fgets(command, sizeof(command), stdin);

        int src_node, dest_node, node_id;
        char message[MAX_MESSAGE_LENGTH];
        if (sscanf(command, "send %d %d %[^\n]", &src_node, &dest_node, message) == 3)
        {
            create_and_send_command(src_node, dest_node, graph, MAX_NODES, message, message_id);
            message_id++;
        }
        else if (sscanf(command, "stop %d", &node_id) == 1)
        {
            stop_node(node_id);
            remove_node(node_id, graph);
        }
        else
        {
            printf("Invalid command format.\n");
        }
    }

    handle_signal(SIGINT);
    return EXIT_SUCCESS;
}
