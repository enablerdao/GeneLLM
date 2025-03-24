CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c99
LDFLAGS = -lm

all: dna_search

dna_search: dna_search.c
	$(CC) $(CFLAGS) -o dna_search dna_search.c $(LDFLAGS)

clean:
	rm -f dna_search

install: dna_search
	mkdir -p ../../bin
	cp dna_search ../../bin/

.PHONY: all clean install