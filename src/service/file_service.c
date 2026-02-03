#include <string.h>
#include <sys/stat.h>
#include "config.h"
#include "server.h"
#include "service/file_service.h"
#include "error.h"
#include "file.h"
#include "log.h"

int build_response_head(const char* path, struct http_response_t* resp) {
    memset(resp, 0, sizeof(struct http_response_t));

    struct stat st;
    if (stat(path, &st) < 0) {
        err_set("stat failed");
        return -1;
    }

    // 状态行
    strcpy(resp->version, "HTTP/1.1");
    resp->status_code = 200;
    strcpy(resp->status_text, "OK");

    // Content-Type
    snprintf(resp->headers[0].key, sizeof(resp->headers[0].key),
             "Content-Type");
    snprintf(resp->headers[0].value, sizeof(resp->headers[0].value),
             "%s", file_get_mime(path));

    // Content-Length
    snprintf(resp->headers[1].key, sizeof(resp->headers[1].key),
             "Content-Length");
    snprintf(resp->headers[1].value, sizeof(resp->headers[1].value),
             "%ld", st.st_size);

    resp->header_count = 2;

    return 0;
}

int file_service_handler(int fd, const struct http_request_t* req, void* user_data) {
    struct server_context* ctx = user_data;
    struct server_config_t* conf = ctx->conf;

    char real_path[512];

    if (strcmp(req->method, "GET") != 0 && strcmp(req->method, "POST") != 0) {
        http_send_error(fd, 405, "Method Not Allowed");
        return -1;
    }

    if (file_map_path(conf->root, req->path, real_path, sizeof(real_path)) < 0) {
        err_print();
        http_send_error(fd, 404, "Not Found");
        return -1;
    }

    struct http_response_t resp;
    if (build_response_head(real_path, &resp) < 0) {
        err_print();
        return -1;
    }

    if (http_send_response(fd, &resp) < 0) {
        err_print();
        return -1;
    }

    if (file_send(fd, real_path) < 0) {
        log_error("send file failed: %s", err_last());
        http_send_error(fd, 500, "Internal Server Error");
        return -1;
    }

    return 0;
}
