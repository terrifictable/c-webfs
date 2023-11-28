#pragma once

#ifdef __cplusplus
#   include <cstdio>
#else
#   include <stdio.h>
#endif


/* extern "C" guards */
#ifdef __cplusplus
extern "C" {
#endif
	/* extern "C" guards */


#ifdef __cplusplus
#define unused [[maybe_unused]]
#else
#define unused __attribute__((unused))
#endif


#define DEBUG


#define ANSI_RESET "\033[0m"
#define ANSI_GREY  "\033[38;5;242m"
#define ANSI_HIGH  "\033[38;5;159m"

#define ANSI_RED    "\033[38;5;162m"
#define ANSI_GREEN  "\033[38;5;49m"
#define ANSI_BLUE   "\033[38;5;81m"
#define ANSI_ORANGE "\033[38;5;215m"
#define ANSI_YELLOW "\033[38;5;220m"


#define msg(msg, ...)  printf(    msg "\n", ##__VA_ARGS__)
#define ok(msg, ...)   printf(" " ANSI_GREY "[" ANSI_GREEN  "+" ANSI_GREY "] " ANSI_RESET msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf(" " ANSI_GREY "[" ANSI_BLUE   "*" ANSI_GREY "] " ANSI_RESET msg "\n", ##__VA_ARGS__)
#define warn(msg, ...) printf(" " ANSI_GREY "[" ANSI_ORANGE "#" ANSI_GREY "] " ANSI_RESET msg "\n", ##__VA_ARGS__)

#define err_code(msg, ...)  printf(" " ANSI_GREY "[" ANSI_RED    "-" ANSI_GREY "] (%s:%d in %s) " ANSI_RESET msg "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define err(msg, ...)  printf(" " ANSI_GREY "[" ANSI_RED    "-" ANSI_GREY "] " ANSI_RESET msg "\n", ##__VA_ARGS__)

#define sock_info(msg, ...)  printf(" " ANSI_GREY "[" ANSI_YELLOW "~" ANSI_GREY "] " ANSI_RESET msg "\n", ##__VA_ARGS__)




/* extern "C" guards */
#ifdef __cplusplus
}
#endif
/* extern "C" guards */
