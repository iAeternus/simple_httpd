#ifndef PROCESS_H
#define PROCESS_H

#include "rio.h"

typedef void (*conn_handler_t)(rio_t*);

/**
 * @brief 初始化进程模块
 */
void process_init(void);

/**
 * @brief fork 一个 worker 处理连接
 */
int process_fork_worker(rio_t* clinet_rp, conn_handler_t handler);

#endif
