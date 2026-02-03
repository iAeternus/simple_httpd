#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "log.h"
#include "net.h"
#include "process.h"
#include "http.h"
#include "file.h"
#include "error.h"
#include "file_service.h"

#define REAL_PATH_LEN 512

static struct server_config_t g_conf;

static void handle_client(int client_fd) {
    log_debug("Handling client request...");

    struct http_request_t req;
    memset(&req, 0, sizeof(req));

    if (http_read_request(client_fd, &req) < 0) {
        log_error("failed to read request: %s", err_last());
        http_send_error(client_fd, 400, "Bad Request");
        close(client_fd);
        return;
    }

    log_info("request: %s %s %s", req.method, req.path, req.version);

    char real_path[REAL_PATH_LEN];
    if (file_map_path(g_conf.root, req.path, real_path, sizeof(real_path)) < 0) {
        log_error("map path failed: %s", err_last());
        http_send_error(client_fd, 404, "Not Found");
        close(client_fd);
        return;
    }

    struct http_response_t resp;
    if (build_response_head(real_path, &resp) < 0) {
        err_print();
        close(client_fd);
        return;
    }

    if(http_send_response(client_fd, &resp) < 0) {
        err_print();
        close(client_fd);
        return;
    }

    if (file_send(client_fd, real_path) < 0) {
        log_error("send file failed: %s", err_last());
        http_send_error(client_fd, 500, "Internal Server Error");
        close(client_fd);
        return;
    }

    close(client_fd);
}

int main(int argc, char* argv[]) {
    const char* conf_path = "conf/server.conf";
    if (argc > 1) {
        conf_path = argv[1];
    }

    // 加载配置
    if (config_load(conf_path, &g_conf) < 0) {
        err_print();
        exit(EXIT_FAILURE);
    }
    config_dump(&g_conf);

    // 初始化进程模块（注册 SIGCHLD）
    process_init();

    // 监听socket
    int listen_fd = net_listen(g_conf.port);
    if (listen_fd < 0) {
        err_print();
        exit(EXIT_FAILURE);
    }

    log_info("server listening on port %d", g_conf.port);

    // 主循环
    for (;;) {
        int client_fd = net_accept(listen_fd);
        if (client_fd < 0) {
            log_error("accept failed: %s", err_last());
            continue;
        }

        if (process_fork_worker(client_fd, handle_client) < 0) {
            log_error("fork worker failed: %s", err_last());
            close(client_fd);
            continue;
        }

        close(client_fd);
    }

    return 0;
}