#!/bin/bash

# GeneLLM ベンチマーク比較スクリプト
# 複数のベンチマーク結果を比較します

# 引数の解析
DIRS=()
LABELS=()

# ヘルプメッセージ
show_help() {
    echo "使用方法: $0 [オプション]"
    echo ""
    echo "オプション:"
    echo "  -d, --dir DIR     比較するベンチマーク結果ディレクトリ (複数指定可能)"
    echo "  -l, --label LABEL 各ディレクトリのラベル (複数指定可能)"
    echo "  -o, --output DIR  出力ディレクトリ"
    echo "  -h, --help        このヘルプメッセージを表示"
    echo ""
    echo "例:"
    echo "  $0 --dir benchmark/results/20230101 benchmark/results/20230102 --label 'バージョン1.0' 'バージョン1.1'"
    echo ""
    echo "注: ディレクトリを指定しない場合は、最新の2つのベンチマーク結果を使用します。"
}

# 引数がない場合はヘルプを表示
if [ $# -eq 0 ]; then
    show_help
    exit 0
fi

# 引数の解析
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--dir)
            DIRS+=("$2")
            shift 2
            ;;
        -l|--label)
            LABELS+=("$2")
            shift 2
            ;;
        -o|--output)
            OUTPUT="$2"
            shift 2
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            echo "エラー: 不明なオプション: $1"
            show_help
            exit 1
            ;;
    esac
done

# Pythonの依存関係をチェック
echo "🐍 Pythonの依存関係をチェック中..."
pip install pandas matplotlib numpy -q

# ディレクトリとラベルのオプションを構築
DIR_OPTS=""
LABEL_OPTS=""

if [ ${#DIRS[@]} -gt 0 ]; then
    DIR_OPTS="--dirs ${DIRS[@]}"
fi

if [ ${#LABELS[@]} -gt 0 ]; then
    LABEL_OPTS="--labels ${LABELS[@]}"
fi

OUTPUT_OPTS=""
if [ -n "$OUTPUT" ]; then
    OUTPUT_OPTS="--output $OUTPUT"
fi

# 比較スクリプトを実行
echo "📊 ベンチマーク結果を比較中..."
python benchmark/compare_benchmarks.py $DIR_OPTS $LABEL_OPTS $OUTPUT_OPTS

# 終了メッセージ
echo "✅ ベンチマーク比較が完了しました！"
if [ -n "$OUTPUT" ]; then
    echo "結果は $OUTPUT ディレクトリに保存されました"
else
    echo "結果は benchmark/results/comparison ディレクトリに保存されました"
fi