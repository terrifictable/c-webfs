#pragma once

/* wtf kinda shit is this vscode erroring when i use common.h and util/string.h */
#include "../../common.h"
#include "../../util/string.h"

typedef struct _header {
    string name;
    string value;
} header;

header header_new(string name, string value);
string headers_to_string(header* headers, size_t count);
void header_free(header* header);
void headers_free(header* header, size_t count);
