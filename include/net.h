#ifndef NET_H
#define NET_H

/**
 * @brief 创建监听 socke
 */
int net_listen(int port);

/**
 * @brief 接收连接
 */
int net_accept(int listen_fd);

#endif
