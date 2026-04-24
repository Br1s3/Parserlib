CC = gcc
RM = rm -rf
CFLAGS += \
-Wall 	  \
-Wextra   \
-g3

main: main.c
	$(CC) $< -o $@ $(CFLAGS)

.PHONY: all clean

all: main

clean: 
	$(RM) main
