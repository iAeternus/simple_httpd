#ifndef SERVER_H
#define SERVER_H

#include "http.h"

/**
 * @brief HTTP业务回调
 * @note 框架层在解析完 HTTP 请求后调用该函数，将连接 fd、请求结构体以及用户上下文传给业务层处理
 * @param client_fd 已建立连接的 socket fd
 * @param req       已解析完成的 HTTP 请求
 * @param user_data 用户自定义上下文（由框架层透传）
 * @return 0 成功，-1 失败
 */
typedef int (*http_handler_t)(
    int client_fd,
    const struct http_request_t* req,
    void* user_data);

/**
 * @brief 服务器运行时上下文
 */
struct server_context {
    struct server_config_t* conf;
};

/**
 * @brief 启动 HTTP 服务器
 * @note 加载配置文件，初始化监听 socket，并进入主循环，每当有客户端连接时调用注册的 handler 处理请求。
 * @param conf_path 配置文件路径
 * @param handler   业务处理函数
 * @return 0 成功，-1 失败
 */
int http_server_run(const char* conf_path, http_handler_t handler);

#endif // SERVER_H