#!/bin/bash

# GeneLLM DNA辞書管理ツール
# 使用方法: ./dict_manager.sh [コマンド] [引数]

WORKSPACE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." &> /dev/null && pwd)"
SCRIPTS_DIR="$WORKSPACE_DIR/scripts"
DNA_DICT_FILE="$WORKSPACE_DIR/data/dna_dictionary.txt"
BACKUP_DIR="$WORKSPACE_DIR/data/backups"

# バックアップディレクトリを作成
mkdir -p "$BACKUP_DIR"

# DNA辞書を表示
show_dictionary() {
    local limit="${1:-all}"
    
    if [ ! -f "$DNA_DICT_FILE" ]; then
        echo "エラー: DNA辞書ファイルが見つかりません: $DNA_DICT_FILE"
        exit 1
    fi
    
    echo "DNA辞書の内容："
    echo "----------------------------------------"
    
    # 辞書の総エントリ数を取得
    local total_entries=$(wc -l < "$DNA_DICT_FILE")
    echo "総エントリ数: $total_entries"
    echo ""
    
    # 主語（E）、動詞（C）、結果（R）の数を取得
    local entity_count=$(grep "^E" "$DNA_DICT_FILE" | wc -l)
    local concept_count=$(grep "^C" "$DNA_DICT_FILE" | wc -l)
    local result_count=$(grep "^R" "$DNA_DICT_FILE" | wc -l)
    
    echo "主語(E): $entity_count エントリ"
    echo "動詞(C): $concept_count エントリ"
    echo "結果(R): $result_count エントリ"
    echo ""
    
    # 辞書の内容を表示
    if [ "$limit" = "all" ]; then
        # すべてのエントリを表示
        echo "すべてのエントリ："
        echo "----------------------------------------"
        cat -n "$DNA_DICT_FILE" | while read -r line_num line; do
            code=$(echo "$line" | cut -d'|' -f1)
            word=$(echo "$line" | cut -d'|' -f2)
            printf "%4d: %s\t%s\n" "$line_num" "$code" "$word"
        done
    else
        # 指定された数のエントリを表示
        echo "最初の $limit エントリ："
        echo "----------------------------------------"
        head -n "$limit" "$DNA_DICT_FILE" | cat -n | while read -r line_num line; do
            code=$(echo "$line" | cut -d'|' -f1)
            word=$(echo "$line" | cut -d'|' -f2)
            printf "%4d: %s\t%s\n" "$line_num" "$code" "$word"
        done
    fi
}

# DNA辞書にエントリを追加
add_entry() {
    local type="$1"
    local word="$2"
    
    if [ -z "$type" ] || [ -z "$word" ]; then
        echo "エラー: タイプと単語を指定してください"
        exit 1
    fi
    
    # タイプを大文字に変換
    type=$(echo "$type" | tr '[:lower:]' '[:upper:]')
    
    # タイプの検証
    if [ "$type" != "E" ] && [ "$type" != "C" ] && [ "$type" != "R" ]; then
        echo "エラー: タイプは E（主語）、C（動詞）、R（結果）のいずれかを指定してください"
        exit 1
    fi
    
    # 辞書ファイルが存在しない場合は作成
    if [ ! -f "$DNA_DICT_FILE" ]; then
        touch "$DNA_DICT_FILE"
    fi
    
    # 既に同じエントリが存在するか確認
    if grep -q "^$type.*|$word$" "$DNA_DICT_FILE"; then
        echo "エラー: 同じタイプと単語の組み合わせが既に存在します"
        exit 1
    fi
    
    # 新しいIDを取得
    local last_id=$(grep "^$type" "$DNA_DICT_FILE" | wc -l)
    local new_id=$(printf "%02d" "$last_id")
    
    # エントリを追加
    echo "$type$new_id|$word" >> "$DNA_DICT_FILE"
    
    echo "エントリを追加しました: $type$new_id|$word"
}

# DNA辞書からエントリを削除
delete_entry() {
    local code="$1"
    
    if [ -z "$code" ]; then
        echo "エラー: 削除するエントリのコードを指定してください"
        exit 1
    fi
    
    if [ ! -f "$DNA_DICT_FILE" ]; then
        echo "エラー: DNA辞書ファイルが見つかりません: $DNA_DICT_FILE"
        exit 1
    fi
    
    # エントリが存在するか確認
    if ! grep -q "^$code|" "$DNA_DICT_FILE"; then
        echo "エラー: コード '$code' のエントリが見つかりません"
        exit 1
    fi
    
    # バックアップを作成
    local timestamp=$(date +"%Y%m%d%H%M%S")
    local backup_file="$BACKUP_DIR/dna_dictionary_$timestamp.txt"
    cp "$DNA_DICT_FILE" "$backup_file"
    
    # エントリを削除
    grep -v "^$code|" "$DNA_DICT_FILE" > "$DNA_DICT_FILE.tmp"
    mv "$DNA_DICT_FILE.tmp" "$DNA_DICT_FILE"
    
    echo "エントリを削除しました: $code"
    echo "バックアップを作成しました: $backup_file"
}

# DNA辞書をエクスポート
export_dictionary() {
    local output_file="$1"
    
    if [ -z "$output_file" ]; then
        # デフォルトのファイル名を生成
        local timestamp=$(date +"%Y%m%d%H%M%S")
        output_file="$WORKSPACE_DIR/data/dna_dictionary_export_$timestamp.txt"
    fi
    
    if [ ! -f "$DNA_DICT_FILE" ]; then
        echo "エラー: DNA辞書ファイルが見つかりません: $DNA_DICT_FILE"
        exit 1
    fi
    
    # 辞書をコピー
    cp "$DNA_DICT_FILE" "$output_file"
    
    echo "DNA辞書をエクスポートしました: $output_file"
}

# DNA辞書をインポート
import_dictionary() {
    local input_file="$1"
    
    if [ -z "$input_file" ]; then
        echo "エラー: インポートするファイルを指定してください"
        exit 1
    fi
    
    if [ ! -f "$input_file" ]; then
        echo "エラー: 指定されたファイルが見つかりません: $input_file"
        exit 1
    fi
    
    # バックアップを作成
    if [ -f "$DNA_DICT_FILE" ]; then
        local timestamp=$(date +"%Y%m%d%H%M%S")
        local backup_file="$BACKUP_DIR/dna_dictionary_$timestamp.txt"
        cp "$DNA_DICT_FILE" "$backup_file"
        echo "既存の辞書のバックアップを作成しました: $backup_file"
    fi
    
    # 辞書をインポート
    cp "$input_file" "$DNA_DICT_FILE"
    
    echo "DNA辞書をインポートしました: $input_file"
}

# DNA辞書を検索
search_dictionary() {
    local keyword="$1"
    
    if [ -z "$keyword" ]; then
        echo "エラー: 検索キーワードを指定してください"
        exit 1
    fi
    
    if [ ! -f "$DNA_DICT_FILE" ]; then
        echo "エラー: DNA辞書ファイルが見つかりません: $DNA_DICT_FILE"
        exit 1
    fi
    
    echo "キーワード '$keyword' で辞書を検索中..."
    echo "----------------------------------------"
    
    # 検索結果を表示
    grep -i "$keyword" "$DNA_DICT_FILE" | cat -n | while read -r line_num line; do
        code=$(echo "$line" | cut -d'|' -f1)
        word=$(echo "$line" | cut -d'|' -f2)
        printf "%4d: %s\t%s\n" "$line_num" "$code" "$word"
    done
}

# ヘルプを表示
show_help() {
    cat << EOF
使用方法: ./dict_manager.sh [コマンド] [引数]

コマンド:
  show [limit]                  DNA辞書の内容を表示
  add <type> <word>             DNA辞書にエントリを追加
  delete <code>                 DNA辞書からエントリを削除
  export [output_file]          DNA辞書をエクスポート
  import <input_file>           DNA辞書をインポート
  search <keyword>              DNA辞書を検索
  help                          このヘルプを表示

引数:
  limit                         表示するエントリの数 (デフォルト: すべて)
  type                          エントリのタイプ (E: 主語, C: 動詞, R: 結果)
  word                          追加する単語
  code                          削除するエントリのコード (例: E00, C01)
  output_file                   エクスポート先のファイルパス
  input_file                    インポート元のファイルパス
  keyword                       検索キーワード

例:
  ./dict_manager.sh show 10            # 最初の10エントリを表示
  ./dict_manager.sh add E 猫           # 主語として「猫」を追加
  ./dict_manager.sh delete E00         # コードE00のエントリを削除
  ./dict_manager.sh export             # 辞書をエクスポート
  ./dict_manager.sh import dict.txt    # 辞書をインポート
  ./dict_manager.sh search 猫          # 「猫」を含むエントリを検索
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
    show)
        limit="${1:-all}"
        show_dictionary "$limit"
        ;;
    add)
        if [ $# -lt 2 ]; then
            echo "エラー: タイプと単語を指定してください"
            show_help
            exit 1
        fi
        add_entry "$1" "$2"
        ;;
    delete)
        if [ $# -lt 1 ]; then
            echo "エラー: 削除するエントリのコードを指定してください"
            show_help
            exit 1
        fi
        delete_entry "$1"
        ;;
    export)
        export_dictionary "$1"
        ;;
    import)
        if [ $# -lt 1 ]; then
            echo "エラー: インポートするファイルを指定してください"
            show_help
            exit 1
        fi
        import_dictionary "$1"
        ;;
    search)
        if [ $# -lt 1 ]; then
            echo "エラー: 検索キーワードを指定してください"
            show_help
            exit 1
        fi
        search_dictionary "$1"
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