#ifndef VECTOR_SEARCH_IMPROVED_H
#define VECTOR_SEARCH_IMPROVED_H

#include <stdbool.h>

#define MAX_VECTORS 25000   // 最大ベクトル数
#define VECTOR_DIM 64      // ベクトルの次元数
#define MAX_SEARCH_RESULTS 10  // 検索結果の最大数

// ベクトルとそのIDを格納する構造体
typedef struct {
    float vector[VECTOR_DIM];  // ベクトルデータ
    int id;                    // ベクトルのID
    float relevance;           // 関連性スコア（検索結果用）
} VectorEntry;

// 検索結果構造体
typedef struct {
    int ids[MAX_SEARCH_RESULTS];       // 検索結果のID配列
    float scores[MAX_SEARCH_RESULTS];  // 各結果のスコア
    int count;                         // 実際の結果数
} SearchResult;

// ベクトルデータベース
typedef struct {
    VectorEntry entries[MAX_VECTORS];  // ベクトルエントリの配列
    int size;                          // 現在のベクトル数
    bool use_index;                    // インデックスを使用するかどうか
    // 簡易インデックス構造（将来的に拡張可能）
    int* index_map;                    // インデックスマッピング
} VectorDB;

// ベクトルデータベースの初期化
void init_vector_db_improved(VectorDB* db);

// ベクトルデータベースの解放
void free_vector_db_improved(VectorDB* db);

// ベクトルデータベースのサイズを取得
int get_vector_db_size_improved(VectorDB* db);

// ベクトルをデータベースに追加
int add_vector_improved(VectorDB* db, float* vector, int id);

// ユークリッド距離で最も近いベクトルを検索（複数結果）
SearchResult search_nearest_euclidean_improved(VectorDB* db, float* query_vector, int max_results);

// コサイン類似度で最も近いベクトルを検索（複数結果）
SearchResult search_nearest_cosine_improved(VectorDB* db, float* query_vector, int max_results);

// ハイブリッド検索（コサイン類似度とキーワードマッチングの組み合わせ）
SearchResult search_hybrid_improved(VectorDB* db, float* query_vector, const char* query_text, int max_results);

// ランダムなベクトルを生成
void generate_random_vector_improved(float* vector);

// ベクトルを正規化
void normalize_vector_improved(float* vector);

// ベクトルの一部を表示
void print_vector_preview_improved(float* vector, int preview_size);

// ベクトルデータベースのインデックスを構築
void build_vector_db_index(VectorDB* db);

// ベクトル間のユークリッド距離を計算
float euclidean_distance_improved(float* v1, float* v2);

// ベクトル間のコサイン類似度を計算
float cosine_similarity_improved(float* v1, float* v2);

// 検索結果をスコアでソート
void sort_search_results(SearchResult* results);

#endif // VECTOR_SEARCH_IMPROVED_H