#ifndef PROCESS_H
#define PROCESS_H

typedef void (*conn_handler_t)(int client_fd);

/**
 * @brief 初始化进程模块
 */
void process_init(void);

/**
 * @brief fork 一个 worker 处理连接
 */
int process_fork_worker(int client_fd, conn_handler_t handler);

// /**
//  * @brief SIGCHLD 回收
//  */
// void process_reap_children(void);

#endif
