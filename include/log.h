#ifndef LOG_H
#define LOG_H

void log_trace(const char* fmt, ...);
void log_debug(const char* fmt, ...);
void log_info(const char* fmt, ...);
void log_error(const char* fmt, ...);
void log_fatal(const char* fmt, ...);

#endif // LOG_H