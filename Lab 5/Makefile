CC = gcc

CFLAGS = $(shell xeno-config --alchemy --cflags)

LDFLAGS = $(shell xeno-config --alchemy --ldflags)

all:
	$(CC) taskno.c -o taskno $(CFLAGS) $(LDFLAGS)
