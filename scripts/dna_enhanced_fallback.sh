#!/bin/bash

# GeneLLMとDNA検索を組み合わせた拡張フォールバックスクリプト
# 使用方法: ./dna_enhanced_fallback.sh [-d] "質問"

# デバッグモードのフラグ
DEBUG=false

# 引数の解析
if [ "$1" = "-d" ]; then
    DEBUG=true
    shift
fi

# 質問テキスト
QUESTION="$1"

# スクリプトのディレクトリを取得
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
WORKSPACE_DIR="$( cd "$SCRIPT_DIR/.." &> /dev/null && pwd )"

# GeneLLMの実行パス
GLLM_PATH="$WORKSPACE_DIR/bin/gllm"
# フォールバックジェネレータの実行パス
FALLBACK_PATH="$WORKSPACE_DIR/bin/simple_fallback_generator"
# DNA検索スクリプトのパス
DNA_SEARCH_PATH="$SCRIPT_DIR/optimized_dna_search.sh"

# 質問のタイプを判定する関数
is_joke_question() {
    local question="$1"
    if [[ "$question" == *"ジョーク"* ]] || [[ "$question" == *"冗談"* ]] || [[ "$question" == *"笑える"* ]]; then
        return 0  # true
    fi
    return 1  # false
}

is_riddle_question() {
    local question="$1"
    if [[ "$question" == *"なぞなぞ"* ]] || [[ "$question" == *"謎々"* ]] || [[ "$question" == *"クイズ"* ]]; then
        return 0  # true
    fi
    return 1  # false
}

is_funny_story_question() {
    local question="$1"
    if [[ "$question" == *"面白い話"* ]] || [[ "$question" == *"おもしろい話"* ]] || [[ "$question" == *"笑える話"* ]]; then
        return 0  # true
    fi
    return 1  # false
}

is_programming_question() {
    local question="$1"
    if [[ "$question" == *"プログラム"* ]] || [[ "$question" == *"コード"* ]] ||
       [[ "$question" == *"C言語"* ]] || [[ "$question" == *"Java"* ]] ||
       [[ "$question" == *"Python"* ]] || [[ "$question" == *"JavaScript"* ]]; then
        return 0  # true
    fi
    return 1  # false
}

is_knowledge_question() {
    local question="$1"
    if [[ "$question" == *"GeneLLM"* ]] || [[ "$question" == *"知識ベース"* ]] ||
       [[ "$question" == *"データ"* ]] || [[ "$question" == *"処理"* ]] ||
       [[ "$question" == *"最適化"* ]] || [[ "$question" == *"学習"* ]] ||
       [[ "$question" == *"自然言語処理"* ]] || [[ "$question" == *"機械学習"* ]]; then
        return 0  # true
    fi
    return 1  # false
}

# GeneLLMの実行
if [ "$DEBUG" = true ]; then
    RESPONSE=$("$GLLM_PATH" -d "$QUESTION")
else
    RESPONSE=$("$GLLM_PATH" "$QUESTION")
fi

# 回答が見つからない場合や不完全な回答の場合のフォールバック処理
if [[ "$RESPONSE" == *"回答はまだ実装されていません"* ]] ||
   [[ "$RESPONSE" == *"類似度が低いため"* ]] ||
   [[ "$RESPONSE" == *"ご紹介します"* ]] ||
   [[ "$RESPONSE" == *"紹介します"* ]] ||
   [[ "$RESPONSE" == *"教えます"* ]] ||
   [[ "$RESPONSE" == *"思わず"* ]] ||
   [[ "$RESPONSE" == *"構成されています"* ]] ||
   [[ "$RESPONSE" == *"ディレクトリ構造"* ]]; then
    if [ "$DEBUG" = true ]; then
        echo "GeneLLMで回答が見つかりませんでした。拡張フォールバックを使用します。"
    fi

    # 元の回答を保存
    ORIGINAL_RESPONSE="$RESPONSE"

    # 質問のタイプに応じてフォールバックを実行
    if is_knowledge_question "$QUESTION"; then
        # 知識関連の質問の場合はDNA検索を使用
        if [ "$DEBUG" = true ]; then
            echo "DNA検索を実行しています..."
        fi
        
        # DNA検索の結果を一時ファイルに保存
        DNA_RESULTS_FILE="/tmp/dna_search_results.txt"
        "$DNA_SEARCH_PATH" "$QUESTION" 5 > "$DNA_RESULTS_FILE"
        
        # DNA検索の結果から回答を生成
        DNA_RESULTS=$(cat "$DNA_RESULTS_FILE" | grep -A 20 "検索結果:" | grep -v "検索結果:" | grep -v "^--" | grep -v "^検索完了")
        
        if [ -n "$DNA_RESULTS" ]; then
            echo "GeneLLMの知識ベースから関連情報を検索しました：

$DNA_RESULTS

これらの情報から、$QUESTION について考えると：

$(cat "$DNA_RESULTS_FILE" | grep "質問から生成したDNAコード:" | sed 's/質問から生成したDNAコード: //')は、GeneLLMの拡張DNAコードシステムで表現される概念です。このコードは主語、動詞、目的語などの要素を組み合わせて、複雑な意味を表現します。

GeneLLMは知識ベースを活用して、ユーザーの質問に対して最適な回答を提供します。上記の検索結果は、質問に関連する可能性のある情報を示しています。"
        else
            # DNA検索で結果が得られなかった場合は通常のフォールバックを使用
            FALLBACK_RESPONSE=$("$FALLBACK_PATH" "$QUESTION")
            echo "正確な回答が見つかりませんでしたが、以下の情報が参考になるかもしれません：

$FALLBACK_RESPONSE"
        fi
    elif is_joke_question "$QUESTION"; then
        FALLBACK_RESPONSE=$("$FALLBACK_PATH" "$QUESTION")
        echo "$FALLBACK_RESPONSE"
    elif is_riddle_question "$QUESTION"; then
        FALLBACK_RESPONSE=$("$FALLBACK_PATH" "$QUESTION")
        echo "$FALLBACK_RESPONSE"
    elif is_funny_story_question "$QUESTION"; then
        FALLBACK_RESPONSE=$("$FALLBACK_PATH" "$QUESTION")
        echo "$FALLBACK_RESPONSE"
    elif is_programming_question "$QUESTION"; then
        # プログラミング関連の質問の場合は、C言語のHello Worldプログラムを例として表示
        if [[ "$QUESTION" == *"Hello World"* ]] || [[ "$QUESTION" == *"ハローワールド"* ]]; then
            echo "C言語でHello Worldを表示するプログラムの例です：

\`\`\`c
#include <stdio.h>

int main() {
    printf(\"Hello, World!\\n\");
    return 0;
}
\`\`\`

このプログラムは、標準出力に「Hello, World!」と表示します。
- #include <stdio.h>: 標準入出力ライブラリを含める
- main関数: プログラムのエントリーポイント
- printf関数: 文字列を出力する関数
- return 0: プログラムが正常終了したことを示す"
        else
            # その他のプログラミング質問の場合はフォールバックジェネレータを使用
            FALLBACK_RESPONSE=$("$FALLBACK_PATH" "$QUESTION")
            echo "正確な回答が見つかりませんでしたが、以下の情報が参考になるかもしれません：

$FALLBACK_RESPONSE"
        fi
    else
        # その他の質問の場合は、フォールバックジェネレータを使用
        FALLBACK_RESPONSE=$("$FALLBACK_PATH" "$QUESTION")
        echo "正確な回答が見つかりませんでしたが、以下の情報が参考になるかもしれません：

$FALLBACK_RESPONSE"
    fi
else
    # GeneLLMで回答が見つかった場合はそのまま表示
    echo "$RESPONSE"
fi