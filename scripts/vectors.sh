#!/bin/bash

# GeneLLM ベクトル表示ツール
# 使用方法: ./vectors.sh [limit] [offset]

WORKSPACE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." &> /dev/null && pwd)"
VECTOR_FILE="$WORKSPACE_DIR/data/word_vectors.dat"

# デフォルト値
LIMIT=${1:-10}
OFFSET=${2:-0}

# ベクトルデータを表示
show_vectors() {
    local limit=$1
    local offset=$2
    
    if [ ! -f "$VECTOR_FILE" ]; then
        echo "エラー: ベクトルデータファイルが見つかりません: $VECTOR_FILE"
        exit 1
    fi
    
    echo "単語ベクトルデータ（位置 $offset から $limit 件表示）："
    echo "----------------------------------------"
    
    # ファイルの行数を取得
    local total_lines=$(wc -l < "$VECTOR_FILE")
    echo "総単語数: $total_lines"
    echo ""
    
    # 指定された範囲の行を表示
    if [ "$offset" -ge "$total_lines" ]; then
        echo "エラー: 指定されたオフセット($offset)が総単語数($total_lines)を超えています"
        exit 1
    fi
    
    head -n $((offset + limit)) "$VECTOR_FILE" | tail -n $limit | while read -r line; do
        # 単語と最初の数個のベクトル値を表示
        word=$(echo "$line" | cut -d' ' -f1)
        vector_preview=$(echo "$line" | cut -d' ' -f2-11 | tr ' ' ', ')
        echo "単語: $word"
        echo "ベクトル: [$vector_preview, ...]"
        echo "----------------------------------------"
    done
}

# ヘルプを表示
show_help() {
    cat << EOF
使用方法: ./vectors.sh [limit] [offset]

説明:
  単語ベクトルデータを表示します。

引数:
  limit   表示する単語の数 (デフォルト: 10)
  offset  表示を開始する位置 (デフォルト: 0)

例:
  ./vectors.sh
  ./vectors.sh 20
  ./vectors.sh 5 100
EOF
}

# メイン処理
if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    show_help
else
    show_vectors "$LIMIT" "$OFFSET"
fi