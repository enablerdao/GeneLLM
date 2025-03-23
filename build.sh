#!/bin/bash

# Create directories if they don't exist
mkdir -p bin
mkdir -p data/knowledge_base
mkdir -p data/models
mkdir -p data/vector_db

# Build main program
echo "Building main program..."
gcc -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/include/word_loader.c -lmecab -lm -lcurl
# Also build to bin directory for compatibility
cp gllm bin/main

# Build individual modules
echo "Building individual modules..."

# Syntax analyzer
gcc -Wall -Wextra -std=c99 -o bin/simple_analyzer src/analyzers/simple_analyzer.c -lmecab

# DNA compressor
gcc -Wall -Wextra -std=c99 -o bin/dna_compressor src/compressors/dna_compressor.c

# Vector search
gcc -Wall -Wextra -std=c99 -o bin/vector_search src/vector_search/vector_search.c -lm

# Graph generator
gcc -Wall -Wextra -std=c99 -o bin/graph_generator src/generators/graph_generator.c

# Router model
gcc -Wall -Wextra -std=c99 -o bin/router_model src/routers/router_model.c -lmecab

echo "Build completed."
echo "To run: ./gllm [options]"
