# ベクトル検索エンジン詳細

`src/vector_search/vector_search.c` はテキストのベクトル表現と類似度検索を担当するコンポーネントです。意味的な類似性に基づく検索を可能にします。

## データ構造

### ベクトルデータベース

```c
// ベクトルデータベース構造体
typedef struct {
    char** ids;           // ベクトルのID
    float** vectors;      // ベクトルデータ
    int count;            // 現在のベクトル数
    int capacity;         // 最大容量
    int vector_dim;       // ベクトルの次元数
} VectorDB;

// グローバルベクトルデータベース
VectorDB global_vector_db;
```

ベクトルデータベースは以下の情報を管理します：
- ベクトルのID（文字列）
- ベクトルデータ（浮動小数点数の配列）
- ベクトル数と最大容量
- ベクトルの次元数

### 検索結果

```c
// 検索結果構造体
typedef struct {
    int index;      // ベクトルのインデックス
    float score;    // 類似度スコア
    char* id;       // ベクトルのID
} SearchResult;
```

## 主要機能

### 1. ベクトルデータベースの初期化

```c
void init_vector_db(VectorDB* db, int capacity, int vector_dim) {
    db->capacity = capacity;
    db->count = 0;
    db->vector_dim = vector_dim;
    
    // メモリ割り当て
    db->ids = (char**)malloc(sizeof(char*) * capacity);
    db->vectors = (float**)malloc(sizeof(float*) * capacity);
    
    for (int i = 0; i < capacity; i++) {
        db->ids[i] = NULL;
        db->vectors[i] = NULL;
    }
}
```

### 2. ベクトルの追加

```c
bool add_vector_to_db(VectorDB* db, const char* id, const float* vector) {
    // IDが既に存在するか確認
    for (int i = 0; i < db->count; i++) {
        if (strcmp(db->ids[i], id) == 0) {
            // 既存のベクトルを更新
            memcpy(db->vectors[i], vector, sizeof(float) * db->vector_dim);
            return true;
        }
    }
    
    // 容量が足りない場合は拡張
    if (db->count >= db->capacity) {
        int new_capacity = db->capacity * 2;
        
        db->ids = (char**)realloc(db->ids, sizeof(char*) * new_capacity);
        db->vectors = (float**)realloc(db->vectors, sizeof(float*) * new_capacity);
        
        for (int i = db->capacity; i < new_capacity; i++) {
            db->ids[i] = NULL;
            db->vectors[i] = NULL;
        }
        
        db->capacity = new_capacity;
    }
    
    // 新しいベクトルを追加
    db->ids[db->count] = strdup(id);
    db->vectors[db->count] = (float*)malloc(sizeof(float) * db->vector_dim);
    memcpy(db->vectors[db->count], vector, sizeof(float) * db->vector_dim);
    
    db->count++;
    return true;
}
```

### 3. 類似度計算

#### コサイン類似度

```c
float cosine_similarity(const float* vec1, const float* vec2, int dim) {
    float dot_product = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (int i = 0; i < dim; i++) {
        dot_product += vec1[i] * vec2[i];
        norm1 += vec1[i] * vec1[i];
        norm2 += vec2[i] * vec2[i];
    }
    
    norm1 = sqrt(norm1);
    norm2 = sqrt(norm2);
    
    if (norm1 < 1e-6 || norm2 < 1e-6) {
        return 0.0f;
    }
    
    return dot_product / (norm1 * norm2);
}
```

#### ユークリッド距離

```c
float euclidean_distance(const float* vec1, const float* vec2, int dim) {
    float sum = 0.0f;
    
    for (int i = 0; i < dim; i++) {
        float diff = vec1[i] - vec2[i];
        sum += diff * diff;
    }
    
    return sqrt(sum);
}
```

### 4. 最近傍検索

```c
// 最も類似したベクトルを検索（コサイン類似度）
SearchResult search_nearest_vector(VectorDB* db, const float* query_vector) {
    SearchResult result;
    result.index = -1;
    result.score = -1.0f;
    
    if (db->count == 0) {
        return result;
    }
    
    float max_similarity = -1.0f;
    int max_index = -1;
    
    // 全ベクトルとの類似度を計算
    for (int i = 0; i < db->count; i++) {
        float similarity = cosine_similarity(query_vector, db->vectors[i], db->vector_dim);
        
        if (similarity > max_similarity) {
            max_similarity = similarity;
            max_index = i;
        }
    }
    
    result.index = max_index;
    result.score = max_similarity;
    result.id = (max_index >= 0) ? db->ids[max_index] : NULL;
    
    return result;
}
```

### 5. Top-K検索

```c
// 上位K個の類似ベクトルを検索
SearchResult* search_top_k_vectors(VectorDB* db, const float* query_vector, int k, int* result_count) {
    if (db->count == 0 || k <= 0) {
        *result_count = 0;
        return NULL;
    }
    
    // 実際に返す結果数を決定（k と db->count の小さい方）
    int actual_k = (k < db->count) ? k : db->count;
    *result_count = actual_k;
    
    // 結果配列を確保
    SearchResult* results = (SearchResult*)malloc(sizeof(SearchResult) * actual_k);
    
    // 全ベクトルの類似度を計算
    typedef struct {
        int index;
        float score;
    } ScoredIndex;
    
    ScoredIndex* scores = (ScoredIndex*)malloc(sizeof(ScoredIndex) * db->count);
    
    for (int i = 0; i < db->count; i++) {
        scores[i].index = i;
        scores[i].score = cosine_similarity(query_vector, db->vectors[i], db->vector_dim);
    }
    
    // スコアでソート（降順）
    qsort(scores, db->count, sizeof(ScoredIndex), compare_scores_desc);
    
    // 上位K個の結果を取得
    for (int i = 0; i < actual_k; i++) {
        results[i].index = scores[i].index;
        results[i].score = scores[i].score;
        results[i].id = db->ids[scores[i].index];
    }
    
    free(scores);
    return results;
}

// スコア比較関数（降順）
static int compare_scores_desc(const void* a, const void* b) {
    const ScoredIndex* sa = (const ScoredIndex*)a;
    const ScoredIndex* sb = (const ScoredIndex*)b;
    
    if (sa->score > sb->score) return -1;
    if (sa->score < sb->score) return 1;
    return 0;
}
```

### 6. テキストからベクトルへの変換

```c
// テキストをベクトルに変換
void text_to_vector(const char* text, float* vector) {
    // ベクトルを初期化
    for (int i = 0; i < VECTOR_DIM; i++) {
        vector[i] = 0.0f;
    }
    
    // テキストを形態素解析
    MeCab::Tagger* tagger = MeCab::createTagger("");
    const MeCab::Node* node = tagger->parseToNode(text);
    
    int word_count = 0;
    
    // 各単語のベクトルを合計
    while (node) {
        if (node->stat == MECAB_NOR_NODE || node->stat == MECAB_UNK_NODE) {
            char word[256];
            strncpy(word, node->surface, node->length);
            word[node->length] = '\0';
            
            // 単語ベクトルを取得
            float word_vector[VECTOR_DIM];
            if (get_word_vector(word, word_vector)) {
                // ベクトルを加算
                for (int i = 0; i < VECTOR_DIM; i++) {
                    vector[i] += word_vector[i];
                }
                word_count++;
            }
        }
        node = node->next;
    }
    
    delete tagger;
    
    // 単語数で正規化
    if (word_count > 0) {
        for (int i = 0; i < VECTOR_DIM; i++) {
            vector[i] /= word_count;
        }
    }
    
    // ベクトルを正規化
    normalize_vector(vector);
}
```

### 7. ベクトルの正規化

```c
// ベクトルを正規化（単位ベクトル化）
void normalize_vector(float* vector) {
    float norm = 0.0f;
    
    // ノルムを計算
    for (int i = 0; i < VECTOR_DIM; i++) {
        norm += vector[i] * vector[i];
    }
    
    norm = sqrt(norm);
    
    // ゼロベクトルの場合は正規化しない
    if (norm < 1e-6) {
        return;
    }
    
    // 正規化
    for (int i = 0; i < VECTOR_DIM; i++) {
        vector[i] /= norm;
    }
}
```

## ベクトルデータの永続化

### 保存

```c
bool save_word_vectors(const char* filename, VectorDB* db) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return false;
    }
    
    // ヘッダ情報を書き込む
    fwrite(&db->count, sizeof(int), 1, file);
    fwrite(&db->vector_dim, sizeof(int), 1, file);
    
    // 各ベクトルを書き込む
    for (int i = 0; i < db->count; i++) {
        // IDの長さを書き込む
        int id_len = strlen(db->ids[i]);
        fwrite(&id_len, sizeof(int), 1, file);
        
        // IDを書き込む
        fwrite(db->ids[i], sizeof(char), id_len, file);
        
        // ベクトルを書き込む
        fwrite(db->vectors[i], sizeof(float), db->vector_dim, file);
    }
    
    fclose(file);
    return true;
}
```

### 読み込み

```c
bool load_word_vectors(const char* filename, VectorDB* db) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return false;
    }
    
    // 既存のデータをクリア
    for (int i = 0; i < db->count; i++) {
        free(db->ids[i]);
        free(db->vectors[i]);
    }
    
    // ヘッダ情報を読み込む
    int count, vector_dim;
    fread(&count, sizeof(int), 1, file);
    fread(&vector_dim, sizeof(int), 1, file);
    
    // データベースを初期化
    init_vector_db(db, count, vector_dim);
    
    // 各ベクトルを読み込む
    for (int i = 0; i < count; i++) {
        // IDの長さを読み込む
        int id_len;
        fread(&id_len, sizeof(int), 1, file);
        
        // IDを読み込む
        char* id = (char*)malloc(id_len + 1);
        fread(id, sizeof(char), id_len, file);
        id[id_len] = '\0';
        
        // ベクトルを読み込む
        float* vector = (float*)malloc(sizeof(float) * vector_dim);
        fread(vector, sizeof(float), vector_dim, file);
        
        // データベースに追加
        db->ids[i] = id;
        db->vectors[i] = vector;
        db->count++;
    }
    
    fclose(file);
    return true;
}
```

## 拡張ポイント

ベクトル検索エンジンは以下の点で拡張可能です：

1. 高度なインデックス構造の実装（KD-treeやHNSWなど）
2. 並列処理による検索の高速化
3. 次元削減技術の導入（PCAやt-SNEなど）
4. 複数の類似度指標のサポート
5. インクリメンタル学習機能の追加
6. 事前学習済みの単語埋め込みモデルの統合