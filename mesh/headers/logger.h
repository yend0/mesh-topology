#ifndef LOGGER_H
#define LOGGER_H

#include "stdafx.h"
#include "common.h"

#define LOG_FILE "logs.log"

const char *get_message_type_string(const message_type type);
void log_message(const char *creator, const message_type type,
                 const char *message_format, ...);

#endif // LOGGER_H