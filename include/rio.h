#ifndef RIO_H
#define RIO_H

#include <sys/types.h>

#define RIO_BUFSIZE 8192

/**
 * @brief 健壮缓冲区
 */
typedef struct {
    int rio_fd;                // 关联的文件描述符
    ssize_t rio_cnt;            // 内部缓冲区中尚未读取的字节数
    char* rio_bufptr;          // 指向内部缓冲区中下一个待读取字节的位置
    char rio_buf[RIO_BUFSIZE]; // 内部读缓冲区
} rio_t;

/**
 * @brief 从描述符fd的当前文件位置最多传送n个字节到内存位置usrbuf
 * @param fd 文件描述符
 * @param usrbuf 用户缓冲区
 * @param n 最多读取的字节数
 * @return 若成功则为读取的字节数，若EOF则为0，若出错则为-1
 */
ssize_t rio_readn(int fd, void* usrbuf, size_t n);

/**
 * @brief 从位置usrbuf传送n个字节到描述符fd
 * @param fd 文件描述符
 * @param usrbuf 用户缓冲区
 * @param n 读取的字节数
 * @return 若成功则为读取的字节数，若出错则为-1。绝不会返回不足值（short count）
 */
ssize_t rio_writen(int fd, const void* usrbuf, size_t n);

/**
 * @brief 初始化rio_t结构，并将其与文件描述符fd关联
 * @param rp 指向rio_t的指针
 * @param fd 文件描述符
 */
void rio_readinitb(rio_t* rp, int fd);

/**
 * @brief 从内部读缓冲区复制一个文本行，将其传送到usrbuf，当缓冲区变空时，会自动地调用read重新填满缓冲区
 * @param rp 指向rio_t的指针
 * @param usrbuf 用户缓冲区
 * @param maxlen 用户缓冲区最大容量
 * @return 若成功则为读取的字节数，若EOF则为0，若出错则为-1
 */
ssize_t rio_readlineb(rio_t* rp, void* usrbuf, size_t maxlen);

/**
 * @brief 从内部读缓冲区复制最多n个字节，将其传送到usrbuf
 * @param rp 指向rio_t的指针
 * @param usrbuf 用户缓冲区
 * @param n 最多读取的字节数
 * @return 若成功则为读取的字节数，若EOF则为0，若出错则为-1
 */
ssize_t rio_readnb(rio_t* rp, void* usrbuf, size_t n);

#endif // RIO_H