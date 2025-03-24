#!/bin/bash

# 質問からDNAコードを生成し、類似したDNAコードを検索する拡張スクリプト
# 使用方法: ./enhanced_dna_search.sh "質問文" [結果数]

WORKSPACE_DIR="/workspace"
ENHANCED_DNA_DICT_FILE="$WORKSPACE_DIR/data/enhanced_dna_dictionary.txt"
DNA_COMBINATIONS_FILE="$WORKSPACE_DIR/data/dna_combinations.txt"
TEMP_DIR="/tmp/genellm_dna"
MAX_RESULTS="${2:-10}"

# 必要なディレクトリを作成
mkdir -p "$TEMP_DIR"

# 引数をチェック
if [ $# -lt 1 ]; then
    echo "使用方法: $0 \"質問文\" [結果数]"
    exit 1
fi

QUERY="$1"
echo "質問: $QUERY"
echo "----------------------------------------"

# 質問からDNAコードを生成
generate_dna_from_query() {
    local query="$1"
    local dna_code="E0C3R1"  # デフォルト: GeneLLMは解析する知識ベースを
    
    # 質問文に特定のキーワードが含まれているかチェック
    if [[ "$query" == *"最適化"* || "$query" == *"optimize"* ]]; then
        dna_code="E0C5R1"  # GeneLLMは最適化する知識ベースを
    elif [[ "$query" == *"生成"* || "$query" == *"generate"* ]]; then
        dna_code="E0C4R1"  # GeneLLMは生成する知識ベースを
    elif [[ "$query" == *"学習"* || "$query" == *"learn"* ]]; then
        dna_code="E0C10R1"  # GeneLLMは学習する知識ベースを
    elif [[ "$query" == *"処理"* || "$query" == *"process"* ]]; then
        dna_code="E0C11R1"  # GeneLLMは処理する知識ベースを
    fi
    
    # 属性に関するキーワード
    if [[ "$query" == *"高速"* || "$query" == *"fast"* ]]; then
        dna_code="${dna_code}A0"  # 高速な
    elif [[ "$query" == *"効率"* || "$query" == *"efficient"* ]]; then
        dna_code="${dna_code}A1"  # 効率的な
    elif [[ "$query" == *"安定"* || "$query" == *"stable"* ]]; then
        dna_code="${dna_code}A2"  # 安定な
    fi
    
    # 場所に関するキーワード
    if [[ "$query" == *"メモリ"* || "$query" == *"memory"* ]]; then
        dna_code="${dna_code}L0"  # メモリ上で
    elif [[ "$query" == *"クラウド"* || "$query" == *"cloud"* ]]; then
        dna_code="${dna_code}L2"  # クラウドで
    elif [[ "$query" == *"サーバー"* || "$query" == *"server"* ]]; then
        dna_code="${dna_code}L4"  # サーバー上で
    fi
    
    # 様態に関するキーワード
    if [[ "$query" == *"効率的に"* || "$query" == *"efficiently"* ]]; then
        dna_code="${dna_code}M0"  # 効率的に
    elif [[ "$query" == *"安全に"* || "$query" == *"safely"* ]]; then
        dna_code="${dna_code}M1"  # 安全に
    elif [[ "$query" == *"自動的に"* || "$query" == *"automatically"* ]]; then
        dna_code="${dna_code}M2"  # 自動的に
    fi
    
    echo "$dna_code"
}

# DNAコードの類似度を計算
calculate_dna_similarity() {
    local dna1="$1"
    local dna2="$2"
    
    # 共通の要素数をカウント
    local common=0
    local total=0
    
    # 主語（E）の比較
    if [[ "${dna1:0:1}" == "E" && "${dna2:0:1}" == "E" ]]; then
        local e1_end=$(expr index "$dna1" "C")
        if [ $e1_end -eq 0 ]; then e1_end=${#dna1}; fi
        local e1="${dna1:0:$e1_end}"
        
        local e2_end=$(expr index "$dna2" "C")
        if [ $e2_end -eq 0 ]; then e2_end=${#dna2}; fi
        local e2="${dna2:0:$e2_end}"
        
        if [ "$e1" == "$e2" ]; then
            common=$((common + 1))
        fi
        total=$((total + 1))
    fi
    
    # 動詞（C）の比較
    if [[ "$dna1" == *"C"* && "$dna2" == *"C"* ]]; then
        local c1_start=$(expr index "$dna1" "C")
        local c1_end=$(expr index "${dna1:$c1_start}" "R")
        if [ $c1_end -eq 0 ]; then c1_end=${#dna1}; else c1_end=$((c1_start + c1_end - 1)); fi
        local c1="${dna1:$c1_start-1:$c1_end-$c1_start+1}"
        
        local c2_start=$(expr index "$dna2" "C")
        local c2_end=$(expr index "${dna2:$c2_start}" "R")
        if [ $c2_end -eq 0 ]; then c2_end=${#dna2}; else c2_end=$((c2_start + c2_end - 1)); fi
        local c2="${dna2:$c2_start-1:$c2_end-$c2_start+1}"
        
        if [ "$c1" == "$c2" ]; then
            common=$((common + 1))
        fi
        total=$((total + 1))
    fi
    
    # 目的語（R）の比較
    if [[ "$dna1" == *"R"* && "$dna2" == *"R"* ]]; then
        local r1_start=$(expr index "$dna1" "R")
        local r1_end=$(expr index "${dna1:$r1_start}" "A")
        if [ $r1_end -eq 0 ]; then 
            r1_end=$(expr index "${dna1:$r1_start}" "L")
            if [ $r1_end -eq 0 ]; then
                r1_end=$(expr index "${dna1:$r1_start}" "M")
                if [ $r1_end -eq 0 ]; then
                    r1_end=${#dna1}
                else
                    r1_end=$((r1_start + r1_end - 1))
                fi
            else
                r1_end=$((r1_start + r1_end - 1))
            fi
        else
            r1_end=$((r1_start + r1_end - 1))
        fi
        local r1="${dna1:$r1_start-1:$r1_end-$r1_start+1}"
        
        local r2_start=$(expr index "$dna2" "R")
        local r2_end=$(expr index "${dna2:$r2_start}" "A")
        if [ $r2_end -eq 0 ]; then
            r2_end=$(expr index "${dna2:$r2_start}" "L")
            if [ $r2_end -eq 0 ]; then
                r2_end=$(expr index "${dna2:$r2_start}" "M")
                if [ $r2_end -eq 0 ]; then
                    r2_end=${#dna2}
                else
                    r2_end=$((r2_start + r2_end - 1))
                fi
            else
                r2_end=$((r2_start + r2_end - 1))
            fi
        else
            r2_end=$((r2_start + r2_end - 1))
        fi
        local r2="${dna2:$r2_start-1:$r2_end-$r2_start+1}"
        
        if [ "$r1" == "$r2" ]; then
            common=$((common + 1))
        fi
        total=$((total + 1))
    fi
    
    # 属性（A）の比較
    if [[ "$dna1" == *"A"* && "$dna2" == *"A"* ]]; then
        local a1_start=$(expr index "$dna1" "A")
        local a1_end=$(expr index "${dna1:$a1_start}" "L")
        if [ $a1_end -eq 0 ]; then
            a1_end=$(expr index "${dna1:$a1_start}" "M")
            if [ $a1_end -eq 0 ]; then
                a1_end=${#dna1}
            else
                a1_end=$((a1_start + a1_end - 1))
            fi
        else
            a1_end=$((a1_start + a1_end - 1))
        fi
        local a1="${dna1:$a1_start-1:$a1_end-$a1_start+1}"
        
        local a2_start=$(expr index "$dna2" "A")
        local a2_end=$(expr index "${dna2:$a2_start}" "L")
        if [ $a2_end -eq 0 ]; then
            a2_end=$(expr index "${dna2:$a2_start}" "M")
            if [ $a2_end -eq 0 ]; then
                a2_end=${#dna2}
            else
                a2_end=$((a2_start + a2_end - 1))
            fi
        else
            a2_end=$((a2_start + a2_end - 1))
        fi
        local a2="${dna2:$a2_start-1:$a2_end-$a2_start+1}"
        
        if [ "$a1" == "$a2" ]; then
            common=$((common + 1))
        fi
        total=$((total + 1))
    fi
    
    # 場所（L）の比較
    if [[ "$dna1" == *"L"* && "$dna2" == *"L"* ]]; then
        local l1_start=$(expr index "$dna1" "L")
        local l1_end=$(expr index "${dna1:$l1_start}" "M")
        if [ $l1_end -eq 0 ]; then
            l1_end=${#dna1}
        else
            l1_end=$((l1_start + l1_end - 1))
        fi
        local l1="${dna1:$l1_start-1:$l1_end-$l1_start+1}"
        
        local l2_start=$(expr index "$dna2" "L")
        local l2_end=$(expr index "${dna2:$l2_start}" "M")
        if [ $l2_end -eq 0 ]; then
            l2_end=${#dna2}
        else
            l2_end=$((l2_start + l2_end - 1))
        fi
        local l2="${dna2:$l2_start-1:$l2_end-$l2_start+1}"
        
        if [ "$l1" == "$l2" ]; then
            common=$((common + 1))
        fi
        total=$((total + 1))
    fi
    
    # 様態（M）の比較
    if [[ "$dna1" == *"M"* && "$dna2" == *"M"* ]]; then
        local m1_start=$(expr index "$dna1" "M")
        local m1_end=${#dna1}
        local m1="${dna1:$m1_start-1:$m1_end-$m1_start+1}"
        
        local m2_start=$(expr index "$dna2" "M")
        local m2_end=${#dna2}
        local m2="${dna2:$m2_start-1:$m2_end-$m2_start+1}"
        
        if [ "$m1" == "$m2" ]; then
            common=$((common + 1))
        fi
        total=$((total + 1))
    fi
    
    # 類似度を計算（Jaccard係数）
    if [ $total -eq 0 ]; then
        echo "0.0"
    else
        # 主語が一致する場合は重み付け
        if [[ "${dna1:0:1}" == "E" && "${dna2:0:1}" == "E" ]]; then
            local e1_end=$(expr index "$dna1" "C")
            if [ $e1_end -eq 0 ]; then e1_end=${#dna1}; fi
            local e1="${dna1:0:$e1_end}"
            
            local e2_end=$(expr index "$dna2" "C")
            if [ $e2_end -eq 0 ]; then e2_end=${#dna2}; fi
            local e2="${dna2:0:$e2_end}"
            
            if [ "$e1" == "$e2" ]; then
                # 主語が一致する場合は類似度を1.5倍
                echo "scale=4; ($common * 1.5) / $total" | bc
                return
            fi
        fi
        
        echo "scale=4; $common / $total" | bc
    fi
}

# DNAコンビネーションファイルが存在するか確認
if [ ! -f "$DNA_COMBINATIONS_FILE" ]; then
    echo "DNAコンビネーションファイルが見つかりません: $DNA_COMBINATIONS_FILE"
    echo "generate_dna_combinations.shを実行して生成してください。"
    exit 1
fi

# 質問からDNAコードを生成
QUERY_DNA_CODE=$(generate_dna_from_query "$QUERY")
echo "質問から生成したDNAコード: $QUERY_DNA_CODE"

# 類似したDNAコードを検索
echo "類似したDNAコードを検索しています..."

# 結果を格納する一時ファイル
> "$TEMP_DIR/search_results.txt"

# 各DNAコードとの類似度を計算
while IFS='|' read -r dna_code description; do
    similarity=$(calculate_dna_similarity "$QUERY_DNA_CODE" "$dna_code")
    echo "$similarity|$dna_code|$description" >> "$TEMP_DIR/search_results.txt"
done < "$DNA_COMBINATIONS_FILE"

# 結果を類似度でソートして表示
echo "検索結果:"
echo "----------------------------------------"
sort -nr "$TEMP_DIR/search_results.txt" | head -n $MAX_RESULTS | while IFS='|' read -r similarity dna_code description; do
    printf "[%.4f] %s: %s\n" "$similarity" "$dna_code" "$description"
done

echo "検索完了"