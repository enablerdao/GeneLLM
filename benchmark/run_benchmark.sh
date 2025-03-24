#!/bin/bash

# ベンチマークスクリプト
# 20個の質問に対する応答時間と品質を測定します

# タイムスタンプ付きの出力ディレクトリを作成
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
RESULTS_DIR="benchmark/results/$TIMESTAMP"
mkdir -p "$RESULTS_DIR"

# 結果ファイル
RESULTS_FILE="$RESULTS_DIR/benchmark_results.md"
CSV_FILE="$RESULTS_DIR/benchmark_results.csv"

# 最新の結果へのシンボリックリンクを作成
ln -sf "$RESULTS_DIR" benchmark/results/latest

# 結果ファイルの初期化
echo "# GeneLLM ベンチマーク結果" > $RESULTS_FILE
echo "実行日時: $(date)" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "## 実行環境" >> $RESULTS_FILE
echo "- OS: $(uname -s) $(uname -r)" >> $RESULTS_FILE
echo "- CPU: $(grep "model name" /proc/cpuinfo | head -1 | cut -d':' -f2 | sed 's/^[ \t]*//' || echo "不明")" >> $RESULTS_FILE
echo "- メモリ: $(free -h | grep Mem | awk '{print $2}')" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "## 結果サマリー" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "| # | 質問 | 応答時間(秒) | 応答文字数 | 評価スコア |" >> $RESULTS_FILE
echo "|---|------|--------------|------------|------------|" >> $RESULTS_FILE

# CSVヘッダー
echo "質問番号,質問,応答時間(秒),応答文字数,評価スコア" > $CSV_FILE

# 質問ファイル
QUESTIONS_FILE="benchmark/questions.txt"

# 質問数をカウント
TOTAL_QUESTIONS=$(wc -l < $QUESTIONS_FILE)
echo "合計 $TOTAL_QUESTIONS 個の質問をベンチマークします..."

# 評価関数
evaluate_response() {
    local response="$1"
    local question="$2"
    
    # 応答の長さによる基本スコア (0-5)
    local length=${#response}
    local length_score=0
    
    if [ $length -lt 50 ]; then
        length_score=1
    elif [ $length -lt 200 ]; then
        length_score=2
    elif [ $length -lt 500 ]; then
        length_score=3
    elif [ $length -lt 1000 ]; then
        length_score=4
    else
        length_score=5
    fi
    
    # キーワードの存在チェック (0-5)
    local keyword_score=0
    
    # 質問に応じたキーワードを設定
    if [[ "$question" == *"Hello World"* ]]; then
        if [[ "$response" == *"printf"* ]] || [[ "$response" == *"puts"* ]]; then
            ((keyword_score++))
        fi
        if [[ "$response" == *"#include"* ]]; then
            ((keyword_score++))
        fi
        if [[ "$response" == *"<stdio.h>"* ]]; then
            ((keyword_score++))
        fi
        if [[ "$response" == *"main"* ]]; then
            ((keyword_score++))
        fi
        if [[ "$response" == *"return 0"* ]]; then
            ((keyword_score++))
        fi
    elif [[ "$question" == *"配列"* ]] && [[ "$question" == *"逆順"* ]]; then
        if [[ "$response" == *"for"* ]]; then
            ((keyword_score++))
        fi
        if [[ "$response" == *"["* ]] && [[ "$response" == *"]"* ]]; then
            ((keyword_score++))
        fi
        if [[ "$response" == *"temp"* ]] || [[ "$response" == *"swap"* ]]; then
            ((keyword_score++))
        fi
        if [[ "$response" == *"int"* ]]; then
            ((keyword_score++))
        fi
        if [[ "$response" == *"length"* ]] || [[ "$response" == *"size"* ]]; then
            ((keyword_score++))
        fi
    else
        # その他の質問に対する一般的なキーワードチェック
        if [[ "$response" == *"#include"* ]]; then
            ((keyword_score++))
        fi
        if [[ "$response" == *"main"* ]]; then
            ((keyword_score++))
        fi
        if [[ "$response" == *"return"* ]]; then
            ((keyword_score++))
        fi
        if [[ "$response" == *"{"* ]] && [[ "$response" == *"}"* ]]; then
            ((keyword_score++))
        fi
        if [[ "$response" == *"int"* ]] || [[ "$response" == *"char"* ]] || [[ "$response" == *"void"* ]]; then
            ((keyword_score++))
        fi
    fi
    
    # 総合スコア (0-10)
    local total_score=$((length_score + keyword_score))
    echo $total_score
}

# 各質問に対してベンチマークを実行
question_num=1
total_time=0
total_score=0

while IFS= read -r question; do
    echo "[$question_num/$TOTAL_QUESTIONS] 質問: $question"
    
    # 応答時間を測定
    start_time=$(date +%s.%N)
    response=$(./gllm "$question")
    end_time=$(date +%s.%N)
    
    # 応答時間を計算 (秒)
    elapsed_time=$(echo "$end_time - $start_time" | bc)
    elapsed_time_rounded=$(printf "%.2f" $elapsed_time)
    
    # 応答の文字数
    response_length=${#response}
    
    # 応答を評価
    score=$(evaluate_response "$response" "$question")
    
    # 結果をファイルに追加
    echo "| $question_num | ${question:0:30}... | $elapsed_time_rounded | $response_length | $score/10 |" >> $RESULTS_FILE
    echo "$question_num,\"$question\",$elapsed_time_rounded,$response_length,$score" >> $CSV_FILE
    
    # 詳細な結果を別ファイルに保存
    detail_file="$RESULTS_DIR/question_${question_num}.md"
    echo "# 質問 $question_num" > $detail_file
    echo "" >> $detail_file
    echo "## 質問" >> $detail_file
    echo "$question" >> $detail_file
    echo "" >> $detail_file
    echo "## 応答" >> $detail_file
    echo "\`\`\`" >> $detail_file
    echo "$response" >> $detail_file
    echo "\`\`\`" >> $detail_file
    echo "" >> $detail_file
    echo "## 評価" >> $detail_file
    echo "- 応答時間: $elapsed_time_rounded 秒" >> $detail_file
    echo "- 応答文字数: $response_length 文字" >> $detail_file
    echo "- 評価スコア: $score/10" >> $detail_file
    
    # 合計を更新
    total_time=$(echo "$total_time + $elapsed_time" | bc)
    total_score=$((total_score + score))
    
    # 次の質問へ
    ((question_num++))
done < $QUESTIONS_FILE

# 平均値を計算
avg_time=$(echo "scale=2; $total_time / $TOTAL_QUESTIONS" | bc)
avg_score=$(echo "scale=1; $total_score / $TOTAL_QUESTIONS" | bc)

# サマリーを追加
echo "" >> $RESULTS_FILE
echo "## 総合評価" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "- 合計質問数: $TOTAL_QUESTIONS" >> $RESULTS_FILE
echo "- 平均応答時間: $avg_time 秒" >> $RESULTS_FILE
echo "- 平均評価スコア: $avg_score/10" >> $RESULTS_FILE

echo ""
echo "ベンチマークが完了しました！"
echo "結果は $RESULTS_FILE に保存されました"
echo "CSVデータは $CSV_FILE に保存されました"
echo ""
echo "総合評価:"
echo "- 合計質問数: $TOTAL_QUESTIONS"
echo "- 平均応答時間: $avg_time 秒"
echo "- 平均評価スコア: $avg_score/10"