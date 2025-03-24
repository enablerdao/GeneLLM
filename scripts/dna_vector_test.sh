#!/bin/bash

# DNAベクトルデータベースのテストスクリプト
# 使用方法: ./dna_vector_test.sh

WORKSPACE_DIR="/workspace"
ENHANCED_DNA_DICT_FILE="$WORKSPACE_DIR/data/enhanced_dna_dictionary.txt"
DNA_VECTOR_DB_FILE="$WORKSPACE_DIR/data/dna_vector_db.bin"
TEMP_DIR="/tmp/genellm_dna"

# 必要なディレクトリを作成
mkdir -p "$TEMP_DIR"

# テスト用のDNAコードを生成
echo "テスト用のDNAコードを生成しています..."
cat > "$TEMP_DIR/test_dna_codes.txt" << EOF
E0C5R1A7L2M3
E1C1R0A1L0M0
E2C2R2A2L1M1
E0C0R0
E1C1R1
E2C2R2
E0C5R1A7
E1C1R0A1
E2C2R2A2
E0C5R1A7L2
E1C1R0A1L0
E2C2R2A2L1
EOF

# テスト用のC言語プログラムを作成
echo "テスト用のプログラムを作成しています..."
cat > "$TEMP_DIR/dna_vector_test.c" << EOF
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/workspace/src/include/dna_vector_db.h"

int main() {
    printf("DNAベクトルデータベースのテスト\n");
    printf("----------------------------------------\n");
    
    // DNAベクトルデータベースを初期化
    DNAVectorDB db;
    init_dna_vector_db(&db);
    
    // テスト用のDNAコードを読み込む
    FILE* fp = fopen("$TEMP_DIR/test_dna_codes.txt", "r");
    if (!fp) {
        printf("テストファイルを開けませんでした\n");
        return 1;
    }
    
    char dna_code[128];
    int id = 0;
    
    printf("DNAコードをベクトル化してデータベースに追加しています...\n");
    while (fgets(dna_code, sizeof(dna_code), fp)) {
        // 改行を削除
        dna_code[strcspn(dna_code, "\n")] = 0;
        
        // DNAコードをベクトル化してデータベースに追加
        if (add_dna_vector(&db, dna_code, id)) {
            printf("  追加: %s (ID: %d)\n", dna_code, id);
            id++;
        } else {
            printf("  追加失敗: %s\n", dna_code);
        }
    }
    fclose(fp);
    
    printf("DNAベクトルデータベースのサイズ: %d\n", get_dna_vector_db_size(&db));
    
    // DNAコードの類似度テスト
    printf("\nDNAコードの類似度テスト:\n");
    const char* test_pairs[][2] = {
        {"E0C5R1A7L2M3", "E0C5R1A7L2M3"},  // 完全一致
        {"E0C5R1A7L2M3", "E0C5R1A7L2M4"},  // 1要素違い
        {"E0C5R1A7L2M3", "E0C5R1A7L3M3"},  // 1要素違い
        {"E0C5R1A7L2M3", "E0C5R1A8L2M3"},  // 1要素違い
        {"E0C5R1A7L2M3", "E0C5R2A7L2M3"},  // 1要素違い
        {"E0C5R1A7L2M3", "E0C6R1A7L2M3"},  // 1要素違い
        {"E0C5R1A7L2M3", "E1C5R1A7L2M3"},  // 1要素違い
        {"E0C5R1A7L2M3", "E1C6R2A8L3M4"},  // 全要素違い
        {"E0C5R1", "E0C5R1A7L2M3"},        // 部分一致
        {"E0C5", "E0C5R1A7L2M3"},          // 部分一致
        {"E0", "E0C5R1A7L2M3"}             // 部分一致
    };
    
    for (int i = 0; i < sizeof(test_pairs) / sizeof(test_pairs[0]); i++) {
        float similarity = calculate_dna_similarity(test_pairs[i][0], test_pairs[i][1]);
        printf("  %s と %s の類似度: %.4f\n", test_pairs[i][0], test_pairs[i][1], similarity);
    }
    
    // DNAベクトルデータベースをファイルに保存
    printf("\nDNAベクトルデータベースをファイルに保存しています...\n");
    if (save_dna_vector_db(&db, "$DNA_VECTOR_DB_FILE")) {
        printf("  保存成功: $DNA_VECTOR_DB_FILE\n");
    } else {
        printf("  保存失敗\n");
    }
    
    // DNAベクトルデータベースをファイルから読み込む
    printf("\nDNAベクトルデータベースをファイルから読み込んでいます...\n");
    DNAVectorDB loaded_db;
    int loaded_count = load_dna_vector_db(&loaded_db, "$DNA_VECTOR_DB_FILE");
    if (loaded_count > 0) {
        printf("  読み込み成功: %d エントリ\n", loaded_count);
    } else {
        printf("  読み込み失敗\n");
    }
    
    // 最も近いDNAコードを検索
    printf("\n最も近いDNAコードを検索しています...\n");
    const char* query_dna_code = "E0C5R1A7L2M4";  // M3からM4に変更
    
    printf("  クエリ: %s\n", query_dna_code);
    
    int nearest_euclidean = search_nearest_dna_euclidean(&loaded_db, query_dna_code);
    if (nearest_euclidean >= 0) {
        printf("  ユークリッド距離で最も近いDNAコード: %s (ID: %d)\n", 
               loaded_db.entries[nearest_euclidean].dna_code, nearest_euclidean);
    } else {
        printf("  ユークリッド距離での検索に失敗しました\n");
    }
    
    int nearest_cosine = search_nearest_dna_cosine(&loaded_db, query_dna_code);
    if (nearest_cosine >= 0) {
        printf("  コサイン類似度で最も近いDNAコード: %s (ID: %d)\n", 
               loaded_db.entries[nearest_cosine].dna_code, nearest_cosine);
    } else {
        printf("  コサイン類似度での検索に失敗しました\n");
    }
    
    printf("\nテスト完了\n");
    return 0;
}
EOF

# テストプログラムをコンパイル
echo "テストプログラムをコンパイルしています..."
gcc -o "$TEMP_DIR/dna_vector_test" "$TEMP_DIR/dna_vector_test.c" "$WORKSPACE_DIR/src/include/dna_vector_db.c" -lm

# テストプログラムを実行
echo "テストプログラムを実行しています..."
"$TEMP_DIR/dna_vector_test"

echo "テスト完了"