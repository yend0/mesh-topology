#ifndef PACKET_H
#define PACKET_H

#include "constants.h"
#include "common.h"

typedef enum
{
    PACKET_TYPE_MAC,
    PACKET_TYPE_APP
} packet_type;

typedef struct
{
    packet_type type;

    int route[MAX_NODES];
    int route_length;

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

Packet create_mac_packet(uint8_t sender, uint8_t receiver, uint8_t ttl, const char *message)
{
    Packet packet;
    packet.type = PACKET_TYPE_MAC;
    packet.mac_packet.mac_sender = sender;
    packet.mac_packet.mac_receiver = receiver;
    packet.mac_packet.ttl = ttl;
    packet.mac_packet.message_length = strlen(message);
    strncpy(packet.mac_packet.message, message, MAX_MESSAGE_LENGTH);
    packet.mac_packet.crc = calculate_crc((char *)&packet.mac_packet.message, sizeof(packet.mac_packet.message_length));

    return packet;
}

#endif // PACKET_H