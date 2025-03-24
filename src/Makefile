CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
LDFLAGS = -pthread

all: knowledge_manager

knowledge_manager: knowledge_manager.o
	$(CC) $(LDFLAGS) -o $@ $^

knowledge_manager.o: knowledge_manager.c knowledge_manager.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f knowledge_manager *.o

.PHONY: all clean