#include "learning_module.h"

#define INITIAL_CAPACITY 100
#define SIMILARITY_THRESHOLD 0.8

// 学習データベースの初期化
LearningDB* learning_db_init(const char* filename) {
    LearningDB* db = (LearningDB*)malloc(sizeof(LearningDB));
    if (!db) {
        fprintf(stderr, "メモリ割り当てエラー: 学習データベースの初期化に失敗しました\n");
        return NULL;
    }
    
    db->entries = (LearningEntry*)malloc(sizeof(LearningEntry) * INITIAL_CAPACITY);
    if (!db->entries) {
        fprintf(stderr, "メモリ割り当てエラー: 学習エントリの初期化に失敗しました\n");
        free(db);
        return NULL;
    }
    
    db->count = 0;
    db->capacity = INITIAL_CAPACITY;
    strncpy(db->filename, filename, sizeof(db->filename) - 1);
    db->filename[sizeof(db->filename) - 1] = '\0';
    
    // 既存のデータがあれば読み込む
    learning_db_load(db);
    
    return db;
}

// 学習データベースの解放
void learning_db_free(LearningDB* db) {
    if (db) {
        if (db->entries) {
            free(db->entries);
        }
        free(db);
    }
}

// 学習データの追加
bool learning_db_add(LearningDB* db, const char* question, const char* answer, float confidence) {
    if (!db || !question || !answer) {
        return false;
    }
    
    // 既存の質問と類似していないか確認
    for (int i = 0; i < db->count; i++) {
        float similarity = calculate_question_similarity(db->entries[i].question, question);
        if (similarity > SIMILARITY_THRESHOLD) {
            // 既存の質問が類似している場合は更新
            strncpy(db->entries[i].answer, answer, sizeof(db->entries[i].answer) - 1);
            db->entries[i].answer[sizeof(db->entries[i].answer) - 1] = '\0';
            db->entries[i].confidence = confidence;
            return learning_db_save(db);
        }
    }
    
    // 容量が足りない場合は拡張
    if (db->count >= db->capacity) {
        int new_capacity = db->capacity * 2;
        LearningEntry* new_entries = (LearningEntry*)realloc(db->entries, sizeof(LearningEntry) * new_capacity);
        if (!new_entries) {
            fprintf(stderr, "メモリ割り当てエラー: 学習データベースの拡張に失敗しました\n");
            return false;
        }
        db->entries = new_entries;
        db->capacity = new_capacity;
    }
    
    // 新しいエントリを追加
    strncpy(db->entries[db->count].question, question, sizeof(db->entries[db->count].question) - 1);
    db->entries[db->count].question[sizeof(db->entries[db->count].question) - 1] = '\0';
    
    strncpy(db->entries[db->count].answer, answer, sizeof(db->entries[db->count].answer) - 1);
    db->entries[db->count].answer[sizeof(db->entries[db->count].answer) - 1] = '\0';
    
    db->entries[db->count].confidence = confidence;
    db->count++;
    
    // データベースを保存
    return learning_db_save(db);
}

// 学習データの検索
bool learning_db_find(LearningDB* db, const char* question, char* answer, float* confidence) {
    if (!db || !question || !answer || !confidence) {
        return false;
    }
    
    float max_similarity = 0.0f;
    int best_match = -1;
    
    // 最も類似度の高い質問を検索
    for (int i = 0; i < db->count; i++) {
        float similarity = calculate_question_similarity(db->entries[i].question, question);
        if (similarity > max_similarity) {
            max_similarity = similarity;
            best_match = i;
        }
    }
    
    // 類似度が閾値を超えていれば回答を返す
    if (max_similarity > SIMILARITY_THRESHOLD && best_match >= 0) {
        strncpy(answer, db->entries[best_match].answer, 2047);
        answer[2047] = '\0';
        *confidence = db->entries[best_match].confidence;
        return true;
    }
    
    return false;
}

// 学習データの保存
bool learning_db_save(LearningDB* db) {
    if (!db) {
        return false;
    }
    
    FILE* file = fopen(db->filename, "w");
    if (!file) {
        fprintf(stderr, "ファイルオープンエラー: %s\n", db->filename);
        return false;
    }
    
    // ヘッダー情報を書き込む
    fprintf(file, "%d\n", db->count);
    
    // 各エントリを書き込む
    for (int i = 0; i < db->count; i++) {
        // 質問と回答をエスケープして保存
        fprintf(file, "Q: %s\n", db->entries[i].question);
        fprintf(file, "A: %s\n", db->entries[i].answer);
        fprintf(file, "C: %.4f\n", db->entries[i].confidence);
    }
    
    fclose(file);
    return true;
}

// 学習データの読み込み
bool learning_db_load(LearningDB* db) {
    if (!db) {
        return false;
    }
    
    FILE* file = fopen(db->filename, "r");
    if (!file) {
        // ファイルが存在しない場合は新規作成とみなす
        return true;
    }
    
    char line[2048];
    
    // ヘッダー情報を読み込む
    if (fgets(line, sizeof(line), file)) {
        int count = atoi(line);
        
        // 容量が足りない場合は拡張
        if (count > db->capacity) {
            LearningEntry* new_entries = (LearningEntry*)realloc(db->entries, sizeof(LearningEntry) * count);
            if (!new_entries) {
                fprintf(stderr, "メモリ割り当てエラー: 学習データベースの拡張に失敗しました\n");
                fclose(file);
                return false;
            }
            db->entries = new_entries;
            db->capacity = count;
        }
        
        // 各エントリを読み込む
        db->count = 0;
        while (db->count < count) {
            char question[1024] = {0};
            char answer[2048] = {0};
            float confidence = 0.0f;
            
            // 質問を読み込む
            if (fgets(line, sizeof(line), file) && strncmp(line, "Q: ", 3) == 0) {
                strncpy(question, line + 3, sizeof(question) - 1);
                // 改行を削除
                size_t len = strlen(question);
                if (len > 0 && question[len-1] == '\n') {
                    question[len-1] = '\0';
                }
            } else {
                break;
            }
            
            // 回答を読み込む
            if (fgets(line, sizeof(line), file) && strncmp(line, "A: ", 3) == 0) {
                strncpy(answer, line + 3, sizeof(answer) - 1);
                // 改行を削除
                size_t len = strlen(answer);
                if (len > 0 && answer[len-1] == '\n') {
                    answer[len-1] = '\0';
                }
            } else {
                break;
            }
            
            // 確信度を読み込む
            if (fgets(line, sizeof(line), file) && strncmp(line, "C: ", 3) == 0) {
                confidence = atof(line + 3);
            } else {
                break;
            }
            
            // エントリを追加
            strncpy(db->entries[db->count].question, question, sizeof(db->entries[db->count].question) - 1);
            db->entries[db->count].question[sizeof(db->entries[db->count].question) - 1] = '\0';
            
            strncpy(db->entries[db->count].answer, answer, sizeof(db->entries[db->count].answer) - 1);
            db->entries[db->count].answer[sizeof(db->entries[db->count].answer) - 1] = '\0';
            
            db->entries[db->count].confidence = confidence;
            db->count++;
        }
    }
    
    fclose(file);
    return true;
}

// 質問の類似度計算（単純な単語の一致率）
float calculate_question_similarity(const char* q1, const char* q2) {
    if (!q1 || !q2) {
        return 0.0f;
    }
    
    // 単語の一致数をカウント
    char q1_copy[1024];
    char q2_copy[1024];
    strncpy(q1_copy, q1, sizeof(q1_copy) - 1);
    q1_copy[sizeof(q1_copy) - 1] = '\0';
    strncpy(q2_copy, q2, sizeof(q2_copy) - 1);
    q2_copy[sizeof(q2_copy) - 1] = '\0';
    
    // 単語に分割
    char* q1_words[100] = {0};
    char* q2_words[100] = {0};
    int q1_word_count = 0;
    int q2_word_count = 0;
    
    char* token = strtok(q1_copy, " 　、。？！,.?!");
    while (token && q1_word_count < 100) {
        q1_words[q1_word_count++] = token;
        token = strtok(NULL, " 　、。？！,.?!");
    }
    
    token = strtok(q2_copy, " 　、。？！,.?!");
    while (token && q2_word_count < 100) {
        q2_words[q2_word_count++] = token;
        token = strtok(NULL, " 　、。？！,.?!");
    }
    
    // 一致する単語をカウント
    int match_count = 0;
    for (int i = 0; i < q1_word_count; i++) {
        for (int j = 0; j < q2_word_count; j++) {
            if (strcmp(q1_words[i], q2_words[j]) == 0) {
                match_count++;
                break;
            }
        }
    }
    
    // 類似度を計算（Jaccard係数の簡易版）
    int total_unique_words = q1_word_count + q2_word_count - match_count;
    if (total_unique_words == 0) {
        return 0.0f;
    }
    
    return (float)match_count / total_unique_words;
}