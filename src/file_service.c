#include <string.h>
#include <sys/stat.h>
#include "file_service.h"
#include "error.h"
#include "file.h"

int build_response_head(const char* path, struct http_response_t* resp) {
    memset(resp, 0, sizeof(struct http_response_t));

    struct stat st;
    if (stat(path, &st) < 0) {
        err_set("stat failed");
        return -1;
    }

    // 状态行
    strcpy(resp->version, "HTTP/1.1");
    resp->status_code = 200;
    strcpy(resp->status_text, "OK");

    // Content-Type
    snprintf(resp->headers[0].key, sizeof(resp->headers[0].key),
             "Content-Type");
    snprintf(resp->headers[0].value, sizeof(resp->headers[0].value),
             "%s", file_get_mime(path));

    // Content-Length
    snprintf(resp->headers[1].key, sizeof(resp->headers[1].key),
             "Content-Length");
    snprintf(resp->headers[1].value, sizeof(resp->headers[1].value),
             "%ld", st.st_size);

    resp->header_count = 2;

    return 0;
}

