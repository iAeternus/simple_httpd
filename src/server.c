#include <string.h>
#include <unistd.h>
#include "server.h"
#include "config.h"
#include "net.h"
#include "process.h"
#include "log.h"
#include "error.h"
#include "http.h"

static struct server_context g_ctx;
static http_handler_t g_handler;

static void handle_client(int client_fd) {
    struct http_request_t req;
    memset(&req, 0, sizeof(req));

    if (http_read_request(client_fd, &req) < 0) {
        log_error("read request failed: %s", err_last());
        http_send_error(client_fd, 400, "Bad Request");
        goto cleanup;
    }

    if (g_handler) {
        g_handler(client_fd, &req, &g_ctx);
    }

cleanup:
    close(client_fd);
}

int http_server_run(const char* conf_path, http_handler_t handler) {
    static struct server_config_t conf;

    if (config_load(conf_path, &conf) < 0) {
        log_error("load config failed: %s", err_last());
        return -1;
    }

    g_ctx.conf = &conf;
    g_handler = handler;

    config_dump(&conf);

    log_init(g_ctx.conf->log);

    process_init();

    int listen_fd = net_listen(conf.port);
    if (listen_fd < 0) {
        log_error("listen failed: %s", err_last());
        return -1;
    }

    log_info("server listening on port %d", conf.port);

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

    log_close();
    return 0;
}
