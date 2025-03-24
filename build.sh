#!/bin/bash

# Create directories if they don't exist
mkdir -p bin
mkdir -p data/knowledge_base
mkdir -p data/models
mkdir -p data/vector_db
mkdir -p logs

# Build main program
echo "Building main program..."
gcc -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/include/word_loader.c -lmecab -lm -lcurl
# Also build to bin directory for compatibility
cp gllm bin/main

# Build individual modules
echo "Building individual modules..."

# Check if files exist before building
if [ -f src/analyzers/simple_analyzer.c ]; then
    gcc -Wall -Wextra -std=c99 -o bin/simple_analyzer src/analyzers/simple_analyzer.c -lmecab
fi

if [ -f src/compressors/dna_compressor.c ]; then
    gcc -Wall -Wextra -std=c99 -o bin/dna_compressor src/compressors/dna_compressor.c
fi

if [ -f src/vector_search/vector_search.c ]; then
    gcc -Wall -Wextra -std=c99 -o bin/vector_search src/vector_search/vector_search.c -lm
fi

if [ -f src/generators/graph_generator.c ]; then
    gcc -Wall -Wextra -std=c99 -o bin/graph_generator src/generators/graph_generator.c
fi

if [ -f src/routers/router_model.c ]; then
    gcc -Wall -Wextra -std=c99 -o bin/router_model src/routers/router_model.c -lmecab
fi

echo "Build completed."
echo "To run: ./gllm [options]"
