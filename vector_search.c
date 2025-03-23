#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <float.h>

#define MAX_VECTORS 2000   // 最大ベクトル数
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
void init_vector_db(VectorDB* db) {
    db->size = 0;
}

// ベクトルをデータベースに追加
int add_vector(VectorDB* db, float* vector, int id) {
    if (db->size >= MAX_VECTORS) {
        return 0;  // データベースが満杯
    }
    
    // ベクトルをコピー
    for (int i = 0; i < VECTOR_DIM; i++) {
        db->entries[db->size].vector[i] = vector[i];
    }
    db->entries[db->size].id = id;
    db->size++;
    
    return 1;  // 成功
}

// ユークリッド距離の計算
float euclidean_distance(float* v1, float* v2) {
    float sum = 0.0f;
    for (int i = 0; i < VECTOR_DIM; i++) {
        float diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    return sqrtf(sum);
}

// コサイン類似度の計算
float cosine_similarity(float* v1, float* v2) {
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

// 最も近いベクトルを検索（ユークリッド距離）
int search_nearest_euclidean(VectorDB* db, float* query_vector) {
    if (db->size == 0) {
        return -1;  // データベースが空
    }
    
    float min_distance = FLT_MAX;
    int nearest_id = -1;
    
    for (int i = 0; i < db->size; i++) {
        float distance = euclidean_distance(query_vector, db->entries[i].vector);
        if (distance < min_distance) {
            min_distance = distance;
            nearest_id = db->entries[i].id;
        }
    }
    
    return nearest_id;
}

// 最も近いベクトルを検索（コサイン類似度）
int search_nearest_cosine(VectorDB* db, float* query_vector) {
    if (db->size == 0) {
        return -1;  // データベースが空
    }
    
    float max_similarity = -1.0f;
    int nearest_id = -1;
    
    for (int i = 0; i < db->size; i++) {
        float similarity = cosine_similarity(query_vector, db->entries[i].vector);
        if (similarity > max_similarity) {
            max_similarity = similarity;
            nearest_id = db->entries[i].id;
        }
    }
    
    return nearest_id;
}

// ランダムなベクトルを生成
void generate_random_vector(float* vector) {
    for (int i = 0; i < VECTOR_DIM; i++) {
        vector[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;  // -1.0から1.0の範囲
    }
}

// ベクトルを正規化
void normalize_vector(float* vector) {
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
void print_vector_preview(float* vector, int preview_size) {
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

int main() {
    // 乱数の初期化
    srand((unsigned int)time(NULL));
    
    // ベクトルデータベースの初期化
    VectorDB db;
    init_vector_db(&db);
    
    // テスト用のベクトルを生成してデータベースに追加
    printf("ベクトルデータベースの作成中...\n");
    int num_vectors = 1000;  // 10000から1000に減らす
    for (int i = 0; i < num_vectors; i++) {
        float vector[VECTOR_DIM];
        generate_random_vector(vector);
        normalize_vector(vector);  // 正規化（コサイン類似度用）
        add_vector(&db, vector, i);
        
        if (i % 100 == 0) {
            printf("  %d / %d ベクトルを追加しました\n", i, num_vectors);
        }
    }
    printf("%d ベクトルをデータベースに追加しました\n", db.size);
    
    // クエリベクトルの生成
    float query_vector[VECTOR_DIM];
    generate_random_vector(query_vector);
    normalize_vector(query_vector);  // 正規化（コサイン類似度用）
    
    printf("\nクエリベクトル: ");
    print_vector_preview(query_vector, 5);
    printf("\n");
    
    // 検索の実行（ユークリッド距離）
    clock_t start_time = clock();
    int nearest_id_euclidean = search_nearest_euclidean(&db, query_vector);
    clock_t end_time = clock();
    double search_time_euclidean = (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000.0;
    
    // 検索の実行（コサイン類似度）
    start_time = clock();
    int nearest_id_cosine = search_nearest_cosine(&db, query_vector);
    end_time = clock();
    double search_time_cosine = (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000.0;
    
    // 結果の表示
    printf("\n検索結果（ユークリッド距離）:\n");
    printf("  最も近いベクトルのID: %d\n", nearest_id_euclidean);
    printf("  検索時間: %.2f ミリ秒\n", search_time_euclidean);
    
    printf("\n検索結果（コサイン類似度）:\n");
    printf("  最も近いベクトルのID: %d\n", nearest_id_cosine);
    printf("  検索時間: %.2f ミリ秒\n", search_time_cosine);
    
    // 最も近いベクトルの表示
    if (nearest_id_euclidean >= 0) {
        printf("\n最も近いベクトル（ユークリッド距離）: ");
        for (int i = 0; i < db.size; i++) {
            if (db.entries[i].id == nearest_id_euclidean) {
                print_vector_preview(db.entries[i].vector, 5);
                printf("\n");
                printf("  距離: %.4f\n", euclidean_distance(query_vector, db.entries[i].vector));
                break;
            }
        }
    }
    
    if (nearest_id_cosine >= 0) {
        printf("\n最も近いベクトル（コサイン類似度）: ");
        for (int i = 0; i < db.size; i++) {
            if (db.entries[i].id == nearest_id_cosine) {
                print_vector_preview(db.entries[i].vector, 5);
                printf("\n");
                printf("  類似度: %.4f\n", cosine_similarity(query_vector, db.entries[i].vector));
                break;
            }
        }
    }
    
    return 0;
}