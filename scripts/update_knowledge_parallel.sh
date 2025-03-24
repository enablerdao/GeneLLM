#!/bin/bash

# knowledgeとdocsディレクトリから知識を更新するスクリプト（並列処理版）
# 使用方法: ./update_knowledge_parallel.sh

# 作業ディレクトリを設定
WORKSPACE="/workspace"
KNOWLEDGE_DIR="$WORKSPACE/knowledge"
DOCS_DIR="$WORKSPACE/docs"
ANSWERS_FILE="$KNOWLEDGE_DIR/base/answers.txt"
TEMP_DIR="/tmp/knowledge_update"

# 一時ディレクトリを作成
mkdir -p "$TEMP_DIR"

# 既存の回答ファイルをバックアップ
cp "$ANSWERS_FILE" "${ANSWERS_FILE}.bak"

# 処理済みファイルのリストを保存するファイル
PROCESSED_FILES="$TEMP_DIR/processed_files.txt"
touch "$PROCESSED_FILES"

# 前回の実行時に処理したファイルのリストを読み込む
if [ -f "$PROCESSED_FILES" ]; then
    PREV_PROCESSED_FILES=$(cat "$PROCESSED_FILES")
else
    PREV_PROCESSED_FILES=""
fi

# 現在の処理対象ファイルのリストを作成
find "$KNOWLEDGE_DIR" "$DOCS_DIR" -type f \( -name "*.txt" -o -name "*.md" \) > "$TEMP_DIR/current_files.txt"

# 新しいファイルまたは変更されたファイルのみを処理
echo "変更されたファイルを検出しています..."
CHANGED_FILES=""
while read -r file; do
    # answers.txtは除外
    if [ "$file" == "$ANSWERS_FILE" ]; then
        continue
    fi

    # ファイルが前回処理されていない、または変更されている場合
    if ! echo "$PREV_PROCESSED_FILES" | grep -q "^$file|" || [ "$file" -nt "$PROCESSED_FILES" ]; then
        CHANGED_FILES="$CHANGED_FILES $file"
        echo "$file|$(date +%s)" >> "$TEMP_DIR/new_processed_files.txt"
    else
        # 変更されていないファイルは前回の情報をそのまま使用
        grep "^$file|" "$PROCESSED_FILES" >> "$TEMP_DIR/new_processed_files.txt"
    fi
done < "$TEMP_DIR/current_files.txt"

# 処理済みファイルリストを更新
mv "$TEMP_DIR/new_processed_files.txt" "$PROCESSED_FILES"

# 変更されたファイルがない場合は終了
if [ -z "$CHANGED_FILES" ]; then
    echo "変更されたファイルはありません。処理を終了します。"
    exit 0
fi

# 変更されたファイルを処理
echo "変更されたファイルを処理しています..."

# 並列処理のための関数
process_file() {
    local file="$1"
    local temp_output="$TEMP_DIR/$(basename "$file").qa"
    
    echo "ファイル $file を処理中..."
    
    # ファイルの拡張子を取得
    ext="${file##*.}"
    
    if [ "$ext" == "txt" ]; then
        # テキストファイルの処理
        # 形式: "質問|回答" または "# 質問\n回答"
        grep -E "^[^#|]+\|" "$file" >> "$temp_output" || true
        
        # マークダウン形式の質問と回答を抽出
        awk '
        BEGIN { question = ""; answer = ""; }
        /^# / {
            if (question != "" && answer != "") {
                gsub(/\\/, "\\\\", answer);
                gsub(/\n/, "\\n", answer);
                print question "|" answer;
            }
            question = substr($0, 3);
            answer = "";
            next;
        }
        /^[^#]/ {
            if (answer == "") {
                answer = $0;
            } else {
                answer = answer "\\n" $0;
            }
        }
        END {
            if (question != "" && answer != "") {
                gsub(/\\/, "\\\\", answer);
                gsub(/\n/, "\\n", answer);
                print question "|" answer;
            }
        }' "$file" >> "$temp_output" || true
    elif [ "$ext" == "md" ]; then
        # マークダウンファイルの処理
        awk '
        BEGIN { question = ""; answer = ""; }
        /^## / {
            if (question != "" && answer != "") {
                gsub(/\\/, "\\\\", answer);
                gsub(/\n/, "\\n", answer);
                print question "|" answer;
            }
            question = substr($0, 4);
            answer = "";
            next;
        }
        /^[^#]/ {
            if (question != "" && answer == "") {
                answer = $0;
            } else if (question != "" && answer != "") {
                answer = answer "\\n" $0;
            }
        }
        END {
            if (question != "" && answer != "") {
                gsub(/\\/, "\\\\", answer);
                gsub(/\n/, "\\n", answer);
                print question "|" answer;
            }
        }' "$file" >> "$temp_output" || true
    fi
}

# 並列処理の最大数
MAX_PARALLEL=8
RUNNING=0

# 変更されたファイルを並列処理
for file in $CHANGED_FILES; do
    # 並列処理の数が最大に達した場合、いずれかの処理が終わるまで待機
    if [ $RUNNING -ge $MAX_PARALLEL ]; then
        wait -n
        RUNNING=$((RUNNING - 1))
    fi
    
    # バックグラウンドで処理を実行
    process_file "$file" &
    RUNNING=$((RUNNING + 1))
done

# すべての処理が終わるまで待機
wait

# 一時ファイルを結合
cat "$TEMP_DIR"/*.qa > "$TEMP_DIR/extracted_qa.txt" 2>/dev/null

# 既存の回答ファイルと結合
if [ -f "$TEMP_DIR/extracted_qa.txt" ] && [ -s "$TEMP_DIR/extracted_qa.txt" ]; then
    # 重複を削除して既存の回答ファイルと結合
    echo "重複を削除して回答ファイルを更新しています..."
    
    # 既存の回答ファイルと新しい回答を結合し、重複を削除
    cat "$ANSWERS_FILE" "$TEMP_DIR/extracted_qa.txt" | sort -u > "$TEMP_DIR/combined_answers.txt"
    
    # 新しい回答の数を計算
    NEW_COUNT=$(wc -l < "$TEMP_DIR/combined_answers.txt")
    OLD_COUNT=$(wc -l < "$ANSWERS_FILE")
    ADDED_COUNT=$((NEW_COUNT - OLD_COUNT))
    
    # 結合したファイルを回答ファイルに移動
    mv "$TEMP_DIR/combined_answers.txt" "$ANSWERS_FILE"
    
    echo "$ADDED_COUNT 件の新しい知識が追加されました。"
else
    echo "新しい知識は見つかりませんでした。"
fi

# 一時ファイルを削除
rm -f "$TEMP_DIR"/*.qa "$TEMP_DIR/extracted_qa.txt" "$TEMP_DIR/current_files.txt"

echo "知識の更新が完了しました。"