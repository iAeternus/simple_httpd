#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "net.h"
#include "error.h"

int net_listen(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        err_set("socket failed: %s", strerror(errno));
        return -1;
    }

    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        err_set("setsockopt failed: %s", strerror(errno));
        close(fd);
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0
    addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        err_set("bind failed: %s", strerror(errno));
        close(fd);
        return -1;
    }

    if (listen(fd, 128) < 0) {
        err_set("listen failed: %s", strerror(errno));
        close(fd);
        return -1;
    }

    return fd;
}

int net_accept(int listen_fd) {
    for (;;) {
        int client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue; // 被信号打断，重试
            }
            err_set("accept failed: %s", strerror(errno));
            return -1;
        }
        return client_fd;
    }
}
