#include "logger.h"

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
    case MSG_TYPE_NOT_VALID_DATA:
        return "NOT VALID DATA";
    default:
        return "UNKNOWN";
    }
}

void log_message(const char *creator, const message_type type, const char *message_format, ...)
{
    FILE *logfile = fopen(LOG_FILE, "a");
    if (!logfile)
    {
        perror("Failed to open log file.");
        return;
    }

    int fd = fileno(logfile);
    if (flock(fd, LOCK_EX) == -1)
    {
        perror("Failed to lock log file.");
        fclose(logfile);
        return;
    }

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

    fflush(logfile);
    flock(fd, LOCK_UN);
    fclose(logfile);
}