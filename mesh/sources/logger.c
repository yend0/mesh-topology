#include "logger.h"

/**
 * @brief Converts the message type to a string for output.
 *
 * The function takes a message type and returns its string representation,
 * which makes it convenient to display message types in logs.
 *
 * @param type The message type (message_type) to be converted.
 * @return String representation of the message type.
 */
const char *get_message_type_string(const message_type type)
{
    switch (type)
    {
    case MSG_TYPE_INFO:
        return "INFORMATION";
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

/**
 * @brief Writes the message to a log file.
 *
 * The function opens a log file, locks it, writes a message into it with the creator, message type and timestamp, and then closes the file.
 * specifying the creator, message type and timestamp, and then closes the file.
 * Error handling is performed for file opening and locking.
 *
 * @param creator The name or identifier of the message creator.
 * @param type The type of message (message_type) to be logged.
 * @param message_format A message format that supports a variable number of arguments (printf-style).
 * @param ... Variable number of arguments to format the message.
 */
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