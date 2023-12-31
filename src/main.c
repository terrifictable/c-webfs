#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <winsock2.h>
#include <windows.h>

#include "common.h"
#include "net/client.h"
#include "net/socket.h"
#include "net/sockinfo.h"
#include "util/config.h"


#define MAX_CLIENTS 50


int should_exit = 0;
int retc = 0;
int main(int argc, char* argv[]) {
    config_t *conf = parse_args(argc, argv);
    if (conf->err != STATUS_OK__NO_ERROR) {
        err("Parsing Argument Failed, error: %s", error_str[conf->err]);
        retc = 1; goto cleanup;
    }


    socket_result_t listener = create_socket(*conf, MAX_CLIENTS);    
    if (listener.err != SOCKET_OK) {
        err("%s", listener.msg);
        retc = listener.retc;
        goto cleanup;
    }
    ok("Listening on %s:%d", conf->ip, conf->port);

    int cid = 0;
    while (should_exit == 0) {
        client_result_t client;
        accept_client(listener.sock, &client); 
        if (client.err != CLIENT_OK) {
            err("%s", client.msg);
            continue;
        }
        cid++;
        sock_info("New connection (%d, %p)", cid, client);

        pthread_t thread;
        socket_info si = {.sock = client.sock, .id = cid};
        pthread_create(&thread, NULL, handle_client, (void*)&si);
        pthread_detach(thread);
    }


cleanup:
    info("Exiting");
    free(conf);
    pthread_exit(NULL);
    destroy_socket(listener);

    return retc;
}

