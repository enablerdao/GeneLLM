#!/bin/bash

# GeneLLM トークン表示ツール
# 使用方法: ./tokens.sh [コマンド] [引数]

WORKSPACE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." &> /dev/null && pwd)"
WORD_LIST_FILE="$WORKSPACE_DIR/knowledge/text/japanese_words.txt"
VECTOR_FILE="$WORKSPACE_DIR/data/word_vectors.dat"

# デフォルト値
LIMIT=10
OFFSET=0
TEXT=""
MODE="words"

# 単語リストを表示
show_words() {
    local limit=$1
    local offset=$2
    
    if [ ! -f "$WORD_LIST_FILE" ]; then
        echo "エラー: 単語リストファイルが見つかりません: $WORD_LIST_FILE"
        exit 1
    fi
    
    echo "単語リスト（位置 $offset から $limit 件表示）："
    echo "----------------------------------------"
    
    # ファイルの行数を取得
    local total_lines=$(wc -l < "$WORD_LIST_FILE")
    echo "総単語数: $total_lines"
    echo ""
    
    # 指定された範囲の行を表示
    if [ "$offset" -ge "$total_lines" ]; then
        echo "エラー: 指定されたオフセット($offset)が総単語数($total_lines)を超えています"
        exit 1
    fi
    
    # 各単語とそのIDを表示
    head -n $((offset + limit)) "$WORD_LIST_FILE" | tail -n $limit | nl -v "$offset" -w4 -s': ' | while read -r line; do
        echo "$line"
    done
}

# テキストをトークナイズして表示
tokenize_text() {
    local text="$1"
    
    if [ -z "$text" ]; then
        echo "エラー: テキストが指定されていません"
        exit 1
    fi
    
    echo "テキスト「$text」のトークン化結果："
    echo "----------------------------------------"
    
    # MeCabを使用してトークナイズ
    if ! command -v mecab &> /dev/null; then
        echo "エラー: MeCabがインストールされていません"
        exit 1
    fi
    
    # トークナイズ結果を表示
    echo "表層形\t品詞\t品詞細分類\t基本形"
    echo "----------------------------------------"
    echo "$text" | mecab -Ochasen | grep -v "EOS" | while read -r line; do
        surface=$(echo "$line" | cut -f1)
        pos=$(echo "$line" | cut -f4)
        pos_detail=$(echo "$line" | cut -f5)
        base=$(echo "$line" | cut -f3)
        
        # 単語IDを検索
        word_id=$(grep -n "^$base$" "$WORD_LIST_FILE" | cut -d':' -f1)
        if [ -z "$word_id" ]; then
            word_id="未登録"
        else
            # 0ベースのインデックスに調整（数値チェック）
            if [[ "$word_id" =~ ^[0-9]+$ ]]; then
                word_id=$((word_id - 1))
            else
                word_id="未登録"
            fi
        fi
        
        printf "%-10s\t%-10s\t%-15s\t%-10s\t(ID: %s)\n" "$surface" "$pos" "$pos_detail" "$base" "$word_id"
    done
}

# ヘルプを表示
show_help() {
    cat << EOF
使用方法: ./tokens.sh [コマンド] [引数]

コマンド:
  words [limit] [offset]    単語リストを表示
  tokenize "テキスト"       テキストをトークナイズして表示
  help                     このヘルプを表示

引数:
  limit                    表示する単語の数 (デフォルト: 10)
  offset                   表示を開始する位置 (デフォルト: 0)
  "テキスト"               トークナイズするテキスト

例:
  ./tokens.sh words                  # 単語リストを表示（デフォルト10件）
  ./tokens.sh words 20               # 単語リストを20件表示
  ./tokens.sh words 5 100            # 単語リストを位置100から5件表示
  ./tokens.sh tokenize "こんにちは世界"  # テキストをトークナイズして表示
EOF
}

# メイン処理
if [ $# -eq 0 ]; then
    show_help
    exit 0
fi

# コマンドを取得
COMMAND="$1"
shift

case "$COMMAND" in
    words)
        # 引数として直接数値が指定された場合
        if [[ $# -ge 1 && "$1" =~ ^[0-9]+$ ]]; then
            LIMIT="$1"
            shift
            if [[ $# -ge 1 && "$1" =~ ^[0-9]+$ ]]; then
                OFFSET="$1"
                shift
            fi
        fi
        show_words "$LIMIT" "$OFFSET"
        ;;
    tokenize)
        if [ $# -ge 1 ]; then
            TEXT="$1"
            shift
        fi
        tokenize_text "$TEXT"
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        echo "エラー: 未知のコマンド '$COMMAND'"
        show_help
        exit 1
        ;;
esac