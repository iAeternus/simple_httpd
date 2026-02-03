#include "server.h"
#include "service/file_service.h"

int main(int argc, char* argv[]) {
    const char* conf = "conf/server.conf";
    if (argc > 1) {
        conf = argv[1];
    }

    return http_server_run(conf, file_service_handler);
}
