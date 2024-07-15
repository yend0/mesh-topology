#include <signal.h>
#include <sys/wait.h>

#include "user_interface.h"

pid_t node_pids[MAX_NODES];
int server_socket;
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
        log_message("SERVER", MSG_TYPE_INFO, "Node %d stopped", node_id);
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
    close(server_socket);
    exit(EXIT_SUCCESS);
}

void handle_user_commands(int graph[MAX_NODES][MAX_NODES], int client_socket)
{
    while (1)
    {
        char command[256];
        printf("Enter command: ");
        fgets(command, sizeof(command), stdin);

        int src_node, dest_node, node_id;
        char message[MAX_MESSAGE_LENGTH];

        if (sscanf(command, "send %d %d %[^\n]", &src_node, &dest_node, message) == 3)
        {
            create_and_send_message(src_node, dest_node, graph, MAX_NODES, message, client_socket);
        }
        else if (sscanf(command, "broadcast %d %[^\n]", &src_node, message) == 2)
        {
            create_and_send_broadcast(src_node, graph, MAX_NODES, message, client_socket);
        }
        else if (sscanf(command, "stop %d", &node_id) == 1)
        {
            stop_node(node_id);
            remove_node(node_id, graph);
        }
        else if (strncmp(command, "help", 4) == 0)
        {
            print_help();
        }
        else
        {
            printf("Invalid command format. Type 'help' for a list of commands.\n");
        }
    }
}

int main()
{
    signal(SIGINT, handle_signal);

    int graph[MAX_NODES][MAX_NODES];
    int matrix_size = 10;

    int distances[MAX_NODES];
    int predecessors[MAX_NODES];

    initialize_graph(MAX_NODES, graph);
    add_edges(matrix_size, graph);

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1)
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

    handle_user_commands(graph, server_socket);

    handle_signal(SIGINT);
    return EXIT_SUCCESS;
}
