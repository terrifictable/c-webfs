#include "header.h"

header header_new(string name, string value) {
    header h = {
        .name = name,
        .value = value,
    }; 
    return h;
}

string headers_to_string(header* headers, size_t count) {
    string str = string_new(0);
    for (size_t i = 0; i < count; i++) {
        string_appendf(&str, "%s: %s\n", headers[i].name.str, headers[i].value.str);
    }
    string_append(&str, "\0", 1);
    return str;
}

void header_free(header* h) {
    string_free(&h->name);
    string_free(&h->value);
}

void headers_free(header* headers, size_t count) {
    for (size_t i=0; i < count; i++) {
        header_free(&headers[i]);
    }
}

