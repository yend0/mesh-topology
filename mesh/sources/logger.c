#include "logger.h"

pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

const char *get_message_type_string(const message_type type)
{
    switch (type)
    {
    case MSG_TYPE_DATA:
        return "DATA";
    case MSG_TYPE_COMMAND:
        return "COMMAND";
    case MSG_TYPE_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

void log_message(const char *creator, const message_type type,
                 const char *message_format, ...)
{
    FILE *logfile = fopen(LOG_FILE, "a");

    if (!logfile)
    {
        perror("Failed to open log file.");
        return;
    }

    pthread_mutex_lock(&log_mutex);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    fprintf(logfile, "[%s] [%s] [%s] ", creator, get_message_type_string(type), time_str);

    va_list args;
    va_start(args, message_format);
    vfprintf(logfile, message_format, args);
    va_end(args);

    fprintf(logfile, "\n");

    pthread_mutex_unlock(&log_mutex);

    fclose(logfile);
}