#!/bin/bash

# 質問からDNAコードを生成し、類似したDNAコードを検索する最適化スクリプト
# 使用方法: ./optimized_dna_search.sh "質問文" [結果数]

WORKSPACE_DIR="/workspace"
ENHANCED_DNA_DICT_FILE="$WORKSPACE_DIR/data/enhanced_dna_dictionary.txt"
DNA_COMBINATIONS_FILE="$WORKSPACE_DIR/data/dna_combinations.txt"
TEMP_DIR="/tmp/genellm_dna"
MAX_RESULTS="${2:-20}"
TIMEOUT=10  # 検索タイムアウト（秒）

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
    
    # 主語の抽出
    local entity="E0"  # デフォルト: GeneLLM
    if [[ "$query" == *"知識グラフ"* ]]; then
        entity="E5"  # 知識グラフ
    elif [[ "$query" == *"自然言語処理"* ]]; then
        entity="E6"  # 自然言語処理
    elif [[ "$query" == *"機械学習"* ]]; then
        entity="E7"  # 機械学習
    elif [[ "$query" == *"深層学習"* ]]; then
        entity="E8"  # 深層学習
    elif [[ "$query" == *"強化学習"* ]]; then
        entity="E9"  # 強化学習
    elif [[ "$query" == *"トークナイザー"* ]]; then
        entity="E3"  # トークナイザー
    elif [[ "$query" == *"ベクトル検索"* ]]; then
        entity="E4"  # ベクトル検索
    fi
    
    # 動詞の抽出
    local concept="C3"  # デフォルト: 解析する
    if [[ "$query" == *"最適化"* || "$query" == *"optimize"* ]]; then
        concept="C5"  # 最適化する
    elif [[ "$query" == *"生成"* || "$query" == *"generate"* ]]; then
        concept="C4"  # 生成する
    elif [[ "$query" == *"学習"* || "$query" == *"learn"* ]]; then
        concept="C10"  # 学習する
    elif [[ "$query" == *"処理"* || "$query" == *"process"* ]]; then
        concept="C11"  # 処理する
    elif [[ "$query" == *"実装"* || "$query" == *"implement"* ]]; then
        concept="C1"  # 実装する
    elif [[ "$query" == *"高速化"* || "$query" == *"accelerate"* ]]; then
        concept="C2"  # 高速化する
    elif [[ "$query" == *"抽出"* || "$query" == *"extract"* ]]; then
        concept="C7"  # 抽出する
    elif [[ "$query" == *"分類"* || "$query" == *"classify"* ]]; then
        concept="C8"  # 分類する
    elif [[ "$query" == *"予測"* || "$query" == *"predict"* ]]; then
        concept="C9"  # 予測する
    elif [[ "$query" == *"分析"* || "$query" == *"analyze"* ]]; then
        concept="C3"  # 解析する
    fi
    
    # 目的語の抽出
    local result="R1"  # デフォルト: 知識ベース
    if [[ "$query" == *"テキストデータ"* || "$query" == *"text data"* ]]; then
        result="R3"  # テキストデータ
    elif [[ "$query" == *"バイナリデータ"* || "$query" == *"binary data"* ]]; then
        result="R4"  # バイナリデータ
    elif [[ "$query" == *"構造化データ"* || "$query" == *"structured data"* ]]; then
        result="R5"  # 構造化データ
    elif [[ "$query" == *"非構造化データ"* || "$query" == *"unstructured data"* ]]; then
        result="R6"  # 非構造化データ
    elif [[ "$query" == *"メタデータ"* || "$query" == *"metadata"* ]]; then
        result="R7"  # メタデータ
    elif [[ "$query" == *"特徴量"* || "$query" == *"feature"* ]]; then
        result="R8"  # 特徴量
    elif [[ "$query" == *"パターン"* || "$query" == *"pattern"* ]]; then
        result="R9"  # パターン
    elif [[ "$query" == *"モデル"* || "$query" == *"model"* ]]; then
        result="R10"  # モデル
    elif [[ "$query" == *"C言語"* || "$query" == *"C language"* ]]; then
        result="R0"  # C言語
    elif [[ "$query" == *"更新処理"* || "$query" == *"update process"* ]]; then
        result="R2"  # 更新処理
    elif [[ "$query" == *"データ"* || "$query" == *"data"* ]]; then
        # 非構造化データが明示的に指定されている場合
        if [[ "$query" == *"非構造化"* || "$query" == *"unstructured"* ]]; then
            result="R6"  # 非構造化データ
        # 構造化データが明示的に指定されている場合
        elif [[ "$query" == *"構造化"* || "$query" == *"structured"* ]]; then
            result="R5"  # 構造化データ
        else
            result="R4"  # バイナリデータ（一般的なデータ）
        fi
    fi
    
    # DNAコードの基本部分を構築
    dna_code="${entity}${concept}${result}"
    
    # 属性の抽出
    if [[ "$query" == *"高速"* || "$query" == *"fast"* ]]; then
        dna_code="${dna_code}A0"  # 高速な
    elif [[ "$query" == *"効率的"* || "$query" == *"efficient"* ]]; then
        dna_code="${dna_code}A1"  # 効率的な
    elif [[ "$query" == *"安定"* || "$query" == *"stable"* ]]; then
        dna_code="${dna_code}A2"  # 安定な
    elif [[ "$query" == *"信頼性"* || "$query" == *"reliable"* ]]; then
        dna_code="${dna_code}A3"  # 信頼性の高い
    elif [[ "$query" == *"スケーラブル"* || "$query" == *"scalable"* ]]; then
        dna_code="${dna_code}A4"  # スケーラブルな
    elif [[ "$query" == *"柔軟"* || "$query" == *"flexible"* ]]; then
        dna_code="${dna_code}A5"  # 柔軟な
    elif [[ "$query" == *"堅牢"* || "$query" == *"robust"* ]]; then
        dna_code="${dna_code}A6"  # 堅牢な
    elif [[ "$query" == *"最適化された"* || "$query" == *"optimized"* ]]; then
        dna_code="${dna_code}A7"  # 最適化された
    elif [[ "$query" == *"大規模"* || "$query" == *"large-scale"* ]]; then
        dna_code="${dna_code}A8"  # 先進的な
    fi
    
    # 場所の抽出
    if [[ "$query" == *"メモリ上"* || "$query" == *"in memory"* ]]; then
        dna_code="${dna_code}L0"  # メモリ上で
    elif [[ "$query" == *"ディスク"* || "$query" == *"disk"* ]]; then
        dna_code="${dna_code}L1"  # ディスクで
    elif [[ "$query" == *"クラウド"* || "$query" == *"cloud"* ]]; then
        dna_code="${dna_code}L2"  # クラウドで
    elif [[ "$query" == *"ローカル環境"* || "$query" == *"local environment"* ]]; then
        dna_code="${dna_code}L3"  # ローカル環境で
    elif [[ "$query" == *"サーバー"* || "$query" == *"server"* ]]; then
        dna_code="${dna_code}L4"  # サーバー上で
    elif [[ "$query" == *"データベース"* || "$query" == *"database"* ]]; then
        dna_code="${dna_code}L5"  # データベース内で
    elif [[ "$query" == *"ネットワーク"* || "$query" == *"network"* ]]; then
        dna_code="${dna_code}L6"  # ネットワーク上で
    elif [[ "$query" == *"分散システム"* || "$query" == *"distributed system"* ]]; then
        dna_code="${dna_code}L9"  # 分散システムで
    fi
    
    # 様態の抽出
    if [[ "$query" == *"効率的に"* || "$query" == *"efficiently"* ]]; then
        dna_code="${dna_code}M0"  # 効率的に
    elif [[ "$query" == *"安全に"* || "$query" == *"safely"* ]]; then
        dna_code="${dna_code}M1"  # 安全に
    elif [[ "$query" == *"自動的に"* || "$query" == *"automatically"* ]]; then
        dna_code="${dna_code}M2"  # 自動的に
    elif [[ "$query" == *"迅速に"* || "$query" == *"quickly"* ]]; then
        dna_code="${dna_code}M3"  # 迅速に
    elif [[ "$query" == *"正確に"* || "$query" == *"accurately"* ]]; then
        dna_code="${dna_code}M4"  # 正確に
    elif [[ "$query" == *"柔軟に"* || "$query" == *"flexibly"* ]]; then
        dna_code="${dna_code}M5"  # 柔軟に
    elif [[ "$query" == *"堅牢に"* || "$query" == *"robustly"* ]]; then
        dna_code="${dna_code}M6"  # 堅牢に
    elif [[ "$query" == *"スケーラブルに"* || "$query" == *"scalably"* ]]; then
        dna_code="${dna_code}M7"  # スケーラブルに
    elif [[ "$query" == *"継続的に"* || "$query" == *"continuously"* ]]; then
        dna_code="${dna_code}M9"  # 継続的に
    fi
    
    echo "$dna_code"
}

# DNAコードの類似度を計算（最適化版）
calculate_dna_similarity() {
    local dna1="$1"
    local dna2="$2"
    
    # 各要素の重み
    local entity_weight=3.0
    local concept_weight=2.0
    local result_weight=2.0
    local attribute_weight=1.0
    local location_weight=1.0
    local manner_weight=1.0
    
    local similarity_score=0
    local weight_sum=0
    
    # 主語（E）の比較
    if [[ "${dna1:0:1}" == "E" && "${dna2:0:1}" == "E" ]]; then
        local e1_end=$(expr index "$dna1" "C")
        if [ $e1_end -eq 0 ]; then e1_end=${#dna1}; fi
        local e1="${dna1:0:$e1_end}"
        
        local e2_end=$(expr index "$dna2" "C")
        if [ $e2_end -eq 0 ]; then e2_end=${#dna2}; fi
        local e2="${dna2:0:$e2_end}"
        
        weight_sum=$(echo "$weight_sum + $entity_weight" | bc)
        
        if [ "$e1" == "$e2" ]; then
            similarity_score=$(echo "$similarity_score + $entity_weight" | bc)
        fi
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
        
        weight_sum=$(echo "$weight_sum + $concept_weight" | bc)
        
        if [ "$c1" == "$c2" ]; then
            similarity_score=$(echo "$similarity_score + $concept_weight" | bc)
        fi
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
        
        weight_sum=$(echo "$weight_sum + $result_weight" | bc)
        
        if [ "$r1" == "$r2" ]; then
            similarity_score=$(echo "$similarity_score + $result_weight" | bc)
        fi
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
        
        weight_sum=$(echo "$weight_sum + $attribute_weight" | bc)
        
        if [ "$a1" == "$a2" ]; then
            similarity_score=$(echo "$similarity_score + $attribute_weight" | bc)
        fi
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
        
        weight_sum=$(echo "$weight_sum + $location_weight" | bc)
        
        if [ "$l1" == "$l2" ]; then
            similarity_score=$(echo "$similarity_score + $location_weight" | bc)
        fi
    fi
    
    # 様態（M）の比較
    if [[ "$dna1" == *"M"* && "$dna2" == *"M"* ]]; then
        local m1_start=$(expr index "$dna1" "M")
        local m1_end=${#dna1}
        local m1="${dna1:$m1_start-1:$m1_end-$m1_start+1}"
        
        local m2_start=$(expr index "$dna2" "M")
        local m2_end=${#dna2}
        local m2="${dna2:$m2_start-1:$m2_end-$m2_start+1}"
        
        weight_sum=$(echo "$weight_sum + $manner_weight" | bc)
        
        if [ "$m1" == "$m2" ]; then
            similarity_score=$(echo "$similarity_score + $manner_weight" | bc)
        fi
    fi
    
    # 最終的な類似度スコアを計算
    if [ $(echo "$weight_sum == 0" | bc) -eq 1 ]; then
        echo "0.0"
    else
        echo "scale=4; $similarity_score / $weight_sum" | bc
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

# 検索処理を最適化（サンプリング）
SAMPLE_SIZE=1000
echo "DNAコードデータベースからサンプリング中..."
shuf -n $SAMPLE_SIZE "$DNA_COMBINATIONS_FILE" > "$TEMP_DIR/sampled_dna_codes.txt"

# タイムアウト付きで検索を実行
echo "類似度計算中（タイムアウト: ${TIMEOUT}秒）..."
(
    # 各DNAコードとの類似度を計算
    while IFS='|' read -r dna_code description; do
        similarity=$(calculate_dna_similarity "$QUERY_DNA_CODE" "$dna_code")
        echo "$similarity|$dna_code|$description" >> "$TEMP_DIR/search_results.txt"
    done < "$TEMP_DIR/sampled_dna_codes.txt"
) & PID=$!

# タイムアウト処理
(sleep $TIMEOUT && kill -TERM $PID 2>/dev/null) & TIMEOUT_PID=$!

# 検索プロセスの終了を待機
wait $PID 2>/dev/null
SEARCH_STATUS=$?

# タイムアウトプロセスを終了
kill -TERM $TIMEOUT_PID 2>/dev/null

# 検索結果を表示
echo "検索結果:"
echo "----------------------------------------"

if [ $SEARCH_STATUS -eq 143 ]; then
    echo "検索がタイムアウトしました。部分的な結果を表示します。"
fi

# 結果を類似度でソートして表示
sort -nr "$TEMP_DIR/search_results.txt" | head -n $MAX_RESULTS | while IFS='|' read -r similarity dna_code description; do
    printf "[%.4f] %s: %s\n" "$similarity" "$dna_code" "$description"
done

echo "検索完了"