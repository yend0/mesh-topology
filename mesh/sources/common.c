#include "common.h"

uint16_t calculate_crc(const char *data, size_t length)
{
    uLong crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, (const Bytef *)data, length);
    return (uint16_t)crc;
}

int compress_data(const char *input, size_t input_size, char *output, size_t *output_size)
{
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    if (deflateInit(&stream, Z_BEST_COMPRESSION) != Z_OK)
    {
        return Z_ERRNO;
    }

    stream.next_in = (Bytef *)input;
    stream.avail_in = input_size;
    stream.next_out = (Bytef *)output;
    stream.avail_out = *output_size;

    int ret = deflate(&stream, Z_FINISH);
    if (ret != Z_STREAM_END)
    {
        deflateEnd(&stream);
        return Z_ERRNO;
    }

    *output_size = stream.total_out;
    deflateEnd(&stream);
    return Z_OK;
}

int decompress_data(const char *input, size_t input_size, char *output, size_t *output_size)
{
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    if (inflateInit(&stream) != Z_OK)
    {
        return Z_ERRNO;
    }

    stream.next_in = (Bytef *)input;
    stream.avail_in = input_size;
    stream.next_out = (Bytef *)output;
    stream.avail_out = *output_size;

    int ret = inflate(&stream, Z_FINISH);
    if (ret != Z_STREAM_END)
    {
        inflateEnd(&stream);
        return Z_ERRNO;
    }

    *output_size = stream.total_out;
    inflateEnd(&stream);
    return Z_OK;
}
