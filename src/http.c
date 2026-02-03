#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "http.h"
#include "error.h"
#include "file.h"
#include "log.h"
#include "net.h"

int http_read_request(int fd, struct http_request_t* req) {
    memset(req, 0, sizeof(struct http_request_t));

    ssize_t n;
    char buf[1024];

    // 读取请求行
    n = net_readline(fd, buf, sizeof(buf));
    if (n <= 0) {
        err_set("Failed to read request line: %s", err_last());
        return -1;
    }
    buf[n] = '\0';

    // 解析请求行
    if (sscanf(buf, "%s %s %s", req->method, req->path, req->version) != 3) {
        err_set("Invalid HTTP request line: %s", buf);
        return -1;
    }

    // 解析请求头
    while (1) {
        n = net_readline(fd, buf, sizeof(buf));
        if (n <= 0) {
            err_set("Failed to read request line: %s", err_last());
            return -1;
        }

        // 查找空行，空行表示头部解析结束
        if (strcmp(buf, "\r\n") == 0) {
            break;
        }

        // 解析头部 key: value
        char* colon = strchr(buf, ':');
        if (!colon) continue;
        *colon = '\0';

        // 移除 key 和 value 的空格
        char* key = buf;
        char* value = colon + 1;
        while (*value == ' ') value++;

        if (req->header_count < MAX_HEADER_COUNT) {
            strncpy(req->headers[req->header_count].key, key, MAX_HEADER_KEY - 1);
            strncpy(req->headers[req->header_count].value, value, MAX_HEADER_VALUE - 1);
            req->header_count++;
        }
    }

    log_debug("Received request: method=%s, path=%s, version=%s", req->method, req->path, req->version);
    return 0;
}

int http_send_response(int fd, const struct http_response_t* resp) {
    int n;
    char buf[1024];

    // 发送响应行
    n = snprintf(buf, sizeof(buf), "%s %d %s\r\n", resp->version, resp->status_code, resp->status_text);
    if (n < 0 || (unsigned long)n >= sizeof(buf)) {
        err_set("Failed to write response line");
        return -1;
    }

    if (net_writen(fd, buf, n) < 0) {
        return -1;
    }

    // 发送响应头
    for (int i = 0; i < resp->header_count; ++i) {
        n = snprintf(buf, sizeof(buf), "%s: %s\r\n", resp->headers[i].key, resp->headers[i].value);
        if (n < 0 || (unsigned long)n >= sizeof(buf)) {
            err_set("Failed to write header");
            return -1;
        }

        if (net_writen(fd, buf, n) < 0) {
            return -1;
        }
    }

    // 发送空行，表示头部结束
    if (net_writen(fd, "\r\n", 2) < 0) {
        return -1;
    }

    // 发送响应体
    if (resp->body && resp->body_len > 0) {
        if (net_writen(fd, resp->body, resp->body_len) < 0) {
            return -1;
        }
    }

    log_debug("Response sent: status=%d, body_len=%zu", resp->status_code, resp->body_len);
    return 0;
}

int http_send_error(int fd, int status, const char* msg) {
    struct http_response_t resp;
    memset(&resp, 0, sizeof(resp));

    resp.status_code = status;
    snprintf(resp.status_text, sizeof(resp.status_text), "%s", msg);
    resp.version[0] = 'H';
    resp.version[1] = 'T';
    resp.version[2] = 'T';
    resp.version[3] = 'P';
    resp.version[4] = '/';
    resp.version[5] = '1';
    resp.version[6] = '.';
    resp.version[7] = '1';

    // 设置返回的文本
    const char* body = "<html><body><h1>404 Not Found</h1></body></html>";
    resp.body = body;
    resp.body_len = strlen(body);

    // 返回响应
    return http_send_response(fd, &resp);
}

int http_send_file_response(int fd, const char* path) {
    struct stat st;
    if (stat(path, &st) < 0) {
        err_set("stat failed");
        return -1;
    }

    struct http_response_t resp;
    memset(&resp, 0, sizeof(resp));

    // 状态行
    strcpy(resp.version, "HTTP/1.1");
    resp.status_code = 200;
    strcpy(resp.status_text, "OK");

    // Content-Type
    snprintf(resp.headers[0].key, sizeof(resp.headers[0].key),
             "Content-Type");
    snprintf(resp.headers[0].value, sizeof(resp.headers[0].value),
             "%s", file_get_mime(path));

    // Content-Length
    snprintf(resp.headers[1].key, sizeof(resp.headers[1].key),
             "Content-Length");
    snprintf(resp.headers[1].value, sizeof(resp.headers[1].value),
             "%ld", st.st_size);

    resp.header_count = 2;

    // 先发响应头
    if (http_send_response(fd, &resp) < 0) {
        return -1;
    }

    // 再发文件内容
    if (file_send(fd, path) < 0) {
        return -1;
    }

    return 0;
}