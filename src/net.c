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

ssize_t net_readn(int fd, void* buf, size_t n) {
    size_t left = n;
    char* ptr = buf;

    while (left > 0) {
        ssize_t nr = read(fd, ptr, left);
        if (nr < 0) {
            if (errno == EINTR) {
                continue; // 被信号打断，重试
            }

            err_set("read failed: %s", strerror(errno));
            return -1;
        } else if (nr == 0) {
            break; // 对端关闭连接
        }

        left -= nr;
        ptr += nr;
    }

    return n - left;
}

ssize_t net_readline(int fd, char* buf, size_t maxlen) {
    size_t i = 0;
    char c;
    ssize_t n;

    while (i < maxlen - 1) {
        n = read(fd, &c, 1);
        if (n == 1) {
            buf[i++] = c;
            if (c == '\n') {
                break;
            }
        } else if (n == 0) {
            break; // 对端关闭
        } else {
            if (errno == EINTR) continue;
            err_set("readline failed: %s", strerror(errno));
            return -1;
        }
    }

    buf[i] = '\0';
    return i;
}

ssize_t net_writen(int fd, const void* buf, size_t n) {
    size_t left = n;
    const char* ptr = buf;

    while (left > 0) {
        ssize_t nw = write(fd, ptr, left);
        if (nw < 0) {
            if (errno == EINTR) {
                continue;
            }

            err_set("write failed: %s", strerror(errno));
            return -1;
        }

        left -= nw;
        ptr += nw;
    }

    return n;
}