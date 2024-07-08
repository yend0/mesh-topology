#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "constants.h"
#include "logger.h"
#include "packet.h"

int node_id;
int client_socket;

void send_mac_packet(Packet *packet)
{
    if (packet->mac_packet.ttl <= 0)
    {
        log_message("CLIENT", MSG_TYPE_ERROR, "TTL expired, packet dropped");
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
        log_message("CLIENT", MSG_TYPE_ERROR, "sendto() failed");
    }
    else
    {
        log_message("CLIENT", MSG_TYPE_DATA, "Sent MAC packet to node %d, ttl %d", packet->mac_packet.mac_receiver, packet->mac_packet.ttl);
    }
}

void receive_packet()
{
    Packet packet;
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    while (1)
    {
        int recv_bytes = recvfrom(client_socket, &packet, sizeof(Packet), 0, (struct sockaddr *)&sender_addr, &addr_len);

        if (recv_bytes == -1)
        {
            log_message("CLIENT", MSG_TYPE_ERROR, "recvfrom failed");
            continue;
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

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, (void *)receive_packet, NULL);

    pthread_join(recv_thread, NULL);

    return EXIT_SUCCESS;
}
