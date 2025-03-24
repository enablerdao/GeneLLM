#!/bin/bash

# クイックスタートスクリプト
# このスクリプトは必要な依存関係をインストールし、プロジェクトをビルドして最初の質問を実行します

# エラーが発生したら停止
set -e

echo "🚀 C言語による超軽量AI実装プロジェクト - クイックスタート"
echo "============================================================"

# リポジトリをクローン（存在しない場合）
if [ ! -d "src" ]; then
  echo "📥 リポジトリをクローンしています..."
  git clone https://github.com/enablerdao/GeneLLM.git .
fi

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

# ビルドを実行
echo "🔨 プロジェクトをビルドしています..."

# ディレクトリを作成
mkdir -p bin
mkdir -p knowledge/base
mkdir -p knowledge/docs
mkdir -p knowledge/text/wikipedia
mkdir -p knowledge/text
mkdir -p knowledge/models
mkdir -p knowledge/vector_db
mkdir -p logs
touch knowledge/learning_db.txt

# 日本語単語リストのサンプルを作成（存在しない場合）
if [ ! -f knowledge/text/japanese_words.txt ]; then
  echo "日本語単語リストのサンプルを作成しています..."
  cat > knowledge/text/japanese_words.txt << EOL
こんにちは
世界
プログラミング
コンピュータ
人工知能
機械学習
自然言語処理
データベース
アルゴリズム
ネットワーク
EOL
fi

# メインプログラムをビルド
echo "メインプログラムをビルドしています..."
gcc -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/include/word_loader.c -lmecab -lm -lcurl
# bin ディレクトリにもコピー
if [ -f gllm ]; then
    cp gllm bin/main
fi

# 個別モジュールをビルド
echo "個別モジュールをビルドしています..."

# ファイルが存在する場合のみビルド
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

# 最初の質問を実行
echo "💬 最初の質問を実行します..."
echo "============================================================"
./gllm "C言語でHello Worldを表示するプログラムを教えてください"

echo "============================================================"
echo "✅ クイックスタートが完了しました！"
echo "他の質問を試すには: ./gllm \"あなたの質問\""
echo "デバッグモードで実行するには: ./gllm -d \"あなたの質問\""
echo "対話モードで実行するには: ./gllm -i"
