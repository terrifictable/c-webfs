#include "config.h"

const char* error_str[] = {
    #define CONFIG_ERROR_DEF(err) [err] = STR(err)
    CONFIG_ERROR_DEF(STATUS_OK__NO_ERROR),
    CONFIG_ERROR_DEF(STATUS_ERR__NOT_ENOUGH_ARGUMENTS),
    CONFIG_ERROR_DEF(STATUS_ERR__INVALID_ARGUMENTS),
    CONFIG_ERROR_DEF(STATUS_ERR__INVALID_PORT),
};


config_t *parse_args(int argc, char* argv[]) {
    config_t *conf = malloc(sizeof(config_t));
    conf->err = STATUS_OK__NO_ERROR;
    if (argc < 2) {
        conf->err = STATUS_ERR__NOT_ENOUGH_ARGUMENTS;
        return conf;
    }

    if (argc == 2) {
        if (strchr(argv[1], '.') != 0) {
            if (strchr(argv[1], ':') != 0) {
                conf->ip = strtok(argv[1], ":");
                conf->port = atoi(strtok(NULL, ":"));
            } else {
                conf->ip = argv[1];
                conf->port = 8080;
                warn("No Port provided, using default, 8080");
            }
        } else {
            conf->ip = "127.0.0.1";
            warn("No IP provided, using default, 127.0.0.1");
            conf->port = atoi(argv[1]);
        }
    } else if (argc == 2) {
        conf->ip = argv[1];
        conf->port = atoi(argv[2]);
    } else {
        conf->err = STATUS_ERR__INVALID_ARGUMENTS;
        return conf;
    }

    if (conf->port > 65535) {
        err("Port number is too big: %d, maximum is 65535", conf->port);
        conf->err = STATUS_ERR__INVALID_PORT;
        return conf;
    }
    
    info("Using: %s:%d for listener", conf->ip, conf->port);

    return conf;
}