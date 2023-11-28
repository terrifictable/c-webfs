#include "socket.h"

socket_result_t create_socket(config_t conf, int max_clients) {
    socket_result_t res = {
        .retc = 0,
        .sock = 0,
        .err = SOCKET_OK,
        .msg = NULL,
    };

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        sprintf(res.msg, "WSAStartup failed, error: %d", WSAGetLastError());
        res.retc = 1;
        res.err = SOCKET_FAILED_WSA_STARTUP;
        return res;
    }
    ok("Winsock initialized");

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        sprintf(res.msg, "Failed to create socket, error: %d", WSAGetLastError());
        res.retc = 1;
        res.err = SOCKET_FAILED_CREATE;
        return res;
    }
    ok("Socket created");
    res.sock = sock;

    unused struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(conf.ip);
    addr.sin_port = htons(conf.port);

    if (bind(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        sprintf(res.msg, "Failed to bind socket to %s:%d, error: %d", conf.ip, conf.port, WSAGetLastError());
        res.retc = 1;
        res.err = SOCKET_FAILED_BIND;
        return res;
    }
    ok("Bound socket");

    if (listen(sock, max_clients) == SOCKET_ERROR) {
        sprintf(res.msg, "Failed to listen on %s:%d, error: %d", conf.ip, conf.port, WSAGetLastError());
        res.retc = 1;
        res.err = SOCKET_FAILED_LISTEN;
        return res;
    }

    return res;
}


void destroy_socket(socket_result_t sr) {
    closesocket(sr.sock);
    WSACleanup();
}

