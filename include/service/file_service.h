#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include "http.h"

/**
 * @brief 构建文件响应的 HTTP 响应头
 *
 * 根据文件路径生成对应的 HTTP 响应头信息，
 * 包括：
 *   - 状态行 (200 OK)
 *   - Content-Type
 *   - Content-Length
 *
 * @param path 文件的真实路径
 * @param resp 用于填充的 HTTP 响应结构体
 *
 * @return 0 成功，-1 失败
 */
int build_response_head(const char* path, struct http_response_t* resp);

/**
 * @brief 文件服务器业务处理函数
 *
 * 作为 HTTP 服务器的业务回调函数，用于处理文件访问请求：
 *   - 解析 URL 路径
 *   - 映射到本地文件路径
 *   - 构造响应头
 *   - 发送文件内容
 *
 * @param fd        客户端连接的 socket fd
 * @param req       已解析完成的 HTTP 请求
 * @param user_data 用户上下文（通常包含服务器配置）
 *
 * @return 0 成功，-1 失败
 */
int file_service_handler(int fd, const struct http_request_t* req, void* user_data);

#endif // FILE_SERVICE_H