# c-webfs
An opendir copy written in pure c using the winapi

## ToDo
- Add linux support by using 
> ``` c
> #if defined(_WIN32) || defined(_WIN64)
> // WINDOWS
> #else
> // Linux/MacOS/OpenBSD
> #endif
> ```
- create new thread for every client

