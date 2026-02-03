#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include "process.h"
#include "error.h"

static void sigchld_handler() {
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        /* 回收所有已退出的子进程 */
    }
}

void process_init(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_restorer = sigchld_handler;
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_RESTART; // 自动重启系统调用
    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        err_set("sigaction(SIGCHLD) failed: %s", strerror(errno));
    }
}

int process_fork_worker(int client_fd, conn_handler_t handler) {
    pid_t pid = fork();
    if (pid < 0) {
        err_set("fork failed: %s", strerror(errno));
        return -1;
    }

    if (pid == 0) {
        handler(client_fd);
        _exit(0);
    }

    return 0; // 父进程什么都不做，继续accept
}
