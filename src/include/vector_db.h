#ifndef VECTOR_DB_H
#define VECTOR_DB_H

#include "../vector_search/vector_search.h"

// グローバルなベクトルデータベース
extern VectorDB global_vector_db;

// ベクトルデータベースを初期化
void init_global_vector_db();

// 現在のベクトルデータベースのサイズを取得
int get_global_vector_db_size();

#endif // VECTOR_DB_H