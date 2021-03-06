#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define LOG_COLOR_RED       "\x1b[31m"
#define LOG_COLOR_GREEN     "\x1b[32m"
#define LOG_COLOR_YELLOW    "\x1b[33m"
#define LOG_COLOR_BLUE      "\x1b[34m"
#define LOG_COLOR_MAGENTA   "\x1b[35m"
#define LOG_COLOR_CYAN      "\x1b[36m"
#define LOG_COLOR_RESET     "\x1b[0m"

typedef enum LogMsgType {

	NO_TYPE = 0,

    ERROR = 1,
    WARNING,
    SUCCESS,
    DEBUG_MSG,
    TEST,

    REQ = 10,
    PACKET,
    FILE_REQ,
    GAME,
    PLAYER,

    SERVER = 100,
    CLIENT

} LogMsgType;

extern void logMsg (FILE *__restrict __stream, LogMsgType firstType, LogMsgType secondType,
    const char *msg);

#endif