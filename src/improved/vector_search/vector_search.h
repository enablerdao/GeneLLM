#ifndef VECTOR_SEARCH_H
#define VECTOR_SEARCH_H

#define MAX_VECTORS 25000   // 最大ベクトル数を25000に増加
#define VECTOR_DIM 64      // ベクトルの次元数

// ベクトルとそのIDを格納する構造体
typedef struct {
    float vector[VECTOR_DIM];  // ベクトルデータ
    int id;                    // ベクトルのID
} VectorEntry;

// ベクトルデータベース
typedef struct {
    VectorEntry entries[MAX_VECTORS];  // ベクトルエントリの配列
    int size;                          // 現在のベクトル数
} VectorDB;

// ベクトルデータベースの初期化
void init_vector_db(VectorDB* db);

// ベクトルデータベースのサイズを取得
int get_vector_db_size(VectorDB* db);

// ベクトルをデータベースに追加
int add_vector(VectorDB* db, float* vector, int id);

// ユークリッド距離で最も近いベクトルを検索
int search_nearest_euclidean(VectorDB* db, float* query_vector);

// コサイン類似度で最も近いベクトルを検索
int search_nearest_cosine(VectorDB* db, float* query_vector);

// ランダムなベクトルを生成
void generate_random_vector(float* vector);

// ベクトルを正規化
void normalize_vector(float* vector);

// ベクトルの一部を表示
void print_vector_preview(float* vector, int preview_size);

// テスト用のベクトルをデータベースに追加
void add_test_vectors_to_db(VectorDB* db, int num_vectors);

#endif // VECTOR_SEARCH_H