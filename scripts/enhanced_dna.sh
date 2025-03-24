#!/bin/bash

# 拡張DNAコード圧縮・展開スクリプト
# 使用方法: ./enhanced_dna.sh [コマンド] [引数]

WORKSPACE_DIR="/workspace"
ENHANCED_DNA_DICT_FILE="$WORKSPACE_DIR/data/enhanced_dna_dictionary.txt"

# 拡張DNAコードを生成
generate_enhanced_dna() {
    local text="$1"

    if [ -z "$text" ]; then
        echo "エラー: テキストが指定されていません"
        exit 1
    fi

    echo "テキスト「$text」の拡張DNA圧縮結果："
    echo "----------------------------------------"

    # MeCabを使用してトークナイズ
    if ! command -v mecab &> /dev/null; then
        echo "エラー: MeCabがインストールされていません"
        exit 1
    fi

    # 主語、動詞、目的語、属性、時間、場所、様態、数量を抽出
    local subject=""
    local verb=""
    local object=""
    local attribute=""
    local time=""
    local location=""
    local manner=""
    local quantity=""

    # 形態素解析結果を解析
    local analysis=$(echo "$text" | mecab -Ochasen)

    # 主語（名詞）を探す
    subject=$(echo "$analysis" | grep "名詞" | head -n 1 | cut -f1)

    # 動詞を探す
    verb=$(echo "$analysis" | grep "動詞" | head -n 1 | cut -f3)

    # 目的語（名詞）を探す - 主語の後に出てくる名詞
    object=$(echo "$analysis" | grep "名詞" | sed -n '2p' | cut -f1)

    # 属性（形容詞）を探す
    attribute=$(echo "$analysis" | grep "形容詞" | head -n 1 | cut -f3)

    # 時間表現を探す（「今日」「明日」など）
    time=$(echo "$analysis" | grep -E "名詞-副詞可能|名詞-時相名詞" | head -n 1 | cut -f1)

    # 場所表現を探す（「ここ」「そこ」など）
    location=$(echo "$analysis" | grep -E "名詞-代名詞-一般|名詞-固有名詞-地域" | head -n 1 | cut -f1)

    # 様態（副詞）を探す
    manner=$(echo "$analysis" | grep "副詞" | head -n 1 | cut -f1)

    # 数量表現を探す
    quantity=$(echo "$analysis" | grep -E "名詞-数|名詞-副詞可能" | grep -E "[0-9]|数|何" | head -n 1 | cut -f1)

    # 空の場合は「なし」に設定
    if [ -z "$subject" ]; then subject="(なし)"; fi
    if [ -z "$verb" ]; then verb="(なし)"; fi
    if [ -z "$object" ]; then object="(なし)"; fi
    if [ -z "$attribute" ]; then attribute="(なし)"; fi
    if [ -z "$time" ]; then time="(なし)"; fi
    if [ -z "$location" ]; then location="(なし)"; fi
    if [ -z "$manner" ]; then manner="(なし)"; fi
    if [ -z "$quantity" ]; then quantity="(なし)"; fi

    # 拡張DNAコードを生成
    local dna_code=""

    # 主語のコード（E + 番号）
    if [ "$subject" != "(なし)" ]; then
        local subject_id=$(grep -n "^E.*|$subject$" "$ENHANCED_DNA_DICT_FILE" | head -n 1 | cut -d':' -f1 | cut -d'|' -f1)
        if [ -z "$subject_id" ]; then
            # 新しいエントリを追加
            local last_e_id=$(grep "^E" "$ENHANCED_DNA_DICT_FILE" | wc -l)
            subject_id=$(printf "E%d" $last_e_id)
            echo "$subject_id|$subject" >> "$ENHANCED_DNA_DICT_FILE"
        fi
        dna_code="${dna_code}${subject_id}"
    fi

    # 動詞のコード（C + 番号）
    if [ "$verb" != "(なし)" ]; then
        local verb_id=$(grep -n "^C.*|$verb$" "$ENHANCED_DNA_DICT_FILE" | head -n 1 | cut -d':' -f1 | cut -d'|' -f1)
        if [ -z "$verb_id" ]; then
            # 新しいエントリを追加
            local last_c_id=$(grep "^C" "$ENHANCED_DNA_DICT_FILE" | wc -l)
            verb_id=$(printf "C%d" $last_c_id)
            echo "$verb_id|$verb" >> "$ENHANCED_DNA_DICT_FILE"
        fi
        dna_code="${dna_code}${verb_id}"
    fi

    # 目的語のコード（R + 番号）
    if [ "$object" != "(なし)" ]; then
        local object_id=$(grep -n "^R.*|$object$" "$ENHANCED_DNA_DICT_FILE" | head -n 1 | cut -d':' -f1 | cut -d'|' -f1)
        if [ -z "$object_id" ]; then
            # 新しいエントリを追加
            local last_r_id=$(grep "^R" "$ENHANCED_DNA_DICT_FILE" | wc -l)
            object_id=$(printf "R%d" $last_r_id)
            echo "$object_id|$object" >> "$ENHANCED_DNA_DICT_FILE"
        fi
        dna_code="${dna_code}${object_id}"
    fi

    # 属性のコード（A + 番号）
    if [ "$attribute" != "(なし)" ]; then
        local attribute_id=$(grep -n "^A.*|$attribute$" "$ENHANCED_DNA_DICT_FILE" | head -n 1 | cut -d':' -f1 | cut -d'|' -f1)
        if [ -z "$attribute_id" ]; then
            # 新しいエントリを追加
            local last_a_id=$(grep "^A" "$ENHANCED_DNA_DICT_FILE" | wc -l)
            attribute_id=$(printf "A%d" $last_a_id)
            echo "$attribute_id|$attribute" >> "$ENHANCED_DNA_DICT_FILE"
        fi
        dna_code="${dna_code}${attribute_id}"
    fi

    # 時間のコード（T + 番号）
    if [ "$time" != "(なし)" ]; then
        local time_id=$(grep -n "^T.*|$time$" "$ENHANCED_DNA_DICT_FILE" | head -n 1 | cut -d':' -f1 | cut -d'|' -f1)
        if [ -z "$time_id" ]; then
            # 新しいエントリを追加
            local last_t_id=$(grep "^T" "$ENHANCED_DNA_DICT_FILE" | wc -l)
            time_id=$(printf "T%d" $last_t_id)
            echo "$time_id|$time" >> "$ENHANCED_DNA_DICT_FILE"
        fi
        dna_code="${dna_code}${time_id}"
    fi

    # 場所のコード（L + 番号）
    if [ "$location" != "(なし)" ]; then
        local location_id=$(grep -n "^L.*|$location$" "$ENHANCED_DNA_DICT_FILE" | head -n 1 | cut -d':' -f1 | cut -d'|' -f1)
        if [ -z "$location_id" ]; then
            # 新しいエントリを追加
            local last_l_id=$(grep "^L" "$ENHANCED_DNA_DICT_FILE" | wc -l)
            location_id=$(printf "L%d" $last_l_id)
            echo "$location_id|$location" >> "$ENHANCED_DNA_DICT_FILE"
        fi
        dna_code="${dna_code}${location_id}"
    fi

    # 様態のコード（M + 番号）
    if [ "$manner" != "(なし)" ]; then
        local manner_id=$(grep -n "^M.*|$manner$" "$ENHANCED_DNA_DICT_FILE" | head -n 1 | cut -d':' -f1 | cut -d'|' -f1)
        if [ -z "$manner_id" ]; then
            # 新しいエントリを追加
            local last_m_id=$(grep "^M" "$ENHANCED_DNA_DICT_FILE" | wc -l)
            manner_id=$(printf "M%d" $last_m_id)
            echo "$manner_id|$manner" >> "$ENHANCED_DNA_DICT_FILE"
        fi
        dna_code="${dna_code}${manner_id}"
    fi

    # 数量のコード（Q + 番号）
    if [ "$quantity" != "(なし)" ]; then
        local quantity_id=$(grep -n "^Q.*|$quantity$" "$ENHANCED_DNA_DICT_FILE" | head -n 1 | cut -d':' -f1 | cut -d'|' -f1)
        if [ -z "$quantity_id" ]; then
            # 新しいエントリを追加
            local last_q_id=$(grep "^Q" "$ENHANCED_DNA_DICT_FILE" | wc -l)
            quantity_id=$(printf "Q%d" $last_q_id)
            echo "$quantity_id|$quantity" >> "$ENHANCED_DNA_DICT_FILE"
        fi
        dna_code="${dna_code}${quantity_id}"
    fi

    # 結果を表示
    echo "主語: $subject"
    echo "動詞: $verb"
    echo "目的語: $object"
    echo "属性: $attribute"
    echo "時間: $time"
    echo "場所: $location"
    echo "様態: $manner"
    echo "数量: $quantity"
    echo ""
    echo "拡張DNAコード: $dna_code"
}

# 拡張DNAコードから文を再構築
decompress_enhanced_dna() {
    local dna_code="$1"

    if [ -z "$dna_code" ]; then
        echo "エラー: 拡張DNAコードが指定されていません"
        exit 1
    fi

    echo "拡張DNAコード「$dna_code」の復元結果："
    echo "----------------------------------------"

    # 辞書ファイルが存在するか確認
    if [ ! -f "$ENHANCED_DNA_DICT_FILE" ]; then
        echo "エラー: 拡張DNA辞書ファイルが見つかりません: $ENHANCED_DNA_DICT_FILE"
        exit 1
    fi

    # 拡張DNAコードを解析
    local text=""
    local i=0
    while [ $i -lt ${#dna_code} ]; do
        # 最初の文字を取得（タイプ）
        local type="${dna_code:$i:1}"
        i=$((i + 1))
        
        # 数字部分を取得（ID）
        local id_start=$i
        while [[ $i -lt ${#dna_code} && "${dna_code:$i:1}" =~ [0-9] ]]; do
            i=$((i + 1))
        done
        
        # コードを組み立て
        local code="${type}${dna_code:$id_start:$i-$id_start}"

        # 辞書から単語を検索
        local word=$(grep "^$code|" "$ENHANCED_DNA_DICT_FILE" | cut -d'|' -f2)

        if [ -n "$word" ]; then
            case "${code:0:1}" in
                E) # 主語
                    text="${text}${word}は"
                    ;;
                C) # 動詞
                    text="${text}${word}"
                    ;;
                R) # 目的語
                    text="${text}${word}を"
                    ;;
                A) # 属性
                    text="${text}${word}な"
                    ;;
                T) # 時間
                    text="${text}${word}に"
                    ;;
                L) # 場所
                    text="${text}${word}で"
                    ;;
                M) # 様態
                    text="${text}${word}に"
                    ;;
                Q) # 数量
                    text="${text}${word}の"
                    ;;
                *)
                    text="${text}${word}"
                    ;;
            esac
        fi
    done

    echo "復元されたテキスト: $text"
}

# 拡張DNA辞書の内容を表示
show_enhanced_dna_dictionary() {
    if [ ! -f "$ENHANCED_DNA_DICT_FILE" ]; then
        echo "エラー: 拡張DNA辞書ファイルが見つかりません: $ENHANCED_DNA_DICT_FILE"
        exit 1
    fi

    echo "拡張DNA辞書の内容："
    echo "----------------------------------------"
    echo "コード | 単語"
    echo "----------------------------------------"

    # タイプごとに表示
    echo "【主語 (Entity)】"
    grep "^E" "$ENHANCED_DNA_DICT_FILE" | sort | sed 's/|/ | /'

    echo "【動詞 (Concept)】"
    grep "^C" "$ENHANCED_DNA_DICT_FILE" | sort | sed 's/|/ | /'

    echo "【目的語 (Result)】"
    grep "^R" "$ENHANCED_DNA_DICT_FILE" | sort | sed 's/|/ | /'

    echo "【属性 (Attribute)】"
    grep "^A" "$ENHANCED_DNA_DICT_FILE" | sort | sed 's/|/ | /'

    echo "【時間 (Time)】"
    grep "^T" "$ENHANCED_DNA_DICT_FILE" | sort | sed 's/|/ | /'

    echo "【場所 (Location)】"
    grep "^L" "$ENHANCED_DNA_DICT_FILE" | sort | sed 's/|/ | /'

    echo "【様態 (Manner)】"
    grep "^M" "$ENHANCED_DNA_DICT_FILE" | sort | sed 's/|/ | /'

    echo "【数量 (Quantity)】"
    grep "^Q" "$ENHANCED_DNA_DICT_FILE" | sort | sed 's/|/ | /'

    echo "----------------------------------------"
    echo "合計: $(wc -l < "$ENHANCED_DNA_DICT_FILE") エントリ"
}

# ヘルプを表示
show_help() {
    cat << EOF
使用方法: ./enhanced_dna.sh [コマンド] [引数]

コマンド:
  compress "テキスト"       テキストを拡張DNAコードに圧縮
  decompress "DNAコード"    拡張DNAコードからテキストを復元
  dictionary               拡張DNA辞書の内容を表示
  help                     このヘルプを表示

引数:
  "テキスト"                圧縮するテキスト
  "DNAコード"               復元する拡張DNAコード（例: E00C01R02A00T01）

拡張DNAコードの形式:
  E0〜E9999999999 - 主語(Entity)のID
  C0〜C9999999999 - 動詞(Concept)のID
  R0〜R9999999999 - 結果(Result)のID
  A0〜A9999999999 - 属性(Attribute)のID
  T0〜T9999999999 - 時間(Time)のID
  L0〜L9999999999 - 場所(Location)のID
  M0〜M9999999999 - 様態(Manner)のID
  Q0〜Q9999999999 - 数量(Quantity)のID
EOF
}

# メイン処理
case "$1" in
    compress)
        if [ -z "$2" ]; then
            echo "エラー: テキストが指定されていません"
            show_help
            exit 1
        fi
        generate_enhanced_dna "$2"
        ;;
    decompress)
        if [ -z "$2" ]; then
            echo "エラー: 拡張DNAコードが指定されていません"
            show_help
            exit 1
        fi
        decompress_enhanced_dna "$2"
        ;;
    dictionary)
        show_enhanced_dna_dictionary
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        echo "不明なコマンド: $1"
        show_help
        exit 1
        ;;
esac

exit 0