#include "packet.h"
#include "globals.h"

/**
 * @brief Creates a data packet to send over the network.
 *
 * The function initializes the packet structure, fills it with fields,
 * such as sender, receiver, time to live (TTL), message ID, and the message itself.
 * The checksums (CRC) for the packet and its application are then calculated.
 *
 * @param mac_sender The MAC address of the sending node.
 * @param mac_receiver The MAC address of the destination node.
 * @param ttl Packet lifetime, which determines the number of hops in the network.
 * @param app_sender Identifier of the application sending the message.
 * @param app_receiver The identifier of the application receiving the message.
 * @param message The message to be sent in the packet.
 * @return Returns the generated packet of type packet_t.
 */
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