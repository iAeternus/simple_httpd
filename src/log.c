#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include "log.h"

static int log_fd = STDERR_FILENO;

int log_init(const char* path) {
    if (!path) {
        log_fd = STDERR_FILENO;
        return 0;
    }

    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("open log file");
        return -1;
    }

    log_fd = fd;
    return 0;
}

void log_close() {
    if (log_fd != STDERR_FILENO) {
        close(log_fd);
        log_fd = STDERR_FILENO;
    }
}

static void log_message(const char* level, const char* fmt, va_list args) {
    time_t now;
    time(&now);
    struct tm* local_time = localtime(&now);
    pid_t pid = getpid();

    dprintf(log_fd, "[%04d-%02d-%02d %02d:%02d:%02d] [%5s] [pid=%d] ",
            local_time->tm_year + 1900,
            local_time->tm_mon + 1,
            local_time->tm_mday,
            local_time->tm_hour,
            local_time->tm_min,
            local_time->tm_sec,
            level,
            pid);

    vdprintf(log_fd, fmt, args);
    dprintf(log_fd, "\n");
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