#ifndef CONFIG_H
#define CONFIG_H

#define MAX_PATH_LEN 256

struct server_config_t {
    int port;                // 监听端口
    char root[MAX_PATH_LEN]; // 服务器根目录
    char log[MAX_PATH_LEN];  // 日志文件路径
};

/**
 * @brief 从配置文件或参数加载
 */
int config_load(const char* path, struct server_config_t* conf);

/**
 * @brief 打印配置
 */
void config_dump(const struct server_config_t* conf);

#endif // CONFIG_H