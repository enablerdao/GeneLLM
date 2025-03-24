#!/bin/bash

# GeneLLM ツール統合コマンド
# 使用方法: ./genellm_tools.sh [コマンド] [引数]

WORKSPACE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." &> /dev/null && pwd)"
SCRIPTS_DIR="$WORKSPACE_DIR/scripts"

# ヘルプを表示
show_help() {
    cat << EOF
使用方法: ./genellm_tools.sh [コマンド] [引数]

コマンド:
  vectors [limit] [offset]    単語ベクトルデータを表示
  words [limit] [offset]      単語リストを表示
  tokenize "テキスト"         テキストをトークナイズして表示
  dna-compress "テキスト"     テキストをDNAコードに圧縮
  dna-decompress "DNAコード"  DNAコードからテキストを復元
  verb-info "動詞"            動詞の活用形と種類を表示
  help                       このヘルプを表示

引数:
  limit                      表示する単語の数 (デフォルト: 10)
  offset                     表示を開始する位置 (デフォルト: 0)
  "テキスト"                 トークナイズまたは圧縮するテキスト
  "DNAコード"                復元するDNAコード（例: E00C01R02）
  "動詞"                     活用情報を表示する動詞

例:
  ./genellm_tools.sh vectors              # ベクトルデータを表示（デフォルト10件）
  ./genellm_tools.sh vectors 20           # ベクトルデータを20件表示
  ./genellm_tools.sh vectors 5 100        # ベクトルデータを位置100から5件表示
  ./genellm_tools.sh words                # 単語リストを表示（デフォルト10件）
  ./genellm_tools.sh words 20             # 単語リストを20件表示
  ./genellm_tools.sh tokenize "こんにちは" # テキストをトークナイズして表示
  ./genellm_tools.sh dna-compress "猫が魚を食べる"  # テキストをDNAコードに圧縮
  ./genellm_tools.sh dna-decompress "E00C00R00"    # DNAコードからテキストを復元
  ./genellm_tools.sh verb-info "食べる"   # 動詞の活用形と種類を表示
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
    vectors)
        "$SCRIPTS_DIR/vectors.sh" "$@"
        ;;
    words)
        "$SCRIPTS_DIR/tokens.sh" words "$@"
        ;;
    tokenize)
        "$SCRIPTS_DIR/tokens.sh" tokenize "$@"
        ;;
    dna-compress)
        "$SCRIPTS_DIR/dna.sh" compress "$@"
        ;;
    dna-decompress)
        "$SCRIPTS_DIR/dna.sh" decompress "$@"
        ;;
    verb-info)
        "$SCRIPTS_DIR/dna.sh" verb-info "$@"
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