#ifndef NET_H
#define NET_H

#include <stdio.h>

/**
 * @brief 创建监听 socke
 */
int net_listen(int port);

/**
 * @brief 接收连接
 */
int net_accept(int listen_fd);

/**
 * @brief 可靠读取 n 字节，返回实际读取的字节数
 */
ssize_t net_readn(int fd, void* buf, size_t n);

/**
 * @brief 读取一行
 */
ssize_t net_readline(int fd, char* buf, size_t maxlen);

/**
 * @brief 可靠写入 n 字节
 */
ssize_t net_writen(int fd, const void* buf, size_t n);

#endif
