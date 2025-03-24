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
mkdir -p data/models
mkdir -p data/vector_db
mkdir -p logs
touch data/learning_db.txt

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
echo "Building main program..."

# ビルドスクリプトが存在する場合はそれを使用
if [ -f "build.sh" ]; then
    chmod +x build.sh
    ./build.sh
else
    # macOSの場合はMeCabのパスを検出
    MECAB_CFLAGS=""
    MECAB_LDFLAGS=""
    COMPILER="gcc"
    
    if [[ "$(uname -s)" == "Darwin" ]]; then
        # macOSではclangを使用
        COMPILER="clang"
        
        # mecab-configが利用可能か確認
        if command -v mecab-config &> /dev/null; then
            MECAB_CFLAGS="-I$(mecab-config --inc-dir)"
            MECAB_LDFLAGS="-L$(mecab-config --libs-only-L | sed 's/-L//g')"
        # Homebrewのデフォルトパスを試す
        elif [ -d "/usr/local/include/mecab" ]; then
            MECAB_CFLAGS="-I/usr/local/include"
            MECAB_LDFLAGS="-L/usr/local/lib"
        elif [ -d "/opt/homebrew/include/mecab" ]; then
            MECAB_CFLAGS="-I/opt/homebrew/include"
            MECAB_LDFLAGS="-L/opt/homebrew/lib"
        fi
        
        # macOS特有のフラグを追加
        CFLAGS="$MECAB_CFLAGS -arch $(uname -m)"
        LDFLAGS="$MECAB_LDFLAGS -arch $(uname -m)"
    else
        CFLAGS="$MECAB_CFLAGS"
        LDFLAGS="$MECAB_LDFLAGS"
    fi
    
    # src_newディレクトリが存在する場合は新しいソースを使用
    if [ -d "src_new" ]; then
        echo "新しいソースコードを使用してビルドします..."
        $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src_new/main_simple.c src_new/include/vector_db.c src_new/vector_search/vector_search.c src_new/vector_search/vector_search_global.c src_new/include/word_loader/word_loader.c $LDFLAGS -lmecab -lm -lcurl
    else
        # 従来のソースコードを使用
        $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/include/word_loader.c $LDFLAGS -lmecab -lm -lcurl
    fi
fi

# bin ディレクトリにもコピー
if [ -f gllm ]; then
    cp gllm bin/main
fi

# 個別モジュールをビルド
echo "個別モジュールをビルドしています..."
echo "Building individual modules..."

# macOSでのmecab.hが見つからない問題を解決するための追加チェック
if [[ "$(uname -s)" == "Darwin" ]]; then
    # mecab-configが利用可能かどうかを確認
    if command -v mecab-config >/dev/null 2>&1; then
        echo "mecab-configを使用してMeCabのパスを検出します..."
        MECAB_INCLUDE_PATH=$(mecab-config --inc-dir)
        echo "MeCabインクルードパス: $MECAB_INCLUDE_PATH"
        
        # mecab.hファイルが存在するか確認
        if [ -f "$MECAB_INCLUDE_PATH/mecab.h" ]; then
            echo "mecab.hが見つかりました: $MECAB_INCLUDE_PATH/mecab.h"
        else
            echo "警告: $MECAB_INCLUDE_PATH/mecab.h が見つかりません"
            
            # 一般的なパスを確認
            POSSIBLE_PATHS=(
                "/usr/local/include"
                "/usr/local/include/mecab"
                "/opt/homebrew/include"
                "/opt/homebrew/include/mecab"
                "/usr/include"
                "/usr/include/mecab"
                "$(brew --prefix 2>/dev/null)/include"
                "$(brew --prefix 2>/dev/null)/include/mecab"
            )
            
            for path in "${POSSIBLE_PATHS[@]}"; do
                if [ -f "$path/mecab.h" ]; then
                    echo "mecab.hが見つかりました: $path/mecab.h"
                    MECAB_INCLUDE_PATH="$path"
                    CFLAGS="-I$MECAB_INCLUDE_PATH -arch $(uname -m)"
                    break
                fi
            done
        fi
    fi
fi

# macOSの場合はMeCabのパスを検出
MECAB_CFLAGS=""
MECAB_LDFLAGS=""
COMPILER="gcc"

if [[ "$(uname -s)" == "Darwin" ]]; then
    # macOSではclangを使用
    COMPILER="clang"
    
    # mecab-configが利用可能か確認
    if command -v mecab-config &> /dev/null; then
        MECAB_CFLAGS="-I$(mecab-config --inc-dir)"
        MECAB_LDFLAGS="-L$(mecab-config --libs-only-L | sed 's/-L//g')"
    # Homebrewのデフォルトパスを試す
    elif [ -d "/usr/local/include/mecab" ]; then
        MECAB_CFLAGS="-I/usr/local/include"
        MECAB_LDFLAGS="-L/usr/local/lib"
    elif [ -d "/opt/homebrew/include/mecab" ]; then
        MECAB_CFLAGS="-I/opt/homebrew/include"
        MECAB_LDFLAGS="-L/opt/homebrew/lib"
    fi
    
    # macOS特有のフラグを追加
    CFLAGS="$MECAB_CFLAGS -arch $(uname -m)"
    LDFLAGS="$MECAB_LDFLAGS -arch $(uname -m)"
else
    CFLAGS="$MECAB_CFLAGS"
    LDFLAGS="$MECAB_LDFLAGS"
fi

# ファイルが存在する場合のみビルド
if [ -f src/analyzers/simple_analyzer.c ]; then
    $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o bin/simple_analyzer src/analyzers/simple_analyzer.c $LDFLAGS -lmecab
fi

if [ -f src/compressors/dna_compressor.c ]; then
    $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o bin/dna_compressor src/compressors/dna_compressor.c $LDFLAGS
fi

if [ -f src/vector_search/vector_search.c ]; then
    $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o bin/vector_search src/vector_search/vector_search.c $LDFLAGS -lm
fi

if [ -f src/generators/graph_generator.c ]; then
    $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o bin/graph_generator src/generators/graph_generator.c $LDFLAGS
fi

if [ -f src/routers/router_model.c ]; then
    $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o bin/router_model src/routers/router_model.c $LDFLAGS -lmecab
fi

echo "Build completed."
echo "To run: ./gllm [options]"

# 最初の質問を実行
echo "💬 最初の質問を実行します..."
echo "============================================================"
./gllm "C言語でHello Worldを表示するプログラムを教えてください"

echo "============================================================"
echo "✅ クイックスタートが完了しました！"
echo "他の質問を試すには: ./gllm \"あなたの質問\""
echo "デバッグモードで実行するには: ./gllm -d \"あなたの質問\""
echo "対話モードで実行するには: ./gllm -i"
