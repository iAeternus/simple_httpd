#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include "http.h"

int build_response_head(const char* path, struct http_response_t* resp);

#endif // FILE_SERVICE_H