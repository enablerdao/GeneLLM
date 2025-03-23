CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lm

.PHONY: all clean

all: syntax_analyzer simple_analyzer dna_compressor combined_analyzer vector_search graph_generator router_model main

syntax_analyzer: syntax_analyzer.c
$(CC) $(CFLAGS) -o $@ $<

simple_analyzer: simple_analyzer.c
$(CC) $(CFLAGS) -o $@ $<

dna_compressor: dna_compressor.c
$(CC) $(CFLAGS) -o $@ $<

combined_analyzer: combined_analyzer.c
$(CC) $(CFLAGS) -o $@ $<

vector_search: vector_search.c
$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

graph_generator: graph_generator.c
$(CC) $(CFLAGS) -o $@ $<

router_model: router_model.c
$(CC) $(CFLAGS) -o $@ $<

main: main.c
$(CC) $(CFLAGS) -o $@ $<

clean:
rm -f syntax_analyzer simple_analyzer dna_compressor combined_analyzer vector_search graph_generator router_model main
