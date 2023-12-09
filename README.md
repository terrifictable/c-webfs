# c-webfs
An opendir copy written in pure c using the winapi

## ToDo
- [X] create new thread for every client
- [ ] Add linux support by using 
> ``` c
> #if defined(_WIN32) || defined(_WIN64)
> // WINDOWS
> #else
> // Linux/MacOS/OpenBSD
> #endif
> ```
- [ ] fix how sockets are handled to make multiple requests at the same time possible
- [ ] parse headers
- [ ] make this more nice to use (library like)
> example:
> ```c
> int main(void) {
>   server_t* server = listen("127.0.0.1", 8080);
> 
>   // handle routes, maybe even supporting regex/wildcards
>   DEF_ROUTE(server, "/", handle_dir);
> 
>   while (server->running) {
>       if (!handle_client(server)) { // creates new thread on its own
>           goto cleanup;
>       }
>   }
> 
> cleanup:
>   shutdown(server);
> 
>   return 0;
> }
> ```
