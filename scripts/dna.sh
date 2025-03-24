#!/bin/bash

# GeneLLM DNA圧縮ツール
# 使用方法: ./dna.sh [コマンド] [引数]
#
# DNAベースの概念圧縮（シンボル化）
# - 主語(E): Entity - 主体となる名詞
# - 動詞(C): Concept - 行為や状態を表す動詞
# - 結果(R): Result - 行為の対象や結果
#
# 動詞の活用形:
# - VERB_FORM_BASIC: 基本形（辞書形）- 食べる
# - VERB_FORM_MASU: ます形 - 食べます
# - VERB_FORM_TE: て形 - 食べて
# - VERB_FORM_TA: た形 - 食べた
# - VERB_FORM_NAI: ない形 - 食べない
# - VERB_FORM_PASSIVE: 受身形 - 食べられる
# - VERB_FORM_CAUSATIVE: 使役形 - 食べさせる
# - VERB_FORM_POTENTIAL: 可能形 - 食べられる
# - VERB_FORM_IMPERATIVE: 命令形 - 食べろ/食べよ
# - VERB_FORM_VOLITIONAL: 意志形 - 食べよう
#
# 動詞の種類:
# - VERB_TYPE_GODAN: 五段動詞 - 書く、読む
# - VERB_TYPE_ICHIDAN: 一段動詞 - 食べる、見る
# - VERB_TYPE_IRREGULAR: 不規則動詞 - する、来る

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
  verb-info "動詞"          動詞の活用形と種類を表示
  help                     このヘルプを表示

引数:
  "テキスト"               圧縮するテキスト
  "DNAコード"              復元するDNAコード（例: E00C01R02）
  "動詞"                   活用情報を表示する動詞

例:
  ./dna.sh compress "猫が魚を食べる"
  ./dna.sh decompress "E00C01R02"
  ./dna.sh verb-info "食べる"

DNAコードの形式:
  E00 - 主語(Entity)のID
  C00 - 動詞(Concept)のID
  R00 - 結果(Result)のID
EOF
}

# 動詞の活用形と種類を表示
show_verb_info() {
    local verb="$1"
    
    if [ -z "$verb" ]; then
        echo "エラー: 動詞が指定されていません"
        exit 1
    fi
    
    echo "動詞「$verb」の情報："
    echo "----------------------------------------"
    
    # 動詞の種類を判定（簡易版）
    local verb_type="不明"
    if [[ "$verb" == *"する" ]]; then
        verb_type="不規則動詞 (VERB_TYPE_IRREGULAR)"
    elif [[ "$verb" == *"来る" ]] || [[ "$verb" == *"くる" ]]; then
        verb_type="不規則動詞 (VERB_TYPE_IRREGULAR)"
    elif [[ "$verb" == *"る" ]]; then
        # 一段動詞か五段動詞かを判定（簡易版）
        local stem="${verb%る}"
        local last_char="${stem: -1}"
        if [[ "$last_char" =~ [いえ] ]]; then
            # 一段動詞の例外チェック
            if [[ "$verb" == "入る" ]] || [[ "$verb" == "切る" ]] || [[ "$verb" == "走る" ]] || 
               [[ "$verb" == "知る" ]] || [[ "$verb" == "散る" ]] || [[ "$verb" == "滑る" ]] || 
               [[ "$verb" == "蹴る" ]] || [[ "$verb" == "練る" ]]; then
                verb_type="五段動詞 (VERB_TYPE_GODAN)"
            else
                verb_type="一段動詞 (VERB_TYPE_ICHIDAN)"
            fi
        else
            verb_type="五段動詞 (VERB_TYPE_GODAN)"
        fi
    else
        verb_type="五段動詞 (VERB_TYPE_GODAN)"
    fi
    
    echo "動詞の種類: $verb_type"
    echo ""
    echo "活用形:"
    
    # 基本形
    echo "- 基本形 (VERB_FORM_BASIC): $verb"
    
    # ます形
    local masu_form=""
    if [[ "$verb_type" == *"一段"* ]]; then
        masu_form="${verb%る}ます"
    elif [[ "$verb_type" == *"五段"* ]]; then
        if [[ "$verb" == *"う" ]]; then
            masu_form="${verb%う}います"
        elif [[ "$verb" == *"く" ]]; then
            masu_form="${verb%く}きます"
        elif [[ "$verb" == *"ぐ" ]]; then
            masu_form="${verb%ぐ}ぎます"
        elif [[ "$verb" == *"す" ]]; then
            masu_form="${verb%す}します"
        elif [[ "$verb" == *"つ" ]]; then
            masu_form="${verb%つ}ちます"
        elif [[ "$verb" == *"ぬ" ]]; then
            masu_form="${verb%ぬ}にます"
        elif [[ "$verb" == *"ぶ" ]]; then
            masu_form="${verb%ぶ}びます"
        elif [[ "$verb" == *"む" ]]; then
            masu_form="${verb%む}みます"
        elif [[ "$verb" == *"る" ]]; then
            masu_form="${verb%る}ります"
        fi
    elif [[ "$verb" == "する" ]]; then
        masu_form="します"
    elif [[ "$verb" == "来る" ]] || [[ "$verb" == "くる" ]]; then
        masu_form="きます"
    fi
    echo "- ます形 (VERB_FORM_MASU): $masu_form"
    
    # て形
    local te_form=""
    if [[ "$verb_type" == *"一段"* ]]; then
        te_form="${verb%る}て"
    elif [[ "$verb_type" == *"五段"* ]]; then
        if [[ "$verb" == *"う" ]] || [[ "$verb" == *"つ" ]] || [[ "$verb" == *"る" ]]; then
            te_form="${verb%?}って"
        elif [[ "$verb" == *"く" ]]; then
            te_form="${verb%く}いて"
        elif [[ "$verb" == *"ぐ" ]]; then
            te_form="${verb%ぐ}いで"
        elif [[ "$verb" == *"す" ]]; then
            te_form="${verb%す}して"
        elif [[ "$verb" == *"ぬ" ]] || [[ "$verb" == *"ぶ" ]] || [[ "$verb" == *"む" ]]; then
            te_form="${verb%?}んで"
        fi
    elif [[ "$verb" == "する" ]]; then
        te_form="して"
    elif [[ "$verb" == "来る" ]] || [[ "$verb" == "くる" ]]; then
        te_form="きて"
    fi
    echo "- て形 (VERB_FORM_TE): $te_form"
    
    # た形
    local ta_form=""
    if [[ "$verb_type" == *"一段"* ]]; then
        ta_form="${verb%る}た"
    elif [[ "$verb_type" == *"五段"* ]]; then
        if [[ "$verb" == *"う" ]] || [[ "$verb" == *"つ" ]] || [[ "$verb" == *"る" ]]; then
            ta_form="${verb%?}った"
        elif [[ "$verb" == *"く" ]]; then
            ta_form="${verb%く}いた"
        elif [[ "$verb" == *"ぐ" ]]; then
            ta_form="${verb%ぐ}いだ"
        elif [[ "$verb" == *"す" ]]; then
            ta_form="${verb%す}した"
        elif [[ "$verb" == *"ぬ" ]] || [[ "$verb" == *"ぶ" ]] || [[ "$verb" == *"む" ]]; then
            ta_form="${verb%?}んだ"
        fi
    elif [[ "$verb" == "する" ]]; then
        ta_form="した"
    elif [[ "$verb" == "来る" ]] || [[ "$verb" == "くる" ]]; then
        ta_form="きた"
    fi
    echo "- た形 (VERB_FORM_TA): $ta_form"
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
    verb-info)
        if [ $# -ge 1 ]; then
            show_verb_info "$1"
        else
            echo "エラー: 動詞が指定されていません"
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