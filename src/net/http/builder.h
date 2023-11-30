#pragma once

#include "../../common.h"
#include "../../util/string.h"
#include "../sockinfo.h"
#include "header.h"

// typedef struct _response {
    
// } response;


typedef enum _http_version_e {
    HTTP_0_9,
    HTTP_1_0,
    EXPERIMENTAL_HTTP_1_1, // idk if its supported or not...
    UNSUPPORTED_HTTP_2_0,
    UNSUPPORTED_HTTP_3_0,
} http_version_e;

typedef enum _http_status_e {
    STATUS_OK = 200,
    STATUS_NO_CONTENT = 204,
    STATUS_MOVED_PERMANENTLY = 301,
    STATUS_MOVED_TEMPORARILY = 302,
    STATUS_NOT_MODIFIED = 304,
    STATUS_BAD_REQUEST = 400,
    STATUS_UNAUTHORIZED = 401,
    STATUS_FORBIDDEN = 403,
    STATUS_NOT_FOUND = 404,
    STATUS_INTERNAL_SERVER_ERROR = 500,
    STATUS_NOT_IMPLEMENTED = 501,
    STATUS_SERVICE_UNAVAILABLE = 503,
    STATUS_NETWORK_AUTHENTICATION_REQUIRED = 530,
} http_status_e;

const char* http_version_to_string(http_version_e http);
const char* http_status_to_string(http_status_e status);

string build_response(http_version_e http, http_status_e status, header* headers, size_t header_count, string body);

extern const char* _404;
void send_404(socket_info* si, header* headers, size_t header_count);
