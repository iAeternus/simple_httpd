#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include "log.h"

static void log_message(const char* level, const char* fmt, va_list args) {
    time_t now;
    time(&now);
    struct tm* local_time = localtime(&now);

    // 打印时间戳和日志级别
    dprintf(STDERR_FILENO, "[%04d-%02d-%02d %02d:%02d:%02d] [%5s] ",
            local_time->tm_year + 1900,
            local_time->tm_mon + 1,
            local_time->tm_mday,
            local_time->tm_hour,
            local_time->tm_min,
            local_time->tm_sec,
            level);

    // 打印日志内容
    vdprintf(STDERR_FILENO, fmt, args);
    dprintf(STDERR_FILENO, "\n");
}

void log_trace(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message("TRACE", fmt, args);
    va_end(args);
}

void log_debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message("DEBUG", fmt, args);
    va_end(args);
}

void log_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message("INFO", fmt, args);
    va_end(args);
}

void log_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message("ERROR", fmt, args);
    va_end(args);
}

void log_fatal(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message("FATAL", fmt, args);
    va_end(args);
}