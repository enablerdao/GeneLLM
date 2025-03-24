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
        DNA_CODE=$(cat "$DNA_RESULTS_FILE" | grep "質問から生成したDNAコード:" | sed 's/質問から生成したDNAコード: //')
        
        if [ -n "$DNA_RESULTS" ]; then
            # DNAコードを解析して意味を抽出
            ENTITY=""
            CONCEPT=""
            RESULT=""
            ATTRIBUTE=""
            LOCATION=""
            MANNER=""
            
            if [[ "$DNA_CODE" == *"E0"* ]]; then
                ENTITY="GeneLLM"
            elif [[ "$DNA_CODE" == *"E5"* ]]; then
                ENTITY="知識グラフ"
            elif [[ "$DNA_CODE" == *"E6"* ]]; then
                ENTITY="自然言語処理"
            elif [[ "$DNA_CODE" == *"E7"* ]]; then
                ENTITY="機械学習"
            elif [[ "$DNA_CODE" == *"E8"* ]]; then
                ENTITY="深層学習"
            fi
            
            if [[ "$DNA_CODE" == *"C3"* ]]; then
                CONCEPT="解析する"
            elif [[ "$DNA_CODE" == *"C4"* ]]; then
                CONCEPT="生成する"
            elif [[ "$DNA_CODE" == *"C5"* ]]; then
                CONCEPT="最適化する"
            elif [[ "$DNA_CODE" == *"C10"* ]]; then
                CONCEPT="学習する"
            elif [[ "$DNA_CODE" == *"C11"* ]]; then
                CONCEPT="処理する"
            fi
            
            if [[ "$DNA_CODE" == *"R1"* ]]; then
                RESULT="知識ベース"
            elif [[ "$DNA_CODE" == *"R3"* ]]; then
                RESULT="テキストデータ"
            elif [[ "$DNA_CODE" == *"R5"* ]]; then
                RESULT="構造化データ"
            elif [[ "$DNA_CODE" == *"R6"* ]]; then
                RESULT="非構造化データ"
            fi
            
            if [[ "$DNA_CODE" == *"A0"* ]]; then
                ATTRIBUTE="高速な"
            elif [[ "$DNA_CODE" == *"A1"* ]]; then
                ATTRIBUTE="効率的な"
            fi
            
            if [[ "$DNA_CODE" == *"L2"* ]]; then
                LOCATION="クラウド環境で"
            elif [[ "$DNA_CODE" == *"L5"* ]]; then
                LOCATION="データベース内で"
            fi
            
            if [[ "$DNA_CODE" == *"M0"* ]]; then
                MANNER="効率的に"
            elif [[ "$DNA_CODE" == *"M3"* ]]; then
                MANNER="迅速に"
            fi
            
            # 回答を生成
            ANSWER="${ENTITY}は${RESULT}を${CONCEPT}"
            if [ -n "$ATTRIBUTE" ]; then
                ANSWER="${ANSWER}ために${ATTRIBUTE}アルゴリズムを使用します"
            fi
            if [ -n "$LOCATION" ]; then
                ANSWER="${ANSWER}。この処理は${LOCATION}実行されます"
            fi
            if [ -n "$MANNER" ]; then
                ANSWER="${ANSWER}。システムは${MANNER}動作するよう設計されています"
            fi
            
            # デバッグ情報を非表示にする
            if [ "$DEBUG" = true ]; then
                echo "生成されたDNAコード: $DNA_CODE"
                echo "抽出された要素: 主語=$ENTITY, 動詞=$CONCEPT, 目的語=$RESULT, 属性=$ATTRIBUTE, 場所=$LOCATION, 様態=$MANNER"
            fi
            
            echo "${ENTITY}は${RESULT}を${CONCEPT}します。

${ENTITY}は以下のような方法で${RESULT}を${CONCEPT}します："

1. データの前処理: 入力データを正規化し、ノイズを除去します
2. 効率的なインデックス作成: 高速な検索のためのインデックスを構築します
3. 冗長性の排除: 重複情報を削除して効率化します
4. 分散処理: 大規模データを複数のノードで並列処理します
5. キャッシュ最適化: 頻繁にアクセスされるデータをキャッシュします

これらの技術により、${ENTITY}は${RESULT}を効率的に${CONCEPT}し、ユーザーの質問に迅速に回答できます。"

            # デバッグ情報を非表示にする
            if [ "$DEBUG" = true ]; then
                echo "DNA検索結果:"
                echo "$DNA_RESULTS"
            fi
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

```
#include <stdio.h>

int main() {
    printf(\"Hello, World!\\n\");
    return 0;
}
```

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