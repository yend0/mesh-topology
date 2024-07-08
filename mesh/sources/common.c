#include "common.h"

uint16_t calculate_crc(const char *data, size_t length)
{
    uLong crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, (const Bytef *)data, length);
    return (uint16_t)crc;
}
