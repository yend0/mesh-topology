#include "stdafx.h"
#include "packet.h"

void test_compression_decompression()
{
    const char *message = "Hello";
    packet_t original_packet = create_packet(45, 67, 10, 45, 67, message);

    char compressed_data[sizeof(packet_t)];
    size_t compressed_size = sizeof(packet_t);

    char decompressed_data[sizeof(packet_t)];
    size_t decompressed_size = sizeof(packet_t);

    // Test compression
    int compress_result = compress_data((char *)&original_packet, sizeof(packet_t), compressed_data, &compressed_size);
    if (compress_result != Z_OK)
    {
        fprintf(stderr, "Compression failed\n");
        return;
    }
    printf("Compression successful. Original size: %zu, Compressed size: %zu\n", sizeof(packet_t), compressed_size);

    // Test decompression
    int decompress_result = decompress_data(compressed_data, compressed_size, decompressed_data, &decompressed_size);
    if (decompress_result != Z_OK)
    {
        fprintf(stderr, "Decompression failed\n");
        return;
    }
    printf("Decompression successful. Decompressed size: %zu\n", decompressed_size);

    // Verify the decompressed data matches the original packet
    packet_t *decompressed_packet = (packet_t *)decompressed_data;

    printf("original_packet:\n");
    printf("message: %s\n", original_packet.mac_packet.app_packet.message);
    printf("mac_receiver: %zu\n", original_packet.mac_packet.mac_receiver);
    printf("mac_sender: %zu\n", original_packet.mac_packet.mac_sender);
    printf("message_length: %zu\n", original_packet.mac_packet.message_length);
    printf("ttl: %zu\n", original_packet.mac_packet.ttl);
    printf("crc: %zu\n", original_packet.mac_packet.crc);

    printf("decompressed_packet:\n");
    printf("message: %s\n", decompressed_packet->mac_packet.app_packet.message);
    printf("mac_receiver: %zu\n", decompressed_packet->mac_packet.mac_receiver);
    printf("mac_sender: %zu\n", decompressed_packet->mac_packet.mac_sender);
    printf("message_length: %zu\n", decompressed_packet->mac_packet.message_length);
    printf("ttl: %zu\n", decompressed_packet->mac_packet.ttl);
    printf("crc: %zu\n", decompressed_packet->mac_packet.crc);

    if (memcmp(&original_packet, decompressed_packet, sizeof(packet_t)) == 0)
    {
        printf("Test passed: Decompressed packet matches original packet.\n");
    }
    else
    {
        printf("Test failed: Decompressed packet does not match original packet.\n");
    }
}

int main()
{
    test_compression_decompression();
    return 0;
}
