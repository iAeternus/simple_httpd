#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "error.h"

static __thread char* g_errmsg = NULL;
static pthread_mutex_t errmsg_lock = PTHREAD_MUTEX_INITIALIZER;

void err_set(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int size = vsnprintf(NULL, 0, fmt, ap) + 1;
    va_end(ap);

    pthread_mutex_lock(&errmsg_lock);
    g_errmsg = realloc(g_errmsg, size);
    if (g_errmsg == NULL) {
        fprintf(stderr, "Memory allocation for error message failed!\n");
        pthread_mutex_unlock(&errmsg_lock);
        return;
    }

    va_start(ap, fmt);
    vsnprintf(g_errmsg, size, fmt, ap);
    va_end(ap);

    pthread_mutex_unlock(&errmsg_lock);
}

const char* err_last() {
    return (g_errmsg && g_errmsg[0] != '\0') ? g_errmsg : "no error";
}

void err_clear() {
    pthread_mutex_lock(&errmsg_lock);
    if (g_errmsg) {
        g_errmsg[0] = '\0';
    }
    pthread_mutex_unlock(&errmsg_lock);
}

void err_print(void) {
    pthread_mutex_lock(&errmsg_lock);
    if (g_errmsg && g_errmsg[0] != '\0') {
        fprintf(stderr, "Error: %s\n", g_errmsg);
    } else {
        fprintf(stderr, "No error message available.\n");
    }
    pthread_mutex_unlock(&errmsg_lock);
}
