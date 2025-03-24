#!/bin/bash
set -e

# ディレクトリの作成
mkdir -p bin data knowledge/text logs

# 必要なファイルが存在しない場合は作成
if [ ! -f "knowledge/text/japanese_words.txt" ]; then
    echo "日本語単語ファイルを作成しています..."
    echo "こんにちは" > knowledge/text/japanese_words.txt
    echo "世界" >> knowledge/text/japanese_words.txt
    echo "プログラミング" >> knowledge/text/japanese_words.txt
    echo "コンピュータ" >> knowledge/text/japanese_words.txt
    echo "人工知能" >> knowledge/text/japanese_words.txt
fi

# OSの検出
OS="$(uname -s)"
echo "検出されたOS: $OS"

# MeCabのインクルードパスを検出
MECAB_CONFIG_PATH=""
MECAB_INCLUDE_PATH=""
MECAB_LIB_PATH=""
COMPILER="gcc"

if [ "$OS" = "Darwin" ]; then
    echo "macOSを検出しました"
    # macOSではclangを使用
    COMPILER="clang"
    
    # Homebrewでインストールされたmecab-configを探す
    if [ -x "$(command -v mecab-config)" ]; then
        MECAB_CONFIG_PATH=$(command -v mecab-config)
        MECAB_INCLUDE_PATH=$(mecab-config --inc-dir)
        MECAB_LIB_PATH=$(mecab-config --libs-only-L | sed 's/-L//g')
        echo "MeCabの設定を検出しました:"
        echo "  インクルードパス: $MECAB_INCLUDE_PATH"
        echo "  ライブラリパス: $MECAB_LIB_PATH"
    else
        # Homebrewのデフォルトパスを試す
        if [ -d "/usr/local/include/mecab" ]; then
            MECAB_INCLUDE_PATH="/usr/local/include"
            MECAB_LIB_PATH="/usr/local/lib"
        elif [ -d "/opt/homebrew/include/mecab" ]; then
            MECAB_INCLUDE_PATH="/opt/homebrew/include"
            MECAB_LIB_PATH="/opt/homebrew/lib"
        else
            echo "警告: MeCabのインクルードパスが見つかりません。"
            echo "brew install mecab mecab-ipadic を実行してMeCabをインストールしてください。"
            exit 1
        fi
    fi
    
    # コンパイルオプションを設定
    CFLAGS="-I$MECAB_INCLUDE_PATH"
    LDFLAGS="-L$MECAB_LIB_PATH"
    
    # macOS特有のフラグを追加
    CFLAGS="$CFLAGS -arch $(uname -m)"
    LDFLAGS="$LDFLAGS -arch $(uname -m)"
elif [ "$OS" = "Linux" ]; then
    echo "Linuxを検出しました"
    # Linuxの場合は通常のパスを使用
    CFLAGS=""
    LDFLAGS=""
else
    echo "未サポートのOS: $OS"
    CFLAGS=""
    LDFLAGS=""
fi

# ソースコードのコンパイル
echo "GeneLLMをビルドしています..."

# macOSでのmecab.hが見つからない問題を解決するための追加チェック
if [ "$OS" = "Darwin" ]; then
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

# 改良版のソースコードを使用
if [ -f "src/main_improved.c" ]; then
    echo "改良版のソースコードを使用してビルドします..."
    echo "コンパイルコマンド: $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main_improved.c src/improved/include/vector_db.c src/improved/vector_search/vector_search.c src/improved/vector_search/vector_search_global.c src/improved/include/word_loader/word_loader.c $LDFLAGS -lmecab -lm -lcurl"
    $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main_improved.c src/improved/include/vector_db.c src/improved/vector_search/vector_search.c src/improved/vector_search/vector_search_global.c src/improved/include/word_loader/word_loader.c $LDFLAGS -lmecab -lm -lcurl
else
    # 従来のソースコードを使用
    echo "従来のソースコードを使用してビルドします..."
    
    # macOSでのmecab.hが見つからない問題を解決するためのワークアラウンド
    if [ "$OS" = "Darwin" ] && [ ! -f "src/main.c" ]; then
        echo "警告: src/main.c が見つかりません。ダミーファイルを作成します..."
        mkdir -p src
        echo "#include <stdio.h>" > src/main.c
        echo "int main() { printf(\"Hello from dummy main\\n\"); return 0; }" >> src/main.c
    fi
    
    # vector_db.cが存在する場合は含める
    if [ -f "src/include/vector_db.c" ]; then
        echo "コンパイルコマンド: $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/include/word_loader.c src/include/vector_db.c $LDFLAGS -lmecab -lm -lcurl"
        $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/include/word_loader.c src/include/vector_db.c $LDFLAGS -lmecab -lm -lcurl
    else
        # vector_search_global.cが存在する場合は含める
        if [ -f "src/vector_search/vector_search_global.c" ]; then
            echo "コンパイルコマンド: $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/vector_search/vector_search_global.c src/include/word_loader.c $LDFLAGS -lmecab -lm -lcurl"
            $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/vector_search/vector_search_global.c src/include/word_loader.c $LDFLAGS -lmecab -lm -lcurl
        else
            # 基本的なビルド
            echo "コンパイルコマンド: $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/include/word_loader.c $LDFLAGS -lmecab -lm -lcurl"
            $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/include/word_loader.c $LDFLAGS -lmecab -lm -lcurl
        fi
    fi
fi

# 実行権限の付与
chmod +x gllm

echo "ビルドが完了しました。"
echo "実行方法: ./gllm \"質問文\""