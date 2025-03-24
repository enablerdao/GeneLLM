#include "vector_db.h"
#include <stdio.h>

// グローバル変数の定義
VectorDB global_vector_db;

// 初期化関数
void init_vector_db(VectorDB* db) {
    db->size = 0;
    printf("ベクトルデータベースを初期化しました\n");
}

// グローバルベクトルDBの初期化
void init_global_vector_db() {
    init_vector_db(&global_vector_db);
    printf("グローバルベクトルデータベースを初期化しました\n");
}

// グローバルベクトルDBのサイズ取得
int get_global_vector_db_size() {
    return global_vector_db.size;
}
