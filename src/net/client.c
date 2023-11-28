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

// FIXME: THERE IS SOME STUPID FUCKING SEGFAULT SOMEWHERE IN THE FOLLOWING FUNCTION AT THE DIRECTORY PART

void handle_directory(socket_info* si, char* f);

extern int should_exit;
void handle_client(socket_info* si) {
    const int buf_len = 1024,
             fbuf_len = 64;
    char buf[buf_len],
        fbuf[fbuf_len];
    int recv_result, send_result;

    recv_result = recv(si->sock, buf, buf_len, 0);
    if (recv_result > 0) {
        sock_info("[%d] > bytes received: %d", si->id, recv_result);

        // GET /file.html ...
        char* f = buf + 5;
        *strchr(f, ' ') = 0;

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

        string fstr = string_new(fbuf_len+1);
        while (fgets(fbuf, fbuf_len, fptr)) {
            string_appends(&fstr, fbuf);
            memset(fbuf, 0, fbuf_len);
        }
        fclose(fptr);

        send_result = send(si->sock, fstr.str, fstr.len, 0);
        if (send_result == SOCKET_ERROR) {
            err("[%d] > failed to send '%s', error: %d", si->id, buf, WSAGetLastError());
            goto cleanup;
        }

        memset(fbuf, 0, fbuf_len);
        memset(buf, 0, buf_len);
        string_free(&fstr);
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

    string listing = string_new(0);
    string_appends(&listing, "<head>");
    string_appends(&listing, "<meta name=\"color-scheme\" content=\"dark\">");
    string_appends(&listing, "<style>\n");
    string_appends(&listing, "body {font-family: monospace;}\n");
    string_appends(&listing, "table td {padding: 0 0.4em;}\n");
    string_appends(&listing, "a {color:#56c8ff}\n");
    string_appends(&listing, "a:hover {color:lightblue}\n");
    string_appends(&listing, "tr:hover td {background-color: #333}\n");
    string_appends(&listing, "#name{width:200px}\n");
    string_appends(&listing, "</style>");
    string_appends(&listing, "</head>");
    string_appends(&listing, "<body>");

    char* dir_name = calloc((strlen(f) + 2), sizeof(char));
    sprintf_s(dir_name, strlen(f)+2, ".%s", f);
    DIR *d = opendir(dir_name);
    if (!d) {
        err("[%d] > Failed to open dir: %s", si->id, dir_name);
        send_404(si);
        goto cleanup;
    }

    string_appendf(&listing, "<h3>Index of %s</h3>", f);
    string_appendf(&listing, "<table><thead><tr><td id=\"mode\"><b>Mode</b></td><td id=\"name\"><b>Name</b></td><td id=\"size\"><b>Size</b></td></tr></thead><tbody>");

    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        struct stat fstat;
        char* full_path = malloc((e->d_namlen + strlen(dir_name) + 2) * sizeof(char));
        sprintf(full_path, "%s%s", dir_name, e->d_name);
        stat(full_path, &fstat);

        string modestr = string_new(0);
        get_mode_str(&modestr, &fstat);
        
        if (S_ISDIR(fstat.st_mode)) {
            if ((strcmp(f, "/") == 0 && strcmp(e->d_name, "..") == 0) || strcmp(e->d_name, ".") == 0) { // if not root dir
            } else {
                string_appendf(&listing, "<tr><td>%s</td><td><a href=\"%s%s/\">%s/</a></td><td align=\"right\"></td></tr>", modestr.str, f, e->d_name, e->d_name);
            }
        } else {
            string_appendf(&listing, "<tr><td>%s</td><td><a href=\"%s%s\">%s</a></td><td align=\"right\">%ld B</td></tr>", modestr.str, f, e->d_name, e->d_name, fstat.st_size);
        }
        string_free(&modestr);

        free(full_path);
    }
    string_appends(&listing, "</tbody></table></body>");

    closedir(d);

    int send_result = send(si->sock, listing.str, listing.len, 0);
    if (send_result == SOCKET_ERROR) {
        err("[%d] > failed to send '%s', error: %d", si->id, _404, WSAGetLastError());
    }

cleanup:
    free(dir_name);
    string_free(&listing);
}
