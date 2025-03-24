#!/bin/bash

# ナレッジとドキュメントの内容を読み込み、トークナイズするスクリプト（並列処理版）
# 使用方法: ./tokenize_knowledge_parallel.sh

# 作業ディレクトリを設定
WORKSPACE="/workspace"
KNOWLEDGE_DIR="$WORKSPACE/knowledge"
DOCS_DIR="$WORKSPACE/docs"
TEMP_DIR="/tmp/genellm_tokenize"
OUTPUT_DIR="$WORKSPACE/data/tokenized"

# 必要なディレクトリを作成
mkdir -p "$TEMP_DIR"
mkdir -p "$OUTPUT_DIR"

# トークナイザーの存在確認
if [ ! -f "$WORKSPACE/bin/tokens" ]; then
    echo "エラー: トークナイザーが見つかりません。"
    exit 1
fi

# 処理対象ファイルのリストを作成
echo "処理対象ファイルを検索しています..."
find "$KNOWLEDGE_DIR" "$DOCS_DIR" -type f \( -name "*.txt" -o -name "*.md" \) | grep -v "answers.txt" | grep -v "answers_new.txt" | grep -v "answers_additional.txt" > "$TEMP_DIR/target_files.txt"

# ファイル数を取得
FILE_COUNT=$(wc -l < "$TEMP_DIR/target_files.txt")
echo "合計 $FILE_COUNT 個のファイルを処理します。"

# 並列処理のための関数
process_file() {
    local file="$1"
    local index="$2"
    local PROCESSED_COUNT="$index"
    
    # 出力ファイル名を生成
    local REL_PATH=$(realpath --relative-to="$WORKSPACE" "$file")
    local OUTPUT_FILE="$OUTPUT_DIR/${REL_PATH//\//_}.tokens"
    
    echo "[$PROCESSED_COUNT/$FILE_COUNT] ファイル $file を処理中..."
    
    # ファイルの内容を取得
    local CONTENT=$(cat "$file")
    
    # 内容が空でない場合のみ処理
    if [ -n "$CONTENT" ]; then
        # ファイルの拡張子を取得
        local EXT="${file##*.}"
        
        # マークダウンファイルの場合、マークダウン記法を除去
        if [ "$EXT" == "md" ]; then
            # マークダウン見出しを通常のテキストに変換
            CONTENT=$(echo "$CONTENT" | sed 's/^#\+\s\+//')
            
            # コードブロックを除去
            CONTENT=$(echo "$CONTENT" | sed '/^```/,/^```/d')
            
            # リンク記法を除去 [text](url) -> text
            CONTENT=$(echo "$CONTENT" | sed -E 's/\[([^]]*)\]\([^)]*\)/\1/g')
            
            # 強調記法を除去 **text** -> text
            CONTENT=$(echo "$CONTENT" | sed 's/\*\*\([^*]*\)\*\*/\1/g')
            
            # イタリック記法を除去 *text* -> text
            CONTENT=$(echo "$CONTENT" | sed 's/\*\([^*]*\)\*/\1/g')
        fi
        
        # テキストファイルの場合、質問|回答形式を処理
        if [ "$EXT" == "txt" ]; then
            # 質問|回答形式の場合、両方を含める
            if grep -q "|" <<< "$CONTENT"; then
                # 質問と回答を抽出
                local QUESTIONS=$(echo "$CONTENT" | grep -E "^[^#|]+\|" | cut -d'|' -f1)
                local ANSWERS=$(echo "$CONTENT" | grep -E "^[^#|]+\|" | cut -d'|' -f2-)
                
                # 質問と回答を結合（空でない場合のみ）
                if [ -n "$QUESTIONS" ] && [ -n "$ANSWERS" ]; then
                    CONTENT="$QUESTIONS\n\n$ANSWERS"
                fi
            fi
        fi
        
        # 内容が空白文字のみの場合はスキップ
        if [ -z "$(echo "$CONTENT" | tr -d '[:space:]')" ]; then
            echo "  スキップ: 内容が空白文字のみです: $file"
            return
        fi
        
        # トークナイズ処理
        # 大きなファイルの場合は分割して処理
        if [ ${#CONTENT} -gt 10000 ]; then
            echo "  大きなファイルを分割して処理します: $file"
            # 内容をファイルに保存
            local TEMP_CONTENT_FILE="$TEMP_DIR/temp_content_$PROCESSED_COUNT.txt"
            echo -e "$CONTENT" > "$TEMP_CONTENT_FILE"
            
            # 最初の10000文字だけをトークナイズ
            head -c 10000 "$TEMP_CONTENT_FILE" | "$WORKSPACE/bin/tokens" tokenize "$(head -c 10000 "$TEMP_CONTENT_FILE")" > "$OUTPUT_FILE"
            
            # 一時ファイルを削除
            rm -f "$TEMP_CONTENT_FILE"
        else
            # 通常のトークナイズ処理
            "$WORKSPACE/bin/tokens" tokenize "$CONTENT" > "$OUTPUT_FILE"
        fi
        
        # トークナイズ結果の確認
        if [ -s "$OUTPUT_FILE" ]; then
            echo "  トークナイズ成功: $OUTPUT_FILE"
        else
            echo "  警告: トークナイズ結果が空です: $file"
            rm -f "$OUTPUT_FILE"
        fi
    else
        echo "  スキップ: ファイルが空です: $file"
    fi
}

# 並列処理の最大数
MAX_PARALLEL=8
RUNNING=0
INDEX=1

# ファイルを並列処理
while read -r file; do
    # 並列処理の数が最大に達した場合、いずれかの処理が終わるまで待機
    if [ $RUNNING -ge $MAX_PARALLEL ]; then
        wait -n
        RUNNING=$((RUNNING - 1))
    fi
    
    # バックグラウンドで処理を実行
    process_file "$file" "$INDEX" &
    RUNNING=$((RUNNING + 1))
    INDEX=$((INDEX + 1))
done < "$TEMP_DIR/target_files.txt"

# すべての処理が終わるまで待機
wait

# 処理完了メッセージ
TOKENIZED_COUNT=$(find "$OUTPUT_DIR" -type f | wc -l)
echo "処理完了: $TOKENIZED_COUNT/$FILE_COUNT ファイルがトークナイズされました。"
echo "トークナイズされたファイルは $OUTPUT_DIR に保存されました。"

# 単語ベクトルの更新
echo "単語ベクトルを更新しています..."
"$WORKSPACE/bin/update-vectors" "$OUTPUT_DIR"

# 一時ファイルを削除
rm -f "$TEMP_DIR/target_files.txt"

echo "ナレッジとドキュメントのトークナイズが完了しました。"