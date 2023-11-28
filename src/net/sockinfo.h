#pragma once

#include <winsock2.h>

typedef struct _socket_info {
    SOCKET sock;
    int id;    
} socket_info;