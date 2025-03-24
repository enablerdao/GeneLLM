#ifndef WORD_LOADER_H
#define WORD_LOADER_H

#include "../vector_search/vector_search.h"

// 単語ベクトルをファイルから読み込む
// filename: 単語ベクトルファイルのパス
// db: ベクトルを格納するデータベース
// max_words: 読み込む最大単語数（0の場合は制限なし）
// 戻り値: 読み込んだ単語数
int load_word_vectors(const char* filename, VectorDB* db, int max_words);

// 単語ベクトルをファイルに保存する
// filename: 保存先ファイルのパス
// db: 保存するベクトルデータベース
// 戻り値: 保存した単語数
int save_word_vectors(const char* filename, VectorDB* db);

// Webから単語リストを取得して単語ベクトルを生成
// url: 単語リストを取得するURL
// db: ベクトルを格納するデータベース
// max_words: 取得する最大単語数
// 戻り値: 生成した単語ベクトル数
int fetch_word_vectors_from_web(const char* url, VectorDB* db, int max_words);

// 単語ベクトルを大量に生成する関数
// db: ベクトルを格納するデータベース
// target_size: 目標とするベクトル数
// 戻り値: 生成後のベクトル総数
int generate_large_word_vector_dataset(VectorDB* db, int target_size);

#endif // WORD_LOADER_H