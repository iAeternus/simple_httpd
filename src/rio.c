#include "rio.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>

ssize_t rio_readn(int fd, void* usrbuf, size_t n) {
    size_t nleft = n;
    ssize_t nread;
    char* bufp = usrbuf;

    while (nleft > 0) {
        if ((nread = read(fd, bufp, nleft)) < 0) {
            if (errno == EINTR) {
                nread = 0;
            } else {
                return -1; // errno set by read()
            }
        } else if (nread == 0) {
            break; // EOF
        }
        nleft -= nread;
        bufp += nread;
    }
    return n - nleft; // >= 0
}

ssize_t rio_writen(int fd, const void* usrbuf, size_t n) {
    size_t nleft = n;
    ssize_t nwritten;
    const char* bufp = usrbuf;

    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) < 0) {
            if (errno == EINTR) {
                nwritten = 0;
            } else {
                return -1; // errno set by write()
            }
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return n;
}

void rio_readinitb(rio_t* rp, int fd) {
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

static ssize_t rio_read(rio_t* rp, char* usrbuf, size_t n) {
    size_t cnt;

    while (rp->rio_cnt <= 0) {
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if (rp->rio_cnt < 0) {
            if (errno != EINTR) {
                return -1;
            }
        } else if (rp->rio_cnt == 0) {
            return 0; // EOF
        } else {
            rp->rio_bufptr = rp->rio_buf;
        }
    }

    cnt = n;
    if ((size_t)rp->rio_cnt < n) {
        cnt = (size_t)rp->rio_cnt;
    }
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

ssize_t rio_readlineb(rio_t* rp, void* usrbuf, size_t maxlen) {
    size_t n, rc;
    char c, *bufp = usrbuf;

    for (n = 1; n < maxlen; n++) {
        if ((rc = rio_read(rp, &c, 1)) == 1) {
            *bufp++ = c;
            if (c == '\n') {
                n++;
                break;
            }
        } else if (rc == 0) {
            if (n == 1) {
                return 0; // EOF, no data read
            } else {
                break; // EOF, some data was read
            }
        } else {
            return -1; // Error
        }
    }
    *bufp = 0;
    return n - 1;
}

ssize_t rio_readnb(rio_t* rp, void* usrbuf, size_t n) {
    size_t nleft = n;
    ssize_t nread;
    char* bufp = usrbuf;

    while (nleft > 0) {
        if ((nread = rio_read(rp, bufp, nleft)) < 0) {
            return -1; // errno set by read()
        } else if (nread == 0) {
            break; // EOF
        }
        nleft -= nread;
        bufp += nread;
    }
    return n - nleft; // >= 0
}