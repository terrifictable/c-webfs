#include "client.h"

const char* _404 = "HTTP/1.0 404 Not Found\r\n\r\n404 Not Found";

void send_404(socket_info* si) {
    int send_result = send(si->sock, _404, strlen(_404), 0);
    if (send_result == SOCKET_ERROR) {
        err("[%d] > failed to send '%s', error: %d", si->id, _404, WSAGetLastError());
    }
}

void get_mode_str(string* str, struct stat* fstat) {
    string_appends(str, (S_ISDIR(fstat->st_mode)) ? "d" : "-");
    if (!S_ISDIR(fstat->st_mode)) {
        string_appends(str, (fstat->st_mode & S_IRUSR) ? "r" : "-");
        string_appends(str, (fstat->st_mode & S_IWUSR) ? "w" : "-");
        string_appends(str, (fstat->st_mode & S_IXUSR) ? "x" : "-");
        string_appends(str, (fstat->st_mode & S_IRGRP) ? "r" : "-");
        string_appends(str, (fstat->st_mode & S_IWGRP) ? "w" : "-");
        string_appends(str, (fstat->st_mode & S_IXGRP) ? "x" : "-");
        string_appends(str, (fstat->st_mode & S_IROTH) ? "r" : "-");
        string_appends(str, (fstat->st_mode & S_IWOTH) ? "w" : "-");
        string_appends(str, (fstat->st_mode & S_IXOTH) ? "x" : "-");
    } else {
        string_appends(str, "--------");
    }
    string_append(str, "\0", 1);
}

void handle_directory(socket_info* si, char* f);

extern int should_exit;
void handle_client(socket_info* si) {
    const int buf_len = 1024;
    char buf[buf_len];
    int recv_result, send_result;

    recv_result = recv(si->sock, buf, buf_len, 0);
    if (recv_result > 0) {
        sock_info("[%d] > bytes received: %d", si->id, recv_result);

        if (strncmp(buf, "GET", 3) != 0) {
            warn("[%d] > request is not a GET request", si->id);
            // warn("[%d] > buf: %s", si->id, buf);
            // TODO: maybe handle POST requests and others
            send_404(si);
            goto cleanup;
        }


        // GET /file.html ...
        char* f = buf + 5;
        *strchr(f, ' ') = 0;
        char* p = strchr(f, '?');
        if (p != NULL) {
            *p = 0; // remove query string
        }
 
        char last_c = *(f+strlen(f)-1);
        if (last_c == '/') {
            handle_directory(si, f);
            goto cleanup;
        }

        sock_info("[%d] > file requested: %s", si->id, f);
        FILE* fptr = fopen(f, "rb");
        if (!fptr) { // NULL
            warn("[%d] > requested file doesnt exist, sending 404", si->id);
            send_404(si);
            goto cleanup;
        }

        const int fbuf_len = 1024;
        char* fbuf = calloc(fbuf_len, sizeof(char));

        string fstr = string_new();
        while (fread(fbuf, sizeof(char), fbuf_len, fptr)) {
            string_appendf(&fstr, "%s", fbuf);
            memset(fbuf, 0, fbuf_len);
        }
        fclose(fptr);
        free(fbuf);


        header headers[] = {
            header_new(string_news("Content-Type"), string_news("text/plain")),
            header_new(string_news("Server"), string_news("c-webfs")),
        };

        string response = build_response(HTTP_1_0, STATUS_OK, headers, sizeof(headers)/sizeof(*headers), &fstr);

        headers_free(headers, sizeof(headers)/sizeof(*headers));

        send_result = send(si->sock, response.str, response.len, 0);
        memset(buf, 0, buf_len);
        string_free(&response);
        string_free(&fstr);

        if (send_result == SOCKET_ERROR) {
            err("[%d] > failed to send '%s', error: %d", si->id, buf, WSAGetLastError());
            goto cleanup;
        }
    } else if (recv_result == 0 || WSAGetLastError() == 10054) { // magic number: winsock error code for "remote forcfully closed connection"
        sock_info("[%d] > Closing Connection", si->id);
    } else {
        err("[%d] > failed to get data from connection, error: %d", si->id, WSAGetLastError());
    }

cleanup:
    shutdown(si->sock, SD_BOTH);
    closesocket(si->sock);
}

client_result_t accept_client(SOCKET sock) {
    client_result_t res = { 
        .err = CLIENT_OK, 
        .msg = NULL, 
        .sock = 0 
    };

    res.sock = accept(sock, NULL, NULL);
    if (res.sock == INVALID_SOCKET) {
        res.err = CLIENT_CONNECTION_FAILED;
        sprintf(res.msg, "failed to accept connection, error: %d", WSAGetLastError());
    }
    return res;
}

void handle_directory(socket_info* si, char* f) {
    f--;

    sock_info("[%d] > directory requested: '%s'", si->id, f);;

    string body = string_new(0);
    string_appends(&body, 
        "<head>"
        "<meta name=\"color-scheme\" content=\"dark\">"
        "<style>\n"
        "body {font-family: monospace;}\n"
        "table td {padding: 0 0.4em;}\n"
        "a {color:#56c8ff}\n"
        "a:hover {color:lightblue}\n"
        "tr:hover td {background-color: #333}\n"
        "#name{width:200px}\n"
        "</style>"
        "</head>"
        "<body>"
    );

    char* dir_name = calloc((strlen(f) + 2), sizeof(char));
    sprintf_s(dir_name, strlen(f)+2, ".%s", f);
    DIR *d = opendir(dir_name);
    if (!d) {
        err("[%d] > Failed to open dir: %s", si->id, dir_name);
        send_404(si);
        goto cleanup;
    }

    string_appendf(&body, 
        "<h3>Index of %s</h3>"
        "<table>"
            "<thead>"
                "<tr>"
                    "<td id=\"mode\"><b>Mode</b></td>"
                    "<td id=\"name\"><b>Name</b></td>"
                    "<td id=\"size\"><b>Size</b></td>"
                "</tr>"
            "</thead>"
        "<tbody>",
        f
    );
    

    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        struct stat fstat;
        char* full_path = malloc((e->d_namlen + strlen(dir_name) + 2) * sizeof(char));
        sprintf(full_path, "%s%s", dir_name, e->d_name);
        stat(full_path, &fstat);

        string modestr = string_new();
        get_mode_str(&modestr, &fstat);
        
        if (S_ISDIR(fstat.st_mode)) {
            if ((strcmp(f, "/") == 0 && strcmp(e->d_name, "..") == 0) || strcmp(e->d_name, ".") == 0) { // if not root dir
            } else {
                string_appendf(&body, 
                    "<tr>"
                        "<td>%s</td>"
                        "<td><a href=\"%s%s/\">%s/</a></td>"
                        "<td align=\"right\"></td>"
                    "</tr>", 
                    modestr.str, 
                    f, 
                    e->d_name, 
                    e->d_name
                );
            }
        } else {
            string_appendf(&body, 
                "<tr>"
                    "<td>%s</td>"
                    "<td><a href=\"%s%s\">%s</a></td>"
                    "<td align=\"right\">%d</td>"
                "</tr>", 
                modestr.str, 
                f, 
                e->d_name, 
                e->d_name,
                fstat.st_size
            );
        }
        string_free(&modestr);

        free(full_path);
    }
    string_appends(&body, "</tbody></table></body>");
    closedir(d);


    header headers[] = {
        header_new(string_news("Content-Type"), string_news("text/html")),
        header_new(string_news("Server"), string_news("c-webfs")),
    };

    string response = build_response(HTTP_1_0, STATUS_OK, headers, sizeof(headers)/sizeof(*headers), &body);
    headers_free(headers, sizeof(headers)/sizeof(*headers));

    int send_result = send(si->sock, response.str, response.len, 0);
    string_free(&response);
    if (send_result == SOCKET_ERROR) {
        err("[%d] > failed to send '%s', error: %d", si->id, _404, WSAGetLastError());
    }

cleanup:
    free(dir_name);
    string_free(&body);
}
