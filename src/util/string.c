#include "string.h"

#include "common.h"

string string_new(int len) {
    string str = STRING_INIT();
    str.str = calloc(len, sizeof(char));
    return str;
}

void string_append(string *str, char* s, int len) {
    if (s == NULL) return;

    char* new_str = (char*) realloc(str->str, str->len + len);
    if (new_str == NULL) return;

    memcpy(&new_str[str->len], s, len);
    str->str = new_str;
    str->len += len;
}

void string_appends(string *str, char* s) {
    if (s == NULL) return;
    string_append(str, s, strlen(s));
}

void string_appendf(string *str, char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    const int buf_len = 1024;
    char buf[buf_len];
    memset(buf, 0, buf_len); // 0 out buf
    vsprintf(buf, fmt, args);

    va_end(args);

    string_append(str, buf, strlen(buf));
}

void string_free(string *str) {
    free(str->str);
}
