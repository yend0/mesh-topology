#ifndef PACKET_H
#define PACKET_H

#include "constants.h"
#include "common.h"

typedef enum
{
    PACKET_TYPE_MAC,
    PACKET_TYPE_APP
} packet_t;

typedef struct
{
    packet_t type;

    int network_graph[MAX_NODES][MAX_NODES];

    union
    {
        struct
        {
            uint8_t mac_sender;
            uint8_t mac_receiver;
            uint8_t ttl;
            uint8_t message_length;
            char message[MAX_MESSAGE_LENGTH];
            uint16_t crc;
        } mac_packet;
    };

} Packet;

Packet create_mac_packet(uint8_t sender, uint8_t receiver, uint8_t ttl, const char *message);

#endif // PACKET_H