CFLAGS  := $(shell /usr/bin/xeno-config --skin=alchemy --cflags)
LDFLAGS := $(shell /usr/bin/xeno-config --skin=alchemy --ldflags)

CC = gcc
TARGET = demo_task

all: $(TARGET)

$(TARGET): demo_task.c
	$(CC) -o $(TARGET) demo_task.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
