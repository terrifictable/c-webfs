#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>


typedef struct _string {
    char* str;
    size_t len;
} string;

#define STRING_INIT() { NULL, 0 }

string string_new();
string string_news(char* s);
void string_append(string *str, char* s, int len);
void string_appends(string *str, char* s);
void string_appendf(string *str, char* fmt, ...);
void string_free(string *str);
