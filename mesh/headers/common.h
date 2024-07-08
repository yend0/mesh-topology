#ifndef COMMON_H
#define COMMON_H

#include "stdafx.h"

typedef enum
{
    MSG_TYPE_DATA,
    MSG_TYPE_COMMAND,
    MSG_TYPE_ERROR

} message_type;

uint16_t calculate_crc(const char *data, const size_t length);

#endif // COMMON_H
