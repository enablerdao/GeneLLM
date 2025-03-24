#ifndef DNA_VECTOR_DB_H
#define DNA_VECTOR_DB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_DNA_VECTORS 100000   // 最大DNAベクトル数
#define DNA_CODE_MAX_LEN 128     // DNAコードの最大長

// DNAベクトルエントリの構造体
typedef struct {
    char dna_code[DNA_CODE_MAX_LEN];  // DNAコード (例: E0C5R1A7L2M3)
    float vector[64];                 // 64次元ベクトル表現
    int id;                           // エントリのID
} DNAVectorEntry;

// DNAベクトルデータベース
typedef struct {
    DNAVectorEntry entries[MAX_DNA_VECTORS];  // DNAベクトルエントリの配列
    int size;                                 // 現在のエントリ数
} DNAVectorDB;

// DNAベクトルデータベースの初期化
void init_dna_vector_db(DNAVectorDB* db);

// DNAベクトルデータベースのサイズを取得
int get_dna_vector_db_size(DNAVectorDB* db);

// DNAコードをベクトル化してデータベースに追加
int add_dna_vector(DNAVectorDB* db, const char* dna_code, int id);

// DNAコードからベクトルを生成
void generate_dna_vector(const char* dna_code, float* vector);

// 最も近いDNAコードを検索（ユークリッド距離）
int search_nearest_dna_euclidean(DNAVectorDB* db, const char* query_dna_code);

// 最も近いDNAコードを検索（コサイン類似度）
int search_nearest_dna_cosine(DNAVectorDB* db, const char* query_dna_code);

// DNAベクトルデータベースをファイルから読み込む
int load_dna_vector_db(DNAVectorDB* db, const char* filename);

// DNAベクトルデータベースをファイルに保存
int save_dna_vector_db(DNAVectorDB* db, const char* filename);

// DNAコードの類似度を計算
float calculate_dna_similarity(const char* dna_code1, const char* dna_code2);

// DNAコードの構造を解析
void parse_dna_code(const char* dna_code, char* entity, char* concept, char* result, 
                   char* attribute, char* time, char* location, char* manner, char* quantity);

#endif // DNA_VECTOR_DB_H