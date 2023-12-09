#pragma once

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include <winsock2.h>
#include <windows.h>

#include "http/builder.h"
#include "http/header.h"
#include "util/string.h"
#include "sockinfo.h"
#include "common.h"


enum client_errors {
    CLIENT_OK = 0,
    CLIENT_CONNECTION_FAILED
};

typedef struct _client_result {
    char* msg;
    enum client_errors err;

    SOCKET sock;
} client_result_t;


void accept_client(SOCKET sock, client_result_t* out);
void *handle_client(void* vp_si /* socket_info* */);

