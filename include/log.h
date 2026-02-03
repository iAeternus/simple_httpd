#ifndef LOG_H
#define LOG_H

/**
 * @brief 初始化log
 * @param path 传 NULL 则写入 stderr
 */
int log_init(const char* path);

/**
 * @brief 关闭log服务
 */
void log_close();

void log_trace(const char* fmt, ...);
void log_debug(const char* fmt, ...);
void log_info(const char* fmt, ...);
void log_error(const char* fmt, ...);
void log_fatal(const char* fmt, ...);

#endif // LOG_H