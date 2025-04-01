#ifndef PACKET_H
#define PACKET_H

#include "constants.h"
#include "common.h"

typedef struct
{
    uint8_t app_sender;
    uint8_t app_receiver;
    uint16_t message_id;
    uint8_t message_length;
    char message[MAX_MESSAGE_LENGTH];
    uint16_t crc;
} app_packet_t;

typedef struct
{
    uint8_t mac_sender;
    uint8_t mac_receiver;
    uint8_t ttl;
    uint8_t message_length;
    app_packet_t app_packet;
    uint16_t crc;
} mac_packet_t;

typedef struct
{
    int network_graph[MAX_NODES][MAX_NODES];
    union
    {
        mac_packet_t mac_packet;
        app_packet_t app_packet;
    };
} packet_t;

packet_t create_packet(uint8_t mac_sender, uint8_t mac_receiver, uint8_t ttl,
                       uint8_t app_sender, uint8_t app_receiver, const char *message);
#endif // PACKET_H