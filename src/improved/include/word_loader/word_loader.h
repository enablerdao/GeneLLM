#ifndef WORD_LOADER_H
#define WORD_LOADER_H

#include <stdbool.h>
#include "../../vector_search/vector_search.h"

// 単語ベクトルをファイルから読み込む
int load_word_vectors(const char* filename, VectorDB* db, int offset);

// 単語ベクトルをファイルに保存
int save_word_vectors(const char* filename, VectorDB* db);

// 大規模な単語ベクトルデータセットを生成
int generate_large_word_vector_dataset(VectorDB* db, int target_size);

// 単語に対応するベクトルを取得
bool get_word_vector(const char* word, float* vector);

#endif // WORD_LOADER_H
