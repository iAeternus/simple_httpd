#ifndef FILE_H
#define FILE_H

#include <stdio.h>

/**
 * @brief 根据 URL 路径映射真实文件
 */
int file_map_path(const char* root,
                  const char* url_path,
                  char* real_path,
                  size_t size);

/**
 * @brief 获取 MIME 类型
 */
const char* file_get_mime(const char* path);

/**
 * @brief 发送文件
 */
int file_send(int fd, const char* path);

#endif
