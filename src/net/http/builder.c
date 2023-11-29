#include "builder.h"

string build_response(http_version_e http, http_status_e status, header* headers, size_t header_count, string* body) {
    string response = string_new();
    
    string_appendf(&response, "%s %d %s\r\n", http_version_to_string(http), status, http_status_to_string(status));
    string header_str = headers_to_string(headers, header_count);
    string_appends(&response, header_str.str);
    string_appends(&response, "\r\n");

    string_append(&response, body->str, body->len-4); // idfk why the length is 4 more than it should be
    // string_append(&response, "\0", 1);

    return response;
}

const char* http_version_to_string(http_version_e http) {
    switch (http) {
        case HTTP_0_9: return "HTTP/0.9";
        case HTTP_1_0: return "HTTP/1.0";
        case EXPERIMENTAL_HTTP_1_1: return "HTTP/1.1";
        default: return "HTTP/0.9";
    }
}

const char* http_status_to_string(http_status_e status) {
    #define str(s) case STATUS_##s: return #s;
    switch (status) {
        str(OK)
        str(NO_CONTENT)
        str(MOVED_PERMANENTLY)
        str(MOVED_TEMPORARILY)
        str(NOT_MODIFIED)
        str(BAD_REQUEST)
        str(UNAUTHORIZED)
        str(FORBIDDEN)
        str(NOT_FOUND)
        str(INTERNAL_SERVER_ERROR)
        str(NOT_IMPLEMENTED)
        str(SERVICE_UNAVAILABLE)
        str(NETWORK_AUTHENTICATION_REQUIRED)
        default: return "UNKNOWN";
    }
}
