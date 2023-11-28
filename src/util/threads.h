#pragma once

#include <windows.h>

#include "common.h"


#define MAX_ARGS 5

typedef struct _thread_args {
    void* arg;
    void* func_ptr;
} thread_args;

typedef struct _thread {
    HANDLE handle;
    DWORD id;

    char* err;
} thread;

thread* create_thread(thread_args args);
void destroy_thread(thread* thread_ptr);
