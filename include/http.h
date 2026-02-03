#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>

#define MAX_METHOD_LEN 8
#define MAX_PATH_LEN 256
#define MAX_VERSION_LEN 16
#define MAX_HEADER_COUNT 32
#define MAX_HEADER_KEY 64
#define MAX_HEADER_VALUE 256
#define MAX_STATUS_TEXT 64
#define MAX_MIME_TYPE 64

struct http_header_t {
    char key[MAX_HEADER_KEY];
    char value[MAX_HEADER_VALUE];
};

struct http_request_t {
    char method[MAX_METHOD_LEN];
    char path[MAX_PATH_LEN];
    char version[MAX_VERSION_LEN];

    struct http_header_t headers[MAX_HEADER_COUNT];
    int header_count;
};

struct http_response_t {
    char version[MAX_VERSION_LEN];
    int status_code;
    char status_text[MAX_STATUS_TEXT];
    char content_type[MAX_MIME_TYPE];

    struct http_header_t headers[MAX_HEADER_COUNT];
    int header_count;

    const void* body;
    size_t body_len;
};

/**
 * @brief 从 socket 读取并解析请求
 */
int http_read_request(int fd, struct http_request_t* req);

/**
 * @brief 构造并发送响应
 */
int http_send_response(int fd, const struct http_response_t* resp);

/**
 * @brief 发送错误页面
 */
int http_send_error(int fd, int status, const char* msg);

#endif
