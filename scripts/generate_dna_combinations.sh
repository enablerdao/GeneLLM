#!/bin/bash

# DNAコードの組み合わせを生成するスクリプト
# 使用方法: ./generate_dna_combinations.sh [出力ファイル] [組み合わせ数]

WORKSPACE_DIR="/workspace"
ENHANCED_DNA_DICT_FILE="$WORKSPACE_DIR/data/enhanced_dna_dictionary.txt"
OUTPUT_FILE="${1:-$WORKSPACE_DIR/data/dna_combinations.txt}"
MAX_COMBINATIONS="${2:-1000}"
TEMP_DIR="/tmp/genellm_dna"

# 必要なディレクトリを作成
mkdir -p "$TEMP_DIR"
mkdir -p "$(dirname "$OUTPUT_FILE")"

echo "DNAコードの組み合わせを生成しています..."
echo "出力ファイル: $OUTPUT_FILE"
echo "最大組み合わせ数: $MAX_COMBINATIONS"
echo "----------------------------------------"

# 既存のファイルをバックアップ
if [ -f "$OUTPUT_FILE" ]; then
    cp "$OUTPUT_FILE" "${OUTPUT_FILE}.bak"
    echo "既存のファイルをバックアップしました: ${OUTPUT_FILE}.bak"
fi

# 出力ファイルを初期化
> "$OUTPUT_FILE"

# 主語（Entity）のエントリを取得
grep "^E" "$ENHANCED_DNA_DICT_FILE" > "$TEMP_DIR/entity_entries.txt"
ENTITY_COUNT=$(wc -l < "$TEMP_DIR/entity_entries.txt")
echo "主語(Entity)エントリ数: $ENTITY_COUNT"

# 動詞（Concept）のエントリを取得
grep "^C" "$ENHANCED_DNA_DICT_FILE" > "$TEMP_DIR/concept_entries.txt"
CONCEPT_COUNT=$(wc -l < "$TEMP_DIR/concept_entries.txt")
echo "動詞(Concept)エントリ数: $CONCEPT_COUNT"

# 目的語（Result）のエントリを取得
grep "^R" "$ENHANCED_DNA_DICT_FILE" > "$TEMP_DIR/result_entries.txt"
RESULT_COUNT=$(wc -l < "$TEMP_DIR/result_entries.txt")
echo "目的語(Result)エントリ数: $RESULT_COUNT"

# 属性（Attribute）のエントリを取得
grep "^A" "$ENHANCED_DNA_DICT_FILE" > "$TEMP_DIR/attribute_entries.txt"
ATTRIBUTE_COUNT=$(wc -l < "$TEMP_DIR/attribute_entries.txt")
echo "属性(Attribute)エントリ数: $ATTRIBUTE_COUNT"

# 場所（Location）のエントリを取得
grep "^L" "$ENHANCED_DNA_DICT_FILE" > "$TEMP_DIR/location_entries.txt"
LOCATION_COUNT=$(wc -l < "$TEMP_DIR/location_entries.txt")
echo "場所(Location)エントリ数: $LOCATION_COUNT"

# 様態（Manner）のエントリを取得
grep "^M" "$ENHANCED_DNA_DICT_FILE" > "$TEMP_DIR/manner_entries.txt"
MANNER_COUNT=$(wc -l < "$TEMP_DIR/manner_entries.txt")
echo "様態(Manner)エントリ数: $MANNER_COUNT"

# 理論上の最大組み合わせ数を計算
THEORETICAL_MAX=$((ENTITY_COUNT * CONCEPT_COUNT * RESULT_COUNT))
echo "理論上の最大組み合わせ数(ECR): $THEORETICAL_MAX"

# 実際に生成する組み合わせ数を決定
if [ $MAX_COMBINATIONS -gt $THEORETICAL_MAX ]; then
    MAX_COMBINATIONS=$THEORETICAL_MAX
    echo "最大組み合わせ数を理論上の最大値に調整: $MAX_COMBINATIONS"
fi

# DNAコードの組み合わせを生成
echo "DNAコードの組み合わせを生成しています..."
COUNTER=0

# 主要な主語を取得（最初の20エントリ）
head -n 20 "$TEMP_DIR/entity_entries.txt" > "$TEMP_DIR/primary_entities.txt"

# 主要な動詞を取得（最初の20エントリ）
head -n 20 "$TEMP_DIR/concept_entries.txt" > "$TEMP_DIR/primary_concepts.txt"

# 主要な目的語を取得（最初の20エントリ）
head -n 20 "$TEMP_DIR/result_entries.txt" > "$TEMP_DIR/primary_results.txt"

# 主要な属性を取得（最初の10エントリ）
head -n 10 "$TEMP_DIR/attribute_entries.txt" > "$TEMP_DIR/primary_attributes.txt"

# 主要な場所を取得（最初の10エントリ）
head -n 10 "$TEMP_DIR/location_entries.txt" > "$TEMP_DIR/primary_locations.txt"

# 主要な様態を取得（最初の10エントリ）
head -n 10 "$TEMP_DIR/manner_entries.txt" > "$TEMP_DIR/primary_manners.txt"

# 基本的なECR（主語・動詞・目的語）の組み合わせを生成
echo "基本的なECR組み合わせを生成しています..."
while read -r entity_line; do
    entity_code=$(echo "$entity_line" | cut -d'|' -f1)
    entity_word=$(echo "$entity_line" | cut -d'|' -f2)
    
    while read -r concept_line; do
        concept_code=$(echo "$concept_line" | cut -d'|' -f1)
        concept_word=$(echo "$concept_line" | cut -d'|' -f2)
        
        while read -r result_line; do
            result_code=$(echo "$result_line" | cut -d'|' -f1)
            result_word=$(echo "$result_line" | cut -d'|' -f2)
            
            # ECRの組み合わせを生成
            dna_code="${entity_code}${concept_code}${result_code}"
            echo "$dna_code|${entity_word}は${concept_word}${result_word}を" >> "$OUTPUT_FILE"
            COUNTER=$((COUNTER + 1))
            
            # 一部の組み合わせに属性を追加
            if [ $((COUNTER % 5)) -eq 0 ] && [ $COUNTER -lt $((MAX_COMBINATIONS * 4 / 5)) ]; then
                # ランダムに属性を選択
                attribute_line=$(shuf -n 1 "$TEMP_DIR/primary_attributes.txt")
                attribute_code=$(echo "$attribute_line" | cut -d'|' -f1)
                attribute_word=$(echo "$attribute_line" | cut -d'|' -f2)
                
                # ECRA（主語・動詞・目的語・属性）の組み合わせを生成
                dna_code="${entity_code}${concept_code}${result_code}${attribute_code}"
                echo "$dna_code|${entity_word}は${concept_word}${result_word}を${attribute_word}な" >> "$OUTPUT_FILE"
                COUNTER=$((COUNTER + 1))
            fi
            
            # 一部の組み合わせに場所を追加
            if [ $((COUNTER % 7)) -eq 0 ] && [ $COUNTER -lt $((MAX_COMBINATIONS * 4 / 5)) ]; then
                # ランダムに場所を選択
                location_line=$(shuf -n 1 "$TEMP_DIR/primary_locations.txt")
                location_code=$(echo "$location_line" | cut -d'|' -f1)
                location_word=$(echo "$location_line" | cut -d'|' -f2)
                
                # ECRL（主語・動詞・目的語・場所）の組み合わせを生成
                dna_code="${entity_code}${concept_code}${result_code}${location_code}"
                echo "$dna_code|${entity_word}は${concept_word}${result_word}を${location_word}で" >> "$OUTPUT_FILE"
                COUNTER=$((COUNTER + 1))
            fi
            
            # 一部の組み合わせに様態を追加
            if [ $((COUNTER % 11)) -eq 0 ] && [ $COUNTER -lt $((MAX_COMBINATIONS * 4 / 5)) ]; then
                # ランダムに様態を選択
                manner_line=$(shuf -n 1 "$TEMP_DIR/primary_manners.txt")
                manner_code=$(echo "$manner_line" | cut -d'|' -f1)
                manner_word=$(echo "$manner_line" | cut -d'|' -f2)
                
                # ECRM（主語・動詞・目的語・様態）の組み合わせを生成
                dna_code="${entity_code}${concept_code}${result_code}${manner_code}"
                echo "$dna_code|${entity_word}は${concept_word}${result_word}を${manner_word}" >> "$OUTPUT_FILE"
                COUNTER=$((COUNTER + 1))
            fi
            
            # 一部の組み合わせに属性と場所を追加
            if [ $((COUNTER % 13)) -eq 0 ] && [ $COUNTER -lt $((MAX_COMBINATIONS * 4 / 5)) ]; then
                # ランダムに属性と場所を選択
                attribute_line=$(shuf -n 1 "$TEMP_DIR/primary_attributes.txt")
                attribute_code=$(echo "$attribute_line" | cut -d'|' -f1)
                attribute_word=$(echo "$attribute_line" | cut -d'|' -f2)
                
                location_line=$(shuf -n 1 "$TEMP_DIR/primary_locations.txt")
                location_code=$(echo "$location_line" | cut -d'|' -f1)
                location_word=$(echo "$location_line" | cut -d'|' -f2)
                
                # ECRAL（主語・動詞・目的語・属性・場所）の組み合わせを生成
                dna_code="${entity_code}${concept_code}${result_code}${attribute_code}${location_code}"
                echo "$dna_code|${entity_word}は${concept_word}${result_word}を${attribute_word}な${location_word}で" >> "$OUTPUT_FILE"
                COUNTER=$((COUNTER + 1))
            fi
            
            # 一部の組み合わせに属性と様態を追加
            if [ $((COUNTER % 17)) -eq 0 ] && [ $COUNTER -lt $((MAX_COMBINATIONS * 4 / 5)) ]; then
                # ランダムに属性と様態を選択
                attribute_line=$(shuf -n 1 "$TEMP_DIR/primary_attributes.txt")
                attribute_code=$(echo "$attribute_line" | cut -d'|' -f1)
                attribute_word=$(echo "$attribute_line" | cut -d'|' -f2)
                
                manner_line=$(shuf -n 1 "$TEMP_DIR/primary_manners.txt")
                manner_code=$(echo "$manner_line" | cut -d'|' -f1)
                manner_word=$(echo "$manner_line" | cut -d'|' -f2)
                
                # ECRAM（主語・動詞・目的語・属性・様態）の組み合わせを生成
                dna_code="${entity_code}${concept_code}${result_code}${attribute_code}${manner_code}"
                echo "$dna_code|${entity_word}は${concept_word}${result_word}を${attribute_word}な${manner_word}" >> "$OUTPUT_FILE"
                COUNTER=$((COUNTER + 1))
            fi
            
            # 一部の組み合わせに場所と様態を追加
            if [ $((COUNTER % 19)) -eq 0 ] && [ $COUNTER -lt $((MAX_COMBINATIONS * 4 / 5)) ]; then
                # ランダムに場所と様態を選択
                location_line=$(shuf -n 1 "$TEMP_DIR/primary_locations.txt")
                location_code=$(echo "$location_line" | cut -d'|' -f1)
                location_word=$(echo "$location_line" | cut -d'|' -f2)
                
                manner_line=$(shuf -n 1 "$TEMP_DIR/primary_manners.txt")
                manner_code=$(echo "$manner_line" | cut -d'|' -f1)
                manner_word=$(echo "$manner_line" | cut -d'|' -f2)
                
                # ECRLM（主語・動詞・目的語・場所・様態）の組み合わせを生成
                dna_code="${entity_code}${concept_code}${result_code}${location_code}${manner_code}"
                echo "$dna_code|${entity_word}は${concept_word}${result_word}を${location_word}で${manner_word}" >> "$OUTPUT_FILE"
                COUNTER=$((COUNTER + 1))
            fi
            
            # 一部の組み合わせに属性、場所、様態を追加
            if [ $((COUNTER % 23)) -eq 0 ] && [ $COUNTER -lt $((MAX_COMBINATIONS * 4 / 5)) ]; then
                # ランダムに属性、場所、様態を選択
                attribute_line=$(shuf -n 1 "$TEMP_DIR/primary_attributes.txt")
                attribute_code=$(echo "$attribute_line" | cut -d'|' -f1)
                attribute_word=$(echo "$attribute_line" | cut -d'|' -f2)
                
                location_line=$(shuf -n 1 "$TEMP_DIR/primary_locations.txt")
                location_code=$(echo "$location_line" | cut -d'|' -f1)
                location_word=$(echo "$location_line" | cut -d'|' -f2)
                
                manner_line=$(shuf -n 1 "$TEMP_DIR/primary_manners.txt")
                manner_code=$(echo "$manner_line" | cut -d'|' -f1)
                manner_word=$(echo "$manner_line" | cut -d'|' -f2)
                
                # ECRALM（主語・動詞・目的語・属性・場所・様態）の組み合わせを生成
                dna_code="${entity_code}${concept_code}${result_code}${attribute_code}${location_code}${manner_code}"
                echo "$dna_code|${entity_word}は${concept_word}${result_word}を${attribute_word}な${location_word}で${manner_word}" >> "$OUTPUT_FILE"
                COUNTER=$((COUNTER + 1))
            fi
            
            # 一定数生成したら終了
            if [ $COUNTER -ge $MAX_COMBINATIONS ]; then
                break 3
            fi
        done < <(shuf -n 5 "$TEMP_DIR/primary_results.txt")
    done < <(shuf -n 5 "$TEMP_DIR/primary_concepts.txt")
done < <(shuf -n 5 "$TEMP_DIR/primary_entities.txt")

# 残りの組み合わせをランダムに生成
echo "追加のランダムな組み合わせを生成しています..."
while [ $COUNTER -lt $MAX_COMBINATIONS ]; do
    # ランダムに主語、動詞、目的語を選択
    entity_line=$(shuf -n 1 "$TEMP_DIR/entity_entries.txt")
    entity_code=$(echo "$entity_line" | cut -d'|' -f1)
    entity_word=$(echo "$entity_line" | cut -d'|' -f2)
    
    concept_line=$(shuf -n 1 "$TEMP_DIR/concept_entries.txt")
    concept_code=$(echo "$concept_line" | cut -d'|' -f1)
    concept_word=$(echo "$concept_line" | cut -d'|' -f2)
    
    result_line=$(shuf -n 1 "$TEMP_DIR/result_entries.txt")
    result_code=$(echo "$result_line" | cut -d'|' -f1)
    result_word=$(echo "$result_line" | cut -d'|' -f2)
    
    # 追加要素をランダムに決定
    EXTRA_ELEMENTS=$((RANDOM % 4))
    
    # 基本のECR
    dna_code="${entity_code}${concept_code}${result_code}"
    description="${entity_word}は${concept_word}${result_word}を"
    
    # 追加要素を付加
    if [ $EXTRA_ELEMENTS -ge 1 ]; then
        # 属性を追加
        attribute_line=$(shuf -n 1 "$TEMP_DIR/attribute_entries.txt")
        attribute_code=$(echo "$attribute_line" | cut -d'|' -f1)
        attribute_word=$(echo "$attribute_line" | cut -d'|' -f2)
        
        dna_code="${dna_code}${attribute_code}"
        description="${description}${attribute_word}な"
    fi
    
    if [ $EXTRA_ELEMENTS -ge 2 ]; then
        # 場所を追加
        location_line=$(shuf -n 1 "$TEMP_DIR/location_entries.txt")
        location_code=$(echo "$location_line" | cut -d'|' -f1)
        location_word=$(echo "$location_line" | cut -d'|' -f2)
        
        dna_code="${dna_code}${location_code}"
        description="${description}${location_word}で"
    fi
    
    if [ $EXTRA_ELEMENTS -ge 3 ]; then
        # 様態を追加
        manner_line=$(shuf -n 1 "$TEMP_DIR/manner_entries.txt")
        manner_code=$(echo "$manner_line" | cut -d'|' -f1)
        manner_word=$(echo "$manner_line" | cut -d'|' -f2)
        
        dna_code="${dna_code}${manner_code}"
        description="${description}${manner_word}"
    fi
    
    echo "$dna_code|$description" >> "$OUTPUT_FILE"
    COUNTER=$((COUNTER + 1))
done

# 生成したDNAコードの数を表示
GENERATED_COUNT=$(wc -l < "$OUTPUT_FILE")
echo "----------------------------------------"
echo "生成したDNAコード数: $GENERATED_COUNT"
echo "出力ファイル: $OUTPUT_FILE"
echo "完了しました"