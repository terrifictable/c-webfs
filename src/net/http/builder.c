#include "builder.h"

const char* _404 = "HTTP/1.0 404 Not Found\r\n\r\n404 Not Found";

string build_response(http_version_e http, http_status_e status, header* headers, size_t header_count, string body) {
    string response = string_new();
    
    string_appendf(&response, "%s %d %s\r\n", http_version_to_string(http), status, http_status_to_string(status));
    if (headers != NULL && header_count > 0) {
        string header_str = headers_to_string(headers, header_count);
        string_appends(&response, header_str.str);
    }
    string_appends(&response, "\r\n");

    if (body.len > 0) {
        string_append(&response, body.str, body.len);
    } else {
        string_appendf(&response, "%s\r\n", http_status_to_string(status));
    }

    return response;
}

void send_404(socket_info* si, header* headers, size_t header_count) {
    string body = string_news("404 Not Found");
    string response = build_response(HTTP_1_0, STATUS_NOT_FOUND, headers, header_count, body);
    
    int send_result = send(si->sock, response.str, response.len, 0);
    string_free(&body);
    string_free(&response);
    if (send_result == SOCKET_ERROR) {
        err("[%d] > failed to send 404, error: %d", si->id, WSAGetLastError());
    }
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
