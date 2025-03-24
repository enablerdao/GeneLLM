#!/bin/bash

# 必要なライブラリをチェックしてインストールする関数
check_and_install_dependencies() {
    echo "必要な依存関係をチェックしています..."
    
    # OS の種類を判定
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$NAME
    elif type lsb_release >/dev/null 2>&1; then
        OS=$(lsb_release -si)
    elif [ -f /etc/lsb-release ]; then
        . /etc/lsb-release
        OS=$DISTRIB_ID
    else
        OS=$(uname -s)
    fi
    
    echo "検出されたOS: $OS"
    
    # Debian/Ubuntu系
    if [[ "$OS" == *"Ubuntu"* ]] || [[ "$OS" == *"Debian"* ]]; then
        echo "Debian/Ubuntu系のシステムを検出しました"
        
        # MeCabがインストールされているか確認
        if ! dpkg -l | grep -q libmecab-dev; then
            echo "MeCabがインストールされていません。インストールします..."
            sudo apt-get update
            sudo apt-get install -y libmecab-dev mecab mecab-ipadic-utf8
        else
            echo "MeCabはすでにインストールされています"
        fi
        
        # libcurlがインストールされているか確認
        if ! dpkg -l | grep -q libcurl4-openssl-dev; then
            echo "libcurlがインストールされていません。インストールします..."
            sudo apt-get update
            sudo apt-get install -y libcurl4-openssl-dev
        else
            echo "libcurlはすでにインストールされています"
        fi
    
    # Red Hat/CentOS/Fedora系
    elif [[ "$OS" == *"CentOS"* ]] || [[ "$OS" == *"Red Hat"* ]] || [[ "$OS" == *"Fedora"* ]]; then
        echo "Red Hat/CentOS/Fedora系のシステムを検出しました"
        
        # MeCabがインストールされているか確認
        if ! rpm -qa | grep -q mecab-devel; then
            echo "MeCabがインストールされていません。インストールします..."
            sudo yum install -y mecab mecab-devel mecab-ipadic
        else
            echo "MeCabはすでにインストールされています"
        fi
        
        # libcurlがインストールされているか確認
        if ! rpm -qa | grep -q libcurl-devel; then
            echo "libcurlがインストールされていません。インストールします..."
            sudo yum install -y libcurl-devel
        else
            echo "libcurlはすでにインストールされています"
        fi
    
    # macOS
    elif [[ "$OS" == *"Darwin"* ]] || [[ "$OS" == *"Mac"* ]]; then
        echo "macOSを検出しました"
        
        # Homebrewがインストールされているか確認
        if ! command -v brew &> /dev/null; then
            echo "Homebrewがインストールされていません。インストールします..."
            /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        fi
        
        # MeCabがインストールされているか確認
        if ! brew list mecab &> /dev/null; then
            echo "MeCabがインストールされていません。インストールします..."
            brew install mecab mecab-ipadic
        else
            echo "MeCabはすでにインストールされています"
        fi
        
        # libcurlがインストールされているか確認
        if ! brew list curl &> /dev/null; then
            echo "curlがインストールされていません。インストールします..."
            brew install curl
        else
            echo "curlはすでにインストールされています"
        fi
    
    # その他のOS
    else
        echo "未サポートのOSです: $OS"
        echo "必要なライブラリを手動でインストールしてください:"
        echo "- MeCab (libmecab-dev)"
        echo "- libcurl (libcurl4-openssl-dev)"
    fi
    
    echo "依存関係のチェックが完了しました"
}

# 依存関係をチェックしてインストール
check_and_install_dependencies

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
if [ -f gllm ]; then
    cp gllm bin/main
fi

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
