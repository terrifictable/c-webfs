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

