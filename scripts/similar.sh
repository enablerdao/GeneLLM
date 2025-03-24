#!/bin/bash

# GeneLLM ベクトル類似度検索ツール
# 使用方法: ./similar.sh [コマンド] [引数]

WORKSPACE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." &> /dev/null && pwd)"
SCRIPTS_DIR="$WORKSPACE_DIR/scripts"
WORD_LIST_FILE="$WORKSPACE_DIR/knowledge/text/japanese_words.txt"
VECTOR_FILE="$WORKSPACE_DIR/data/word_vectors.dat"
TEMP_DIR="/tmp/genellm_similar"

# 一時ディレクトリを作成
mkdir -p "$TEMP_DIR"

# 単語のベクトルを取得
get_word_vector() {
    local word="$1"
    local output_file="$2"
    
    # 単語のIDを取得
    local word_id=$(grep -n "^$word$" "$WORD_LIST_FILE" | cut -d':' -f1)
    
    if [ -z "$word_id" ]; then
        echo "エラー: 単語「$word」が単語リストに見つかりません"
        return 1
    fi
    
    # 単語IDを0ベースに変換
    word_id=$((word_id - 1))
    
    # ベクトルファイルから該当する単語のベクトルを抽出
    "$WORKSPACE_DIR/bin/vectors" 1 "$word_id" | grep "ベクトル:" | sed 's/ベクトル: \[\(.*\), \.\.\.\]/\1/' > "$output_file"
    
    if [ ! -s "$output_file" ]; then
        echo "エラー: 単語「$word」のベクトルを取得できませんでした"
        return 1
    fi
    
    return 0
}

# コサイン類似度を計算するPythonスクリプト
calculate_cosine_similarity() {
    local vector1_file="$1"
    local vector2_file="$2"
    
    python3 -c "
import numpy as np

def cosine_similarity(v1, v2):
    dot_product = np.dot(v1, v2)
    norm_v1 = np.linalg.norm(v1)
    norm_v2 = np.linalg.norm(v2)
    return dot_product / (norm_v1 * norm_v2)

# ベクトルを読み込む
with open('$vector1_file', 'r') as f:
    vector1_str = f.read().strip()
    vector1 = np.array([float(x) for x in vector1_str.split(',')])

with open('$vector2_file', 'r') as f:
    vector2_str = f.read().strip()
    vector2 = np.array([float(x) for x in vector2_str.split(',')])

# コサイン類似度を計算
similarity = cosine_similarity(vector1, vector2)
print(f'{similarity:.6f}')
"
}

# 単語の類似度を検索
find_similar_words() {
    local word="$1"
    local limit="${2:-10}"
    
    if [ -z "$word" ]; then
        echo "エラー: 単語が指定されていません"
        exit 1
    fi
    
    echo "単語「$word」に類似した単語を検索中..."
    echo "----------------------------------------"
    
    # 検索単語のベクトルを取得
    local query_vector_file="$TEMP_DIR/query_vector.txt"
    if ! get_word_vector "$word" "$query_vector_file"; then
        exit 1
    fi
    
    # ベクトルの内容を確認
    if [ ! -s "$query_vector_file" ]; then
        echo "エラー: ベクトルファイルが空です"
        exit 1
    fi
    
    # ベクトルの内容をデバッグ表示
    echo "ベクトルデータを取得しました"
    
    # 単語リストの総数を取得
    local total_words=$(wc -l < "$WORD_LIST_FILE")
    
    # 結果を一時ファイルに保存
    local results_file="$TEMP_DIR/similarity_results.txt"
    > "$results_file"
    
    # 進捗表示
    echo "単語リスト内の${total_words}単語と比較します..."
    
    # 各単語との類似度を計算
    for ((i=1; i<=total_words; i++)); do
        
        # 単語を取得
        local current_word=$(sed -n "${i}p" "$WORD_LIST_FILE")
        
        # 検索単語自身はスキップ
        if [ "$current_word" = "$word" ]; then
            continue
        fi
        
        # 現在の単語のベクトルを取得
        local current_vector_file="$TEMP_DIR/current_vector.txt"
        if get_word_vector "$current_word" "$current_vector_file" 2>/dev/null; then
            # 類似度を計算
            local similarity=$(calculate_cosine_similarity "$query_vector_file" "$current_vector_file")
            
            # 結果を保存
            echo "$similarity $current_word" >> "$results_file"
        fi
    done
    
    echo "計算完了"
    echo ""
    
    # 結果を類似度順にソートして表示
    echo "「$word」に最も類似した単語（上位${limit}件）："
    echo "----------------------------------------"
    if [ -s "$results_file" ]; then
        sort -nr "$results_file" | head -n "$limit" | while read -r line; do
            similarity=$(echo "$line" | cut -d' ' -f1)
            similar_word=$(echo "$line" | cut -d' ' -f2-)
            printf "%.4f\t%s\n" "$similarity" "$similar_word"
        done
    else
        echo "類似した単語が見つかりませんでした。"
    fi
    
    # 一時ファイルを削除（デバッグ中はコメントアウト）
    # rm -f "$query_vector_file" "$current_vector_file" "$results_file"
}

# 単語間の類似度を計算
calculate_word_similarity() {
    local word1="$1"
    local word2="$2"
    
    if [ -z "$word1" ] || [ -z "$word2" ]; then
        echo "エラー: 2つの単語を指定してください"
        exit 1
    fi
    
    echo "単語「$word1」と「$word2」の類似度を計算中..."
    echo "----------------------------------------"
    
    # 単語1のベクトルを取得
    local vector1_file="$TEMP_DIR/vector1.txt"
    if ! get_word_vector "$word1" "$vector1_file"; then
        exit 1
    fi
    
    # 単語2のベクトルを取得
    local vector2_file="$TEMP_DIR/vector2.txt"
    if ! get_word_vector "$word2" "$vector2_file"; then
        exit 1
    fi
    
    # 類似度を計算
    local similarity=$(calculate_cosine_similarity "$vector1_file" "$vector2_file")
    
    echo "類似度: $similarity"
    
    # 一時ファイルを削除
    rm -f "$vector1_file" "$vector2_file"
}

# ヘルプを表示
show_help() {
    cat << EOF
使用方法: ./similar.sh [コマンド] [引数]

コマンド:
  find "単語" [limit]        指定した単語に類似した単語を検索
  compare "単語1" "単語2"    2つの単語間の類似度を計算
  help                      このヘルプを表示

引数:
  "単語"                    検索する単語
  limit                     表示する類似単語の数 (デフォルト: 10)
  "単語1" "単語2"           類似度を計算する2つの単語

例:
  ./similar.sh find "猫" 5       # 「猫」に類似した単語を5件表示
  ./similar.sh compare "猫" "犬"  # 「猫」と「犬」の類似度を計算
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
    find)
        if [ $# -ge 1 ]; then
            word="$1"
            limit="${2:-10}"
            find_similar_words "$word" "$limit"
        else
            echo "エラー: 単語が指定されていません"
            show_help
            exit 1
        fi
        ;;
    compare)
        if [ $# -ge 2 ]; then
            word1="$1"
            word2="$2"
            calculate_word_similarity "$word1" "$word2"
        else
            echo "エラー: 2つの単語を指定してください"
            show_help
            exit 1
        fi
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

# 一時ディレクトリを削除
rm -rf "$TEMP_DIR"