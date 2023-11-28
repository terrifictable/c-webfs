#include "threads.h"

thread* create_thread(thread_args args) {
    thread *t = (thread*)malloc(sizeof(thread));
    t->err = NULL;
    t->handle = CreateThread(NULL, 0, args.func_ptr, args.arg, 0, &t->id);
    if (t->handle == NULL) {
        sprintf(t->err, "Failed to create thread, err: %ld", GetLastError());
    }
    return t;
}

void destroy_thread(thread* thread_ptr) {
    CloseHandle(thread_ptr->handle);
    free(thread_ptr);
}
