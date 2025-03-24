#ifndef VECTOR_DB_H
#define VECTOR_DB_H

#include "../vector_search/vector_search.h"

// グローバル変数の宣言
extern VectorDB global_vector_db;

// 初期化関数
void init_vector_db(VectorDB* db);

// グローバルベクトルDBの初期化
void init_global_vector_db();

// グローバルベクトルDBのサイズ取得
int get_global_vector_db_size();

#endif // VECTOR_DB_H
