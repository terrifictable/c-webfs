CC := gcc
INCLUDE := -Iinclude/ -Isrc/
LIBS := -lws2_32
FLAGS := -std=c17 -g -Wall -Wextra -Werror $(INCLUDE)

LD      := ld
LDFLAGS := 

srcdir := src
src    := $(wildcard $(srcdir)/*.c $(srcdir)/*/*.c $(srcdir)/*/*/*.c $(srcdir)/*/*/*/*.c)
obj    := $(src:%.c=%.o)

bin := bin
OUT := $(bin)/http_server


.PHONY: build all clean run
run: build
	$(OUT) $(args)

build: $(obj)
	$(CC) $(FLAGS) $(wildcard $(bin)/*.o) -o $(OUT) $(LIBS) 

%.o: %.c
	-@mkdir -p $(bin)/
	$(CC) $(FLAGS) $< -c -o $(bin)/$(@F) $(LIBS) 

clean:
	@rm $(OUT)
	@rm -r $(bin)/
	@rm *.o
