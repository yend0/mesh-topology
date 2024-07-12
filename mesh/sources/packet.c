#include "packet.h"

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