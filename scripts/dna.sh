#!/bin/bash

# GeneLLM DNA圧縮ツール
# 使用方法: ./dna.sh [コマンド] [引数]

WORKSPACE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." &> /dev/null && pwd)"
SCRIPTS_DIR="$WORKSPACE_DIR/scripts"
DNA_DICT_FILE="$WORKSPACE_DIR/data/dna_dictionary.txt"

# DNAコードを生成
generate_dna() {
    local text="$1"
    
    if [ -z "$text" ]; then
        echo "エラー: テキストが指定されていません"
        exit 1
    fi
    
    echo "テキスト「$text」のDNA圧縮結果："
    echo "----------------------------------------"
    
    # MeCabを使用してトークナイズ
    if ! command -v mecab &> /dev/null; then
        echo "エラー: MeCabがインストールされていません"
        exit 1
    fi
    
    # 主語、動詞、目的語を抽出
    local subject=""
    local verb=""
    local object=""
    
    # 形態素解析結果を解析
    local analysis=$(echo "$text" | mecab -Ochasen)
    
    # 主語（名詞）を探す
    subject=$(echo "$analysis" | grep "名詞" | head -n 1 | cut -f1)
    
    # 動詞を探す
    verb=$(echo "$analysis" | grep "動詞" | head -n 1 | cut -f3)
    
    # 目的語（名詞）を探す - 主語の後に出てくる名詞
    object=$(echo "$analysis" | grep "名詞" | sed -n '2p' | cut -f1)
    
    if [ -z "$subject" ]; then
        subject="(なし)"
    fi
    
    if [ -z "$verb" ]; then
        verb="(なし)"
    fi
    
    if [ -z "$object" ]; then
        object="(なし)"
    fi
    
    # DNAコードを生成
    local dna_code=""
    
    # 主語のコード（E + 番号）
    if [ "$subject" != "(なし)" ]; then
        # 単語IDを検索（単語リストから）
        local subject_id=$(grep -n "^$subject$" "$WORKSPACE_DIR/knowledge/text/japanese_words.txt" | cut -d':' -f1)
        if [ -z "$subject_id" ]; then
            subject_id="00"
        else
            # 2桁の数字にフォーマット
            subject_id=$(printf "%02d" $((subject_id - 1)))
        fi
        dna_code="${dna_code}E${subject_id}"
    fi
    
    # 動詞のコード（C + 番号）
    if [ "$verb" != "(なし)" ]; then
        # 単語IDを検索
        local verb_id=$(grep -n "^$verb$" "$WORKSPACE_DIR/knowledge/text/japanese_words.txt" | cut -d':' -f1)
        if [ -z "$verb_id" ]; then
            verb_id="00"
        else
            # 2桁の数字にフォーマット
            verb_id=$(printf "%02d" $((verb_id - 1)))
        fi
        dna_code="${dna_code}C${verb_id}"
    fi
    
    # 目的語のコード（R + 番号）
    if [ "$object" != "(なし)" ]; then
        # 単語IDを検索
        local object_id=$(grep -n "^$object$" "$WORKSPACE_DIR/knowledge/text/japanese_words.txt" | cut -d':' -f1)
        if [ -z "$object_id" ]; then
            object_id="00"
        else
            # 2桁の数字にフォーマット
            object_id=$(printf "%02d" $((object_id - 1)))
        fi
        dna_code="${dna_code}R${object_id}"
    fi
    
    # 結果を表示
    echo "主語: $subject"
    echo "動詞: $verb"
    echo "目的語: $object"
    echo ""
    echo "DNAコード: $dna_code"
    
    # DNAコードを辞書に保存
    if [ ! -f "$DNA_DICT_FILE" ]; then
        echo "E00|$subject" > "$DNA_DICT_FILE"
        echo "C00|$verb" >> "$DNA_DICT_FILE"
        echo "R00|$object" >> "$DNA_DICT_FILE"
        echo "DNAコードを辞書に保存しました: $DNA_DICT_FILE"
    else
        # 既存の辞書に追加
        # 主語が辞書にあるか確認
        if ! grep -q "^E.*|$subject$" "$DNA_DICT_FILE"; then
            # 新しいIDを取得
            local last_e_id=$(grep "^E" "$DNA_DICT_FILE" | wc -l)
            echo "E$(printf "%02d" $last_e_id)|$subject" >> "$DNA_DICT_FILE"
        fi
        
        # 動詞が辞書にあるか確認
        if ! grep -q "^C.*|$verb$" "$DNA_DICT_FILE"; then
            # 新しいIDを取得
            local last_c_id=$(grep "^C" "$DNA_DICT_FILE" | wc -l)
            echo "C$(printf "%02d" $last_c_id)|$verb" >> "$DNA_DICT_FILE"
        fi
        
        # 目的語が辞書にあるか確認
        if ! grep -q "^R.*|$object$" "$DNA_DICT_FILE"; then
            # 新しいIDを取得
            local last_r_id=$(grep "^R" "$DNA_DICT_FILE" | wc -l)
            echo "R$(printf "%02d" $last_r_id)|$object" >> "$DNA_DICT_FILE"
        fi
    fi
}

# DNAコードから文を再構築
decompress_dna() {
    local dna_code="$1"
    
    if [ -z "$dna_code" ]; then
        echo "エラー: DNAコードが指定されていません"
        exit 1
    fi
    
    echo "DNAコード「$dna_code」の復元結果："
    echo "----------------------------------------"
    
    # 辞書ファイルが存在するか確認
    if [ ! -f "$DNA_DICT_FILE" ]; then
        echo "エラー: DNA辞書ファイルが見つかりません: $DNA_DICT_FILE"
        exit 1
    fi
    
    # DNAコードを解析
    local text=""
    local i=0
    while [ $i -lt ${#dna_code} ]; do
        local code="${dna_code:$i:3}"
        i=$((i + 3))
        
        # 辞書から単語を検索
        local word=$(grep "^$code|" "$DNA_DICT_FILE" | cut -d'|' -f2)
        
        if [ -n "$word" ]; then
            if [[ "$code" == E* ]]; then
                text="${text}${word}は"
            elif [[ "$code" == C* ]]; then
                text="${text}${word}"
            elif [[ "$code" == R* ]]; then
                text="${text}${word}を"
            fi
        fi
    done
    
    echo "復元されたテキスト: $text"
}

# ヘルプを表示
show_help() {
    cat << EOF
使用方法: ./dna.sh [コマンド] [引数]

コマンド:
  compress "テキスト"       テキストをDNAコードに圧縮
  decompress "DNAコード"    DNAコードからテキストを復元
  help                     このヘルプを表示

引数:
  "テキスト"               圧縮するテキスト
  "DNAコード"              復元するDNAコード（例: E00C01R02）

例:
  ./dna.sh compress "猫が魚を食べる"
  ./dna.sh decompress "E00C01R02"
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
    compress)
        if [ $# -ge 1 ]; then
            generate_dna "$1"
        else
            echo "エラー: テキストが指定されていません"
            show_help
            exit 1
        fi
        ;;
    decompress)
        if [ $# -ge 1 ]; then
            decompress_dna "$1"
        else
            echo "エラー: DNAコードが指定されていません"
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