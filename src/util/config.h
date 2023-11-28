#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"


#define STR(x) _STR(x)
#define _STR(x) #x

typedef enum _config_status {
    STATUS_OK__NO_ERROR = 0,
    STATUS_ERR__NOT_ENOUGH_ARGUMENTS,
    STATUS_ERR__INVALID_ARGUMENTS,
    STATUS_ERR__INVALID_PORT,
} config_status_e;

extern const char* error_str[];

typedef struct _config {
    char *ip;
    int port;

    config_status_e err;
} config_t;

config_t *parse_args(int argc, char* argv[]);
