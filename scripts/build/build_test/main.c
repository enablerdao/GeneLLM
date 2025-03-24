#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ベクトルデータベース構造体の定義
#define VECTOR_DIM 64
#define MAX_VECTORS 1000

typedef struct {
    float vector[VECTOR_DIM];
    int id;
} VectorEntry;

typedef struct {
    VectorEntry entries[MAX_VECTORS];
    int size;
} VectorDB;

// グローバル変数の宣言
VectorDB global_vector_db;

// 初期化関数
void init_vector_db(VectorDB* db) {
    db->size = 0;
    printf("ベクトルデータベースを初期化しました\n");
}

// メイン関数
int main() {
    printf("GeneLLM テストプログラム\n");
    
    // ベクトルデータベースの初期化
    init_vector_db(&global_vector_db);
    
    printf("ベクトルデータベースのサイズ: %d\n", global_vector_db.size);
    
    return 0;
}
