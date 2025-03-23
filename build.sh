#!/bin/bash

# Compile all programs
gcc -Wall -Wextra -std=c99 -o syntax_analyzer syntax_analyzer.c
gcc -Wall -Wextra -std=c99 -o simple_analyzer simple_analyzer.c
gcc -Wall -Wextra -std=c99 -o dna_compressor dna_compressor.c
gcc -Wall -Wextra -std=c99 -o combined_analyzer combined_analyzer.c
gcc -Wall -Wextra -std=c99 -o vector_search vector_search.c -lm
gcc -Wall -Wextra -std=c99 -o graph_generator graph_generator.c
gcc -Wall -Wextra -std=c99 -o router_model router_model.c
gcc -Wall -Wextra -std=c99 -o main main.c

echo "All programs compiled successfully!"
