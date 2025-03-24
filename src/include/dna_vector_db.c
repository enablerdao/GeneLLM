#include "dna_vector_db.h"

// DNAベクトルデータベースの初期化
void init_dna_vector_db(DNAVectorDB* db) {
    if (!db) return;
    
    db->size = 0;
    memset(db->entries, 0, sizeof(DNAVectorEntry) * MAX_DNA_VECTORS);
}

// DNAベクトルデータベースのサイズを取得
int get_dna_vector_db_size(DNAVectorDB* db) {
    if (!db) return 0;
    return db->size;
}

// DNAコードをベクトル化してデータベースに追加
int add_dna_vector(DNAVectorDB* db, const char* dna_code, int id) {
    if (!db || !dna_code || db->size >= MAX_DNA_VECTORS) return 0;
    
    // DNAコードからベクトルを生成
    float vector[64];
    generate_dna_vector(dna_code, vector);
    
    // エントリを追加
    DNAVectorEntry* entry = &db->entries[db->size];
    strncpy(entry->dna_code, dna_code, DNA_CODE_MAX_LEN - 1);
    entry->dna_code[DNA_CODE_MAX_LEN - 1] = '\0';
    memcpy(entry->vector, vector, sizeof(float) * 64);
    entry->id = id;
    
    db->size++;
    return 1;
}

// DNAコードからベクトルを生成
void generate_dna_vector(const char* dna_code, float* vector) {
    if (!dna_code || !vector) return;
    
    // ベクトルを初期化
    for (int i = 0; i < 64; i++) {
        vector[i] = 0.0f;
    }
    
    // DNAコードの構造を解析
    char entity[32] = {0};
    char concept[32] = {0};
    char result[32] = {0};
    char attribute[32] = {0};
    char time[32] = {0};
    char location[32] = {0};
    char manner[32] = {0};
    char quantity[32] = {0};
    
    parse_dna_code(dna_code, entity, concept, result, attribute, time, location, manner, quantity);
    
    // 各要素の値を使用してベクトルを生成
    // 主語（Entity）: 0-7
    if (entity[0] == 'E') {
        int entity_id = atoi(entity + 1);
        for (int i = 0; i < 8; i++) {
            vector[i] = (entity_id % (i + 1 + 8)) / 8.0f;
        }
    }
    
    // 動詞（Concept）: 8-15
    if (concept[0] == 'C') {
        int concept_id = atoi(concept + 1);
        for (int i = 0; i < 8; i++) {
            vector[i + 8] = (concept_id % (i + 1 + 8)) / 8.0f;
        }
    }
    
    // 目的語（Result）: 16-23
    if (result[0] == 'R') {
        int result_id = atoi(result + 1);
        for (int i = 0; i < 8; i++) {
            vector[i + 16] = (result_id % (i + 1 + 8)) / 8.0f;
        }
    }
    
    // 属性（Attribute）: 24-31
    if (attribute[0] == 'A') {
        int attribute_id = atoi(attribute + 1);
        for (int i = 0; i < 8; i++) {
            vector[i + 24] = (attribute_id % (i + 1 + 8)) / 8.0f;
        }
    }
    
    // 時間（Time）: 32-39
    if (time[0] == 'T') {
        int time_id = atoi(time + 1);
        for (int i = 0; i < 8; i++) {
            vector[i + 32] = (time_id % (i + 1 + 8)) / 8.0f;
        }
    }
    
    // 場所（Location）: 40-47
    if (location[0] == 'L') {
        int location_id = atoi(location + 1);
        for (int i = 0; i < 8; i++) {
            vector[i + 40] = (location_id % (i + 1 + 8)) / 8.0f;
        }
    }
    
    // 様態（Manner）: 48-55
    if (manner[0] == 'M') {
        int manner_id = atoi(manner + 1);
        for (int i = 0; i < 8; i++) {
            vector[i + 48] = (manner_id % (i + 1 + 8)) / 8.0f;
        }
    }
    
    // 数量（Quantity）: 56-63
    if (quantity[0] == 'Q') {
        int quantity_id = atoi(quantity + 1);
        for (int i = 0; i < 8; i++) {
            vector[i + 56] = (quantity_id % (i + 1 + 8)) / 8.0f;
        }
    }
    
    // ベクトルを正規化
    float norm = 0.0f;
    for (int i = 0; i < 64; i++) {
        norm += vector[i] * vector[i];
    }
    norm = sqrt(norm);
    
    if (norm > 0.0f) {
        for (int i = 0; i < 64; i++) {
            vector[i] /= norm;
        }
    }
}

// 最も近いDNAコードを検索（ユークリッド距離）
int search_nearest_dna_euclidean(DNAVectorDB* db, const char* query_dna_code) {
    if (!db || !query_dna_code || db->size == 0) return -1;
    
    // クエリDNAコードをベクトル化
    float query_vector[64];
    generate_dna_vector(query_dna_code, query_vector);
    
    // 最も近いベクトルを検索
    int nearest_id = -1;
    float min_distance = INFINITY;
    
    for (int i = 0; i < db->size; i++) {
        float distance = 0.0f;
        for (int j = 0; j < 64; j++) {
            float diff = query_vector[j] - db->entries[i].vector[j];
            distance += diff * diff;
        }
        distance = sqrt(distance);
        
        if (distance < min_distance) {
            min_distance = distance;
            nearest_id = db->entries[i].id;
        }
    }
    
    return nearest_id;
}

// 最も近いDNAコードを検索（コサイン類似度）
int search_nearest_dna_cosine(DNAVectorDB* db, const char* query_dna_code) {
    if (!db || !query_dna_code || db->size == 0) return -1;
    
    // クエリDNAコードをベクトル化
    float query_vector[64];
    generate_dna_vector(query_dna_code, query_vector);
    
    // 最も近いベクトルを検索
    int nearest_id = -1;
    float max_similarity = -1.0f;
    
    for (int i = 0; i < db->size; i++) {
        float dot_product = 0.0f;
        float norm_query = 0.0f;
        float norm_entry = 0.0f;
        
        for (int j = 0; j < 64; j++) {
            dot_product += query_vector[j] * db->entries[i].vector[j];
            norm_query += query_vector[j] * query_vector[j];
            norm_entry += db->entries[i].vector[j] * db->entries[i].vector[j];
        }
        
        norm_query = sqrt(norm_query);
        norm_entry = sqrt(norm_entry);
        
        float similarity = 0.0f;
        if (norm_query > 0.0f && norm_entry > 0.0f) {
            similarity = dot_product / (norm_query * norm_entry);
        }
        
        if (similarity > max_similarity) {
            max_similarity = similarity;
            nearest_id = db->entries[i].id;
        }
    }
    
    return nearest_id;
}

// DNAベクトルデータベースをファイルから読み込む
int load_dna_vector_db(DNAVectorDB* db, const char* filename) {
    if (!db || !filename) return 0;
    
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;
    
    // データベースを初期化
    init_dna_vector_db(db);
    
    // エントリ数を読み込む
    int size;
    if (fread(&size, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    
    // 各エントリを読み込む
    for (int i = 0; i < size && i < MAX_DNA_VECTORS; i++) {
        DNAVectorEntry entry;
        if (fread(&entry, sizeof(DNAVectorEntry), 1, fp) != 1) {
            fclose(fp);
            return i;
        }
        db->entries[i] = entry;
        db->size++;
    }
    
    fclose(fp);
    return db->size;
}

// DNAベクトルデータベースをファイルに保存
int save_dna_vector_db(DNAVectorDB* db, const char* filename) {
    if (!db || !filename) return 0;
    
    FILE* fp = fopen(filename, "wb");
    if (!fp) return 0;
    
    // エントリ数を書き込む
    if (fwrite(&db->size, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    
    // 各エントリを書き込む
    for (int i = 0; i < db->size; i++) {
        if (fwrite(&db->entries[i], sizeof(DNAVectorEntry), 1, fp) != 1) {
            fclose(fp);
            return 0;
        }
    }
    
    fclose(fp);
    return 1;
}

// DNAコードの類似度を計算
float calculate_dna_similarity(const char* dna_code1, const char* dna_code2) {
    if (!dna_code1 || !dna_code2) return 0.0f;
    
    // DNAコードをベクトル化
    float vector1[64];
    float vector2[64];
    generate_dna_vector(dna_code1, vector1);
    generate_dna_vector(dna_code2, vector2);
    
    // コサイン類似度を計算
    float dot_product = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (int i = 0; i < 64; i++) {
        dot_product += vector1[i] * vector2[i];
        norm1 += vector1[i] * vector1[i];
        norm2 += vector2[i] * vector2[i];
    }
    
    norm1 = sqrt(norm1);
    norm2 = sqrt(norm2);
    
    if (norm1 > 0.0f && norm2 > 0.0f) {
        return dot_product / (norm1 * norm2);
    }
    
    return 0.0f;
}

// DNAコードの構造を解析
void parse_dna_code(const char* dna_code, char* entity, char* concept, char* result, 
                   char* attribute, char* time, char* location, char* manner, char* quantity) {
    if (!dna_code) return;
    
    // 初期化
    if (entity) entity[0] = '\0';
    if (concept) concept[0] = '\0';
    if (result) result[0] = '\0';
    if (attribute) attribute[0] = '\0';
    if (time) time[0] = '\0';
    if (location) location[0] = '\0';
    if (manner) manner[0] = '\0';
    if (quantity) quantity[0] = '\0';
    
    int i = 0;
    while (dna_code[i] != '\0') {
        char type = dna_code[i++];
        
        // 数字部分を取得
        int start = i;
        while (dna_code[i] >= '0' && dna_code[i] <= '9') {
            i++;
        }
        
        // 対応する要素に値を設定
        switch (type) {
            case 'E':
                if (entity) {
                    entity[0] = 'E';
                    strncpy(entity + 1, dna_code + start, i - start);
                    entity[i - start + 1] = '\0';
                }
                break;
            case 'C':
                if (concept) {
                    concept[0] = 'C';
                    strncpy(concept + 1, dna_code + start, i - start);
                    concept[i - start + 1] = '\0';
                }
                break;
            case 'R':
                if (result) {
                    result[0] = 'R';
                    strncpy(result + 1, dna_code + start, i - start);
                    result[i - start + 1] = '\0';
                }
                break;
            case 'A':
                if (attribute) {
                    attribute[0] = 'A';
                    strncpy(attribute + 1, dna_code + start, i - start);
                    attribute[i - start + 1] = '\0';
                }
                break;
            case 'T':
                if (time) {
                    time[0] = 'T';
                    strncpy(time + 1, dna_code + start, i - start);
                    time[i - start + 1] = '\0';
                }
                break;
            case 'L':
                if (location) {
                    location[0] = 'L';
                    strncpy(location + 1, dna_code + start, i - start);
                    location[i - start + 1] = '\0';
                }
                break;
            case 'M':
                if (manner) {
                    manner[0] = 'M';
                    strncpy(manner + 1, dna_code + start, i - start);
                    manner[i - start + 1] = '\0';
                }
                break;
            case 'Q':
                if (quantity) {
                    quantity[0] = 'Q';
                    strncpy(quantity + 1, dna_code + start, i - start);
                    quantity[i - start + 1] = '\0';
                }
                break;
        }
    }
}