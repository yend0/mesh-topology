#ifndef COMMON_H
#define COMMON_H

#include "stdafx.h"

typedef enum
{
    MSG_TYPE_INFO,
    MSG_TYPE_COMMAND,
    MSG_TYPE_ERROR,
    MSG_TYPE_NOT_VALID_DATA

} message_type;

uint16_t calculate_crc(const char *data, const size_t length);

int compress_data(const char *input, size_t input_size, char *output, size_t *output_size);
int decompress_data(const char *input, size_t input_size, char *output, size_t *output_size);

#endif // COMMON_H
