#include "common.h"

/**
 * @brief Calculates the CRC (Cyclic Redundancy Check) for the data.
 *
 * The function uses the zlib library to calculate CRC32.
 * CRC is used to check the integrity of the data.
 *
 * @param data Pointer to the data for which to calculate the CRC.
 * @param length The length of the data.
 * @return 16-bit CRC value.
 */
uint16_t calculate_crc(const char *data, size_t length)
{
    uLong crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, (const Bytef *)data, length);
    return (uint16_t)crc;
}

/**
 * @brief Compresses input data using zlib.
 *
 * The function uses the deflate algorithm to compress data with the best compression.
 * The compressed data is written to the output buffer.
 *
 * @param input Pointer to the input data to be compressed.
 * @param input_size The size of the input data.
 * @param output Pointer to the buffer for compressed data.
 * @param output_size Pointer to the size of the output buffer. Upon completion, the function writes the size of the compressed data there.
 * @return Returns Z_OK on successful compression, otherwise Z_ERRNO.
 */
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

/**
 * @brief Decompresses input data using zlib.
 *
 * The function uses the inflate algorithm to decompress data.
 * Decompressed data is written to the output buffer.
 *
 * @param input Pointer to the input data to decompress.
 * @param input_size The size of the input data.
 * @param output Pointer to the decompressed data buffer.
 * @param output_size Pointer to the size of the output buffer. When finished, the function writes the size of the decompressed data there.
 * @return Returns Z_OK on successful decompression, otherwise Z_ERRNO.
 */
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
