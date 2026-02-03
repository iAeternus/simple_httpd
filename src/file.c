#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "file.h"
#include "error.h"
#include "log.h"
#include "net.h"

int file_map_path(const char* root,
                  const char* url_path,
                  char* real_path,
                  size_t size) {
    // 请求服务器的根路径
    if (url_path[0] == '/') {
        url_path++; // 路径以 '/' 开头，需要去掉
    }

    // 拼接根目录和 URL 路径
    snprintf(real_path, size, "%s/%s", root, url_path);

    log_debug("Real path: %s", real_path);

    // 检查文件是否存在
    struct stat st;
    if (stat(real_path, &st) == -1) {
        err_set("File not found: %s", real_path);
        return -1;
    }

    // 如果路径是目录，默认返回 index.html
    if (S_ISDIR(st.st_mode)) {
        snprintf(real_path, size, "%s/index.html", root);
        if (stat(real_path, &st) == -1) {
            err_set("Directory not found: %s", real_path);
            return -1;
        }
    }

    return 0;
}

const char* file_get_mime(const char* path) {
    if (strstr(path, ".html") != NULL) return "text/html";
    if (strstr(path, ".css") != NULL) return "text/css";
    if (strstr(path, ".js") != NULL) return "application/javascript";
    if (strstr(path, ".jpg") != NULL || strstr(path, ".jpeg") != NULL) return "image/jpeg";
    if (strstr(path, ".png") != NULL) return "image/png";
    return "application/octet-stream"; // 默认二进制流
}

int file_send(int fd, const char* path) {
    int file_fd = open(path, O_RDONLY);
    if (file_fd < 0) {
        err_set("Unable to open file: %s", path);
        return -1;
    }

    char buf[1024];
    ssize_t n;
    while ((n = read(file_fd, buf, sizeof(buf))) > 0) {
        if (net_writen(fd, buf, n) < 0) {
            err_set("Failed to send file data to client: %s", err_last());
            close(file_fd);
            return -1;
        }
    }

    if (n < 0) {
        err_set("Failed to read file: %s", err_last());
        close(file_fd);
        return -1;
    }

    close(file_fd);
    return 0;
}
