#include "packet.h"
#include "globals.h"

packet_t create_packet(uint8_t mac_sender, uint8_t mac_receiver, uint8_t ttl,
                       uint8_t app_sender, uint8_t app_receiver, const char *message)
{
    packet_t packet;
    memset(&packet, 0, sizeof(packet_t));

    packet.mac_packet.mac_sender = mac_sender;
    packet.mac_packet.mac_receiver = mac_receiver;

    packet.mac_packet.ttl = ttl;

    packet.mac_packet.app_packet.app_sender = app_sender;
    packet.mac_packet.app_packet.app_receiver = app_receiver;
    packet.mac_packet.app_packet.message_id = atomic_fetch_add(&message_id, 1);
    packet.mac_packet.app_packet.message_length = strlen(message);

    strncpy(packet.mac_packet.app_packet.message, message, MAX_MESSAGE_LENGTH);

    packet.mac_packet.app_packet.crc = calculate_crc((const char *)&packet.mac_packet.app_packet.message, sizeof(packet.mac_packet.app_packet.message_length));

    packet.mac_packet.crc = calculate_crc((const char *)&packet.mac_packet.app_packet, sizeof(packet.mac_packet.app_packet));

    packet.mac_packet.message_length = sizeof(app_packet_t) - MAX_MESSAGE_LENGTH + packet.mac_packet.app_packet.message_length + 2;

    return packet;
}