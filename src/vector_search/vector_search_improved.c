#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include "vector_search_improved.h"

// ベクトルデータベースの初期化
void init_vector_db_improved(VectorDB* db) {
    db->size = 0;
    db->use_index = false;
    db->index_map = NULL;
}

// ベクトルデータベースの解放
void free_vector_db_improved(VectorDB* db) {
    if (db->index_map) {
        free(db->index_map);
        db->index_map = NULL;
    }
    db->size = 0;
    db->use_index = false;
}

// ベクトルデータベースのサイズを取得
int get_vector_db_size_improved(VectorDB* db) {
    return db->size;
}

// ベクトルをデータベースに追加
int add_vector_improved(VectorDB* db, float* vector, int id) {
    if (db->size >= MAX_VECTORS) {
        return 0;  // データベースが満杯
    }
    
    // ベクトルをコピー
    for (int i = 0; i < VECTOR_DIM; i++) {
        db->entries[db->size].vector[i] = vector[i];
    }
    db->entries[db->size].id = id;
    db->entries[db->size].relevance = 0.0f;
    db->size++;
    
    // インデックスが構築されている場合は無効化
    if (db->use_index) {
        db->use_index = false;
        if (db->index_map) {
            free(db->index_map);
            db->index_map = NULL;
        }
    }
    
    return 1;  // 成功
}

// ユークリッド距離の計算
float euclidean_distance_improved(float* v1, float* v2) {
    float sum = 0.0f;
    for (int i = 0; i < VECTOR_DIM; i++) {
        float diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    return sqrtf(sum);
}

// コサイン類似度の計算
float cosine_similarity_improved(float* v1, float* v2) {
    float dot_product = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (int i = 0; i < VECTOR_DIM; i++) {
        dot_product += v1[i] * v2[i];
        norm1 += v1[i] * v1[i];
        norm2 += v2[i] * v2[i];
    }
    
    if (norm1 == 0.0f || norm2 == 0.0f) {
        return 0.0f;
    }
    
    return dot_product / (sqrtf(norm1) * sqrtf(norm2));
}

// 検索結果をスコアでソート（降順）
void sort_search_results(SearchResult* results) {
    // バブルソート（結果数は少ないので十分）
    for (int i = 0; i < results->count - 1; i++) {
        for (int j = 0; j < results->count - i - 1; j++) {
            if (results->scores[j] < results->scores[j + 1]) {
                // スコアの交換
                float temp_score = results->scores[j];
                results->scores[j] = results->scores[j + 1];
                results->scores[j + 1] = temp_score;
                
                // IDの交換
                int temp_id = results->ids[j];
                results->ids[j] = results->ids[j + 1];
                results->ids[j + 1] = temp_id;
            }
        }
    }
}

// 最も近いベクトルを検索（ユークリッド距離、複数結果）
SearchResult search_nearest_euclidean_improved(VectorDB* db, float* query_vector, int max_results) {
    SearchResult result;
    result.count = 0;
    
    if (db->size == 0 || max_results <= 0) {
        return result;  // データベースが空または無効なmax_results
    }
    
    // max_resultsの上限を設定
    if (max_results > MAX_SEARCH_RESULTS) {
        max_results = MAX_SEARCH_RESULTS;
    }
    
    // 結果数の上限はデータベースのサイズ
    if (max_results > db->size) {
        max_results = db->size;
    }
    
    // 初期化：最初のmax_results個のエントリで結果を初期化
    for (int i = 0; i < max_results; i++) {
        float distance = euclidean_distance_improved(query_vector, db->entries[i].vector);
        result.ids[i] = db->entries[i].id;
        result.scores[i] = -distance;  // 距離の負値をスコアとして使用（大きいほど良い）
        result.count++;
    }
    
    // 結果をソート（スコア降順）
    sort_search_results(&result);
    
    // 残りのエントリをチェック
    for (int i = max_results; i < db->size; i++) {
        float distance = euclidean_distance_improved(query_vector, db->entries[i].vector);
        float score = -distance;
        
        // 最も低いスコアよりも高いスコアがあれば置き換え
        if (score > result.scores[result.count - 1]) {
            result.ids[result.count - 1] = db->entries[i].id;
            result.scores[result.count - 1] = score;
            
            // 再ソート
            sort_search_results(&result);
        }
    }
    
    return result;
}

// 最も近いベクトルを検索（コサイン類似度、複数結果）
SearchResult search_nearest_cosine_improved(VectorDB* db, float* query_vector, int max_results) {
    SearchResult result;
    result.count = 0;
    
    if (db->size == 0 || max_results <= 0) {
        return result;  // データベースが空または無効なmax_results
    }
    
    // max_resultsの上限を設定
    if (max_results > MAX_SEARCH_RESULTS) {
        max_results = MAX_SEARCH_RESULTS;
    }
    
    // 結果数の上限はデータベースのサイズ
    if (max_results > db->size) {
        max_results = db->size;
    }
    
    // 初期化：最初のmax_results個のエントリで結果を初期化
    for (int i = 0; i < max_results; i++) {
        float similarity = cosine_similarity_improved(query_vector, db->entries[i].vector);
        result.ids[i] = db->entries[i].id;
        result.scores[i] = similarity;
        result.count++;
    }
    
    // 結果をソート（スコア降順）
    sort_search_results(&result);
    
    // 残りのエントリをチェック
    for (int i = max_results; i < db->size; i++) {
        float similarity = cosine_similarity_improved(query_vector, db->entries[i].vector);
        
        // 最も低いスコアよりも高いスコアがあれば置き換え
        if (similarity > result.scores[result.count - 1]) {
            result.ids[result.count - 1] = db->entries[i].id;
            result.scores[result.count - 1] = similarity;
            
            // 再ソート
            sort_search_results(&result);
        }
    }
    
    return result;
}

// ハイブリッド検索（コサイン類似度とキーワードマッチングの組み合わせ）
SearchResult search_hybrid_improved(VectorDB* db, float* query_vector, const char* query_text, int max_results) {
    // まずコサイン類似度で検索
    SearchResult result = search_nearest_cosine_improved(db, query_vector, max_results);
    
    // キーワードマッチングのための重みを設定
    const float keyword_weight = 0.3f;  // キーワードマッチングの重み
    const float vector_weight = 0.7f;   // ベクトル類似度の重み
    
    // クエリテキストが提供されている場合、キーワードマッチングを適用
    if (query_text && strlen(query_text) > 0) {
        // 簡易的なキーワード抽出（スペースで分割）
        char query_copy[1024];
        strncpy(query_copy, query_text, sizeof(query_copy) - 1);
        query_copy[sizeof(query_copy) - 1] = '\0';
        
        char* keywords[20];  // 最大20キーワード
        int keyword_count = 0;
        
        char* token = strtok(query_copy, " ,.");
        while (token && keyword_count < 20) {
            keywords[keyword_count++] = token;
            token = strtok(NULL, " ,.");
        }
        
        // 各結果のスコアを調整
        for (int i = 0; i < result.count; i++) {
            int id = result.ids[i];
            float vector_score = result.scores[i];
            float keyword_score = 0.0f;
            
            // ここでは仮のキーワードスコアを設定
            // 実際の実装では、ドキュメントの内容とキーワードのマッチングを行う
            // この例では、IDが偶数の場合に高いスコアを与える単純な例
            if (id % 2 == 0) {
                keyword_score = 0.8f;
            } else {
                keyword_score = 0.2f;
            }
            
            // 最終スコアを計算
            result.scores[i] = vector_weight * vector_score + keyword_weight * keyword_score;
        }
        
        // 調整後のスコアで再ソート
        sort_search_results(&result);
    }
    
    return result;
}

// ベクトルデータベースのインデックスを構築
void build_vector_db_index(VectorDB* db) {
    if (db->size == 0) {
        return;  // データベースが空
    }
    
    // 既存のインデックスを解放
    if (db->index_map) {
        free(db->index_map);
    }
    
    // 新しいインデックスを割り当て
    db->index_map = (int*)malloc(sizeof(int) * db->size);
    if (!db->index_map) {
        db->use_index = false;
        return;  // メモリ割り当て失敗
    }
    
    // 単純なインデックスマッピング（将来的に改良可能）
    for (int i = 0; i < db->size; i++) {
        db->index_map[i] = i;
    }
    
    db->use_index = true;
}

// ランダムなベクトルを生成
void generate_random_vector_improved(float* vector) {
    for (int i = 0; i < VECTOR_DIM; i++) {
        vector[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;  // -1.0から1.0の範囲
    }
}

// ベクトルを正規化
void normalize_vector_improved(float* vector) {
    float norm = 0.0f;
    for (int i = 0; i < VECTOR_DIM; i++) {
        norm += vector[i] * vector[i];
    }
    norm = sqrtf(norm);
    
    if (norm > 0.0f) {
        for (int i = 0; i < VECTOR_DIM; i++) {
            vector[i] /= norm;
        }
    }
}

// ベクトルの一部を表示
void print_vector_preview_improved(float* vector, int preview_size) {
    printf("[");
    for (int i = 0; i < preview_size && i < VECTOR_DIM; i++) {
        printf("%.4f", vector[i]);
        if (i < preview_size - 1 && i < VECTOR_DIM - 1) {
            printf(", ");
        }
    }
    if (VECTOR_DIM > preview_size) {
        printf(", ...");
    }
    printf("]");
}