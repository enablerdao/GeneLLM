#!/bin/bash

# GeneLLMビルドスクリプト
# このスクリプトは、GeneLLMをビルドします

# エラーが発生したら停止
set -e

echo "🔨 GeneLLM ビルドスクリプト"
echo "=========================="

# 現在のディレクトリを保存
CURRENT_DIR=$(pwd)
# スクリプトのディレクトリを取得
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# プロジェクトのルートディレクトリを設定
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

# ルートディレクトリに移動
cd "$ROOT_DIR"

# OSの検出
OS="$(uname -s)"
echo "検出されたOS: $OS"

# コンパイラとフラグの設定
COMPILER="gcc"
CFLAGS=""
LDFLAGS=""

# OSに応じた設定
case "$OS" in
    Linux*)
        echo "Linuxを検出しました"
        # Linuxの場合の設定
        ;;
    Darwin*)
        echo "macOSを検出しました"
        # macOSの場合の設定
        ;;
    CYGWIN*|MINGW*|MSYS*)
        echo "Windowsを検出しました"
        # Windowsの場合の設定
        ;;
    *)
        echo "不明なOSです: $OS"
        ;;
esac

# 必要なディレクトリの作成
mkdir -p bin
mkdir -p data
mkdir -p logs
mkdir -p knowledge/text

# ビルド
echo "GeneLLMをビルドしています..."

# 改良版のソースコードを使用
if [ -f "src/main_improved.c" ]; then
    echo "改良版のソースコードを使用してビルドします..."
    echo "コンパイルコマンド: $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main_improved.c src/improved/include/vector_db.c src/improved/vector_search/vector_search.c src/improved/vector_search/vector_search_global.c src/improved/include/word_loader/word_loader.c src/improved/include/answer_db.c $LDFLAGS -lmecab -lm -lcurl"
    $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main_improved.c src/improved/include/vector_db.c src/improved/vector_search/vector_search.c src/improved/vector_search/vector_search_global.c src/improved/include/word_loader/word_loader.c src/improved/include/answer_db.c $LDFLAGS -lmecab -lm -lcurl
else
    # 従来のソースコードを使用
    echo "従来のソースコードを使用してビルドします..."
    echo "コンパイルコマンド: $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/vector_search/vector_search_global.c src/include/word_loader/word_loader.c $LDFLAGS -lmecab -lm -lcurl"
    $COMPILER $CFLAGS -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/vector_search/vector_search_global.c src/include/word_loader/word_loader.c $LDFLAGS -lmecab -lm -lcurl
fi

# 実行ファイルをbinディレクトリにコピー
cp gllm bin/

# genellm、gl、gm のシンボリックリンクを作成
ln -sf gllm genellm
ln -sf gllm gl
ln -sf gllm gm

# シンボリックリンクもbinディレクトリにコピー
cp genellm bin/
cp gl bin/
cp gm bin/

echo "ビルドが完了しました。"
echo "実行方法: ./gllm \"質問文\" または ./genellm \"質問文\" または ./gl \"質問文\" または ./gm \"質問文\""

# 元のディレクトリに戻る
cd "$CURRENT_DIR"