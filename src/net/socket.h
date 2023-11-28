#pragma once

#include <winsock2.h>
#include <windows.h>

#include "common.h"
#include "util/config.h"

enum socket_errors {
    SOCKET_OK = 0,
    SOCKET_FAILED_WSA_STARTUP,
    SOCKET_FAILED_CREATE,
    SOCKET_FAILED_BIND,
    SOCKET_FAILED_LISTEN,
};

typedef struct _socket_result {
    char* msg;
    enum socket_errors err;

    int retc;

    SOCKET sock;
} socket_result_t;


socket_result_t create_socket(config_t conf, int max_clients);
void destroy_socket(socket_result_t sr);
