#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>
#include "config.h"
#include "error.h"
#include "log.h"

int config_load(const char* path, struct server_config_t* conf) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        err_set("Unable to open config file: %s", strerror(errno));
        return -1;
    }

    char buf[1024];
    ssize_t n;
    size_t total = 0;

    // 读取文件内容
    while ((n = read(fd, buf + total, sizeof(buf) - total)) > 0) {
        total += n;
        if (total >= sizeof(buf)) {
            break;
        }
    }

    if (n == -1) {
        err_set("Error reading config file: %s", strerror(errno));
        close(fd);
        return -1;
    }

    close(fd);

    // 解析配置文件
    char* line = strtok(buf, "\n");
    while (line != NULL) {
        char key[64], value[256];
        if (sscanf(line, "%63s = %255[^\n]", key, value) == 2) {
            // 解析配置项
            if (strcmp(key, "port") == 0) {
                conf->port = atoi(value);
            } else if (strcmp(key, "root") == 0) {
                // 如果是相对路径，将其转换为绝对路径
                if (value[0] != '/') {
                    char cwd[256];
                    if (getcwd(cwd, sizeof(cwd)) == NULL) {
                        err_set("Failed to get current working directory: %s", strerror(errno));
                        return -1;
                    }
                    snprintf(conf->root, MAX_PATH_LEN, "%s/%s", cwd, value);
                } else {
                    strncpy(conf->root, value, MAX_PATH_LEN - 1);
                }
            } else if (strcmp(key, "log") == 0) {
                // 同样支持相对路径的 log 文件路径
                if (value[0] != '/') {
                    char cwd[256];
                    if (getcwd(cwd, sizeof(cwd)) == NULL) {
                        err_set("Failed to get current working directory: %s", strerror(errno));
                        return -1;
                    }
                    snprintf(conf->log, MAX_PATH_LEN, "%s/%s", cwd, value);
                } else {
                    strncpy(conf->log, value, MAX_PATH_LEN - 1);
                }
            }
        }
        line = strtok(NULL, "\n");
    }

    return 0;
}

void config_dump(const struct server_config_t* conf) {
    log_info("Server configuration:");
    log_info("  Port: %d", conf->port);
    log_info("  Root directory: %s", conf->root);
    log_info("  Log file: %s", conf->log);
}