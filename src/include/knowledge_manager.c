#include "knowledge_manager.h"
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <math.h>

#define KB_INITIAL_CAPACITY 50
#define MAX_LINE_LENGTH 4096

// 知識ベースの初期化
KnowledgeBase* knowledge_base_init(const char* base_dir) {
    KnowledgeBase* kb = (KnowledgeBase*)malloc(sizeof(KnowledgeBase));
    if (!kb) {
        fprintf(stderr, "メモリ割り当てエラー: 知識ベースの初期化に失敗しました\n");
        return NULL;
    }
    
    kb->documents = (KnowledgeDocument*)malloc(sizeof(KnowledgeDocument) * KB_INITIAL_CAPACITY);
    if (!kb->documents) {
        fprintf(stderr, "メモリ割り当てエラー: 知識ドキュメントの初期化に失敗しました\n");
        free(kb);
        return NULL;
    }
    
    kb->count = 0;
    kb->capacity = KB_INITIAL_CAPACITY;
    strncpy(kb->base_dir, base_dir, sizeof(kb->base_dir) - 1);
    kb->base_dir[sizeof(kb->base_dir) - 1] = '\0';
    
    // ベースディレクトリが存在しない場合は作成
    struct stat st = {0};
    if (stat(base_dir, &st) == -1) {
        mkdir(base_dir, 0700);
    }
    
    // 既存のドキュメントを読み込む
    knowledge_base_load(kb);
    
    return kb;
}

// 知識ベースの解放
void knowledge_base_free(KnowledgeBase* kb) {
    if (kb) {
        if (kb->documents) {
            free(kb->documents);
        }
        free(kb);
    }
}

// 知識ドキュメントの追加
bool knowledge_base_add_document(KnowledgeBase* kb, const char* title, const char* content, 
                                const char* category, const char** tags, int tag_count) {
    if (!kb || !title || !content) {
        return false;
    }
    
    // 既存のドキュメントを検索
    for (int i = 0; i < kb->count; i++) {
        if (strcmp(kb->documents[i].title, title) == 0) {
            // 既存のドキュメントを更新
            strncpy(kb->documents[i].content, content, sizeof(kb->documents[i].content) - 1);
            kb->documents[i].content[sizeof(kb->documents[i].content) - 1] = '\0';
            
            strncpy(kb->documents[i].category, category, sizeof(kb->documents[i].category) - 1);
            kb->documents[i].category[sizeof(kb->documents[i].category) - 1] = '\0';
            
            kb->documents[i].tag_count = 0;
            for (int j = 0; j < tag_count && j < 10; j++) {
                strncpy(kb->documents[i].tags[j], tags[j], sizeof(kb->documents[i].tags[j]) - 1);
                kb->documents[i].tags[j][sizeof(kb->documents[i].tags[j]) - 1] = '\0';
                kb->documents[i].tag_count++;
            }
            
            kb->documents[i].updated_at = time(NULL);
            
            // ドキュメントをファイルに保存
            knowledge_base_save_document(kb, &kb->documents[i]);
            
            return true;
        }
    }
    
    // 容量が足りない場合は拡張
    if (kb->count >= kb->capacity) {
        int new_capacity = kb->capacity * 2;
        KnowledgeDocument* new_documents = (KnowledgeDocument*)realloc(kb->documents, sizeof(KnowledgeDocument) * new_capacity);
        if (!new_documents) {
            fprintf(stderr, "メモリ割り当てエラー: 知識ベースの拡張に失敗しました\n");
            return false;
        }
        kb->documents = new_documents;
        kb->capacity = new_capacity;
    }
    
    // 新しいドキュメントを追加
    strncpy(kb->documents[kb->count].title, title, sizeof(kb->documents[kb->count].title) - 1);
    kb->documents[kb->count].title[sizeof(kb->documents[kb->count].title) - 1] = '\0';
    
    strncpy(kb->documents[kb->count].content, content, sizeof(kb->documents[kb->count].content) - 1);
    kb->documents[kb->count].content[sizeof(kb->documents[kb->count].content) - 1] = '\0';
    
    strncpy(kb->documents[kb->count].category, category, sizeof(kb->documents[kb->count].category) - 1);
    kb->documents[kb->count].category[sizeof(kb->documents[kb->count].category) - 1] = '\0';
    
    kb->documents[kb->count].tag_count = 0;
    for (int i = 0; i < tag_count && i < 10; i++) {
        strncpy(kb->documents[kb->count].tags[i], tags[i], sizeof(kb->documents[kb->count].tags[i]) - 1);
        kb->documents[kb->count].tags[i][sizeof(kb->documents[kb->count].tags[i]) - 1] = '\0';
        kb->documents[kb->count].tag_count++;
    }
    
    kb->documents[kb->count].created_at = time(NULL);
    kb->documents[kb->count].updated_at = kb->documents[kb->count].created_at;
    
    // ドキュメントをファイルに保存
    knowledge_base_save_document(kb, &kb->documents[kb->count]);
    
    kb->count++;
    
    return true;
}

// 知識ドキュメントの検索（タイトルで）
KnowledgeDocument* knowledge_base_find_by_title(KnowledgeBase* kb, const char* title) {
    if (!kb || !title) {
        return NULL;
    }
    
    for (int i = 0; i < kb->count; i++) {
        if (strcmp(kb->documents[i].title, title) == 0) {
            return &kb->documents[i];
        }
    }
    
    return NULL;
}

// 知識ドキュメントの検索（カテゴリで）
KnowledgeDocument** knowledge_base_find_by_category(KnowledgeBase* kb, const char* category, int* count) {
    if (!kb || !category || !count) {
        return NULL;
    }
    
    // 該当するドキュメント数をカウント
    *count = 0;
    for (int i = 0; i < kb->count; i++) {
        if (strcmp(kb->documents[i].category, category) == 0) {
            (*count)++;
        }
    }
    
    if (*count == 0) {
        return NULL;
    }
    
    // 結果配列を確保
    KnowledgeDocument** results = (KnowledgeDocument**)malloc(sizeof(KnowledgeDocument*) * (*count));
    if (!results) {
        *count = 0;
        return NULL;
    }
    
    // 該当するドキュメントを結果配列に追加
    int index = 0;
    for (int i = 0; i < kb->count; i++) {
        if (strcmp(kb->documents[i].category, category) == 0) {
            results[index++] = &kb->documents[i];
        }
    }
    
    return results;
}

// 知識ドキュメントの検索（タグで）
KnowledgeDocument** knowledge_base_find_by_tag(KnowledgeBase* kb, const char* tag, int* count) {
    if (!kb || !tag || !count) {
        return NULL;
    }
    
    // 該当するドキュメント数をカウント
    *count = 0;
    for (int i = 0; i < kb->count; i++) {
        for (int j = 0; j < kb->documents[i].tag_count; j++) {
            if (strcmp(kb->documents[i].tags[j], tag) == 0) {
                (*count)++;
                break;
            }
        }
    }
    
    if (*count == 0) {
        return NULL;
    }
    
    // 結果配列を確保
    KnowledgeDocument** results = (KnowledgeDocument**)malloc(sizeof(KnowledgeDocument*) * (*count));
    if (!results) {
        *count = 0;
        return NULL;
    }
    
    // 該当するドキュメントを結果配列に追加
    int index = 0;
    for (int i = 0; i < kb->count; i++) {
        for (int j = 0; j < kb->documents[i].tag_count; j++) {
            if (strcmp(kb->documents[i].tags[j], tag) == 0) {
                results[index++] = &kb->documents[i];
                break;
            }
        }
    }
    
    return results;
}

// 知識ドキュメントの検索（内容で）
KnowledgeDocument** knowledge_base_find_by_content(KnowledgeBase* kb, const char* query, int* count) {
    if (!kb || !query || !count) {
        return NULL;
    }
    
    // 該当するドキュメント数をカウント
    *count = 0;
    for (int i = 0; i < kb->count; i++) {
        if (strstr(kb->documents[i].content, query) != NULL) {
            (*count)++;
        }
    }
    
    if (*count == 0) {
        return NULL;
    }
    
    // 結果配列を確保
    KnowledgeDocument** results = (KnowledgeDocument**)malloc(sizeof(KnowledgeDocument*) * (*count));
    if (!results) {
        *count = 0;
        return NULL;
    }
    
    // 該当するドキュメントを結果配列に追加
    int index = 0;
    for (int i = 0; i < kb->count; i++) {
        if (strstr(kb->documents[i].content, query) != NULL) {
            results[index++] = &kb->documents[i];
        }
    }
    
    return results;
}

// 知識ドキュメントの保存（ファイルに）
bool knowledge_base_save_document(KnowledgeBase* kb, const KnowledgeDocument* doc) {
    if (!kb || !doc) {
        return false;
    }
    
    // ファイル名を生成
    char filename[512];
    char safe_title[256];
    strncpy(safe_title, doc->title, sizeof(safe_title) - 1);
    safe_title[sizeof(safe_title) - 1] = '\0';
    
    // 警告を避けるために別々に構築
    strcpy(filename, kb->base_dir);
    strcat(filename, "/");
    strcat(filename, safe_title);
    strcat(filename, ".md");
    
    // ファイルを開く
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "ファイルオープンエラー: %s\n", filename);
        return false;
    }
    
    // メタデータを書き込む
    fprintf(file, "---\n");
    fprintf(file, "title: %s\n", doc->title);
    fprintf(file, "category: %s\n", doc->category);
    fprintf(file, "tags: ");
    for (int i = 0; i < doc->tag_count; i++) {
        fprintf(file, "%s", doc->tags[i]);
        if (i < doc->tag_count - 1) {
            fprintf(file, ", ");
        }
    }
    fprintf(file, "\n");
    
    char created_at_str[32];
    char updated_at_str[32];
    strftime(created_at_str, sizeof(created_at_str), "%Y-%m-%d %H:%M:%S", localtime(&doc->created_at));
    strftime(updated_at_str, sizeof(updated_at_str), "%Y-%m-%d %H:%M:%S", localtime(&doc->updated_at));
    
    fprintf(file, "created_at: %s\n", created_at_str);
    fprintf(file, "updated_at: %s\n", updated_at_str);
    fprintf(file, "---\n\n");
    
    // 内容を書き込む
    fprintf(file, "%s\n", doc->content);
    
    fclose(file);
    return true;
}

// 知識ベースの保存（すべてのドキュメント）
bool knowledge_base_save_all(KnowledgeBase* kb) {
    if (!kb) {
        return false;
    }
    
    bool success = true;
    for (int i = 0; i < kb->count; i++) {
        if (!knowledge_base_save_document(kb, &kb->documents[i])) {
            success = false;
        }
    }
    
    return success;
}

// 知識ベースの読み込み
bool knowledge_base_load(KnowledgeBase* kb) {
    if (!kb) {
        return false;
    }
    
    DIR* dir = opendir(kb->base_dir);
    if (!dir) {
        fprintf(stderr, "ディレクトリオープンエラー: %s\n", kb->base_dir);
        return false;
    }
    
    struct dirent* entry;
    kb->count = 0;
    
    while ((entry = readdir(dir)) != NULL) {
        // .mdファイルのみ処理
        size_t len = strlen(entry->d_name);
        if (len < 4 || strcmp(entry->d_name + len - 3, ".md") != 0) {
            continue;
        }
        
        // ファイルパスを生成
        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/%s", kb->base_dir, entry->d_name);
        
        // ファイルを開く
        FILE* file = fopen(filepath, "r");
        if (!file) {
            continue;
        }
        
        // 容量が足りない場合は拡張
        if (kb->count >= kb->capacity) {
            int new_capacity = kb->capacity * 2;
            KnowledgeDocument* new_documents = (KnowledgeDocument*)realloc(kb->documents, sizeof(KnowledgeDocument) * new_capacity);
            if (!new_documents) {
                fprintf(stderr, "メモリ割り当てエラー: 知識ベースの拡張に失敗しました\n");
                fclose(file);
                continue;
            }
            kb->documents = new_documents;
            kb->capacity = new_capacity;
        }
        
        // ドキュメントを初期化
        memset(&kb->documents[kb->count], 0, sizeof(KnowledgeDocument));
        
        // タイトルを設定（ファイル名から）
        strncpy(kb->documents[kb->count].title, entry->d_name, len - 3);
        kb->documents[kb->count].title[len - 3] = '\0';
        
        // メタデータを読み込む
        char line[MAX_LINE_LENGTH];
        bool in_metadata = false;
        bool in_content = false;
        
        while (fgets(line, sizeof(line), file)) {
            // 改行を削除
            size_t line_len = strlen(line);
            if (line_len > 0 && line[line_len - 1] == '\n') {
                line[line_len - 1] = '\0';
                line_len--;
            }
            
            // メタデータセクションの開始/終了
            if (strcmp(line, "---") == 0) {
                if (!in_metadata) {
                    in_metadata = true;
                } else {
                    in_metadata = false;
                    in_content = true;
                    continue;
                }
                continue;
            }
            
            if (in_metadata) {
                // メタデータの解析
                char* key = line;
                char* value = strchr(line, ':');
                if (value) {
                    *value = '\0';
                    value++;
                    // 先頭の空白をスキップ
                    while (*value == ' ') {
                        value++;
                    }
                    
                    if (strcmp(key, "category") == 0) {
                        strncpy(kb->documents[kb->count].category, value, sizeof(kb->documents[kb->count].category) - 1);
                        kb->documents[kb->count].category[sizeof(kb->documents[kb->count].category) - 1] = '\0';
                    } else if (strcmp(key, "tags") == 0) {
                        // タグをカンマで分割
                        char* tag = strtok(value, ",");
                        kb->documents[kb->count].tag_count = 0;
                        while (tag && kb->documents[kb->count].tag_count < 10) {
                            // 先頭と末尾の空白をスキップ
                            while (*tag == ' ') {
                                tag++;
                            }
                            char* end = tag + strlen(tag) - 1;
                            while (end > tag && *end == ' ') {
                                *end = '\0';
                                end--;
                            }
                            
                            strncpy(kb->documents[kb->count].tags[kb->documents[kb->count].tag_count], tag, sizeof(kb->documents[kb->count].tags[0]) - 1);
                            kb->documents[kb->count].tags[kb->documents[kb->count].tag_count][sizeof(kb->documents[kb->count].tags[0]) - 1] = '\0';
                            kb->documents[kb->count].tag_count++;
                            
                            tag = strtok(NULL, ",");
                        }
                    } else if (strcmp(key, "created_at") == 0) {
                        struct tm tm = {0};
                        if (strptime(value, "%Y-%m-%d %H:%M:%S", &tm) != NULL) {
                            kb->documents[kb->count].created_at = mktime(&tm);
                        }
                    } else if (strcmp(key, "updated_at") == 0) {
                        struct tm tm = {0};
                        if (strptime(value, "%Y-%m-%d %H:%M:%S", &tm) != NULL) {
                            kb->documents[kb->count].updated_at = mktime(&tm);
                        }
                    }
                }
            } else if (in_content) {
                // 内容を追加
                size_t content_len = strlen(kb->documents[kb->count].content);
                size_t remaining = sizeof(kb->documents[kb->count].content) - content_len - 1;
                
                if (remaining > 0) {
                    strncat(kb->documents[kb->count].content, line, remaining);
                    strncat(kb->documents[kb->count].content, "\n", remaining - strlen(line));
                }
            }
        }
        
        fclose(file);
        kb->count++;
    }
    
    closedir(dir);
    return true;
}

// 知識ドキュメントのベクトル化（簡易版）
float* knowledge_document_vectorize(const KnowledgeDocument* doc) {
    if (!doc) {
        return NULL;
    }
    
    // 簡易的なベクトル化（実際の実装ではもっと複雑になる）
    float* vector = (float*)malloc(sizeof(float) * 256);
    if (!vector) {
        return NULL;
    }
    
    // 初期化
    for (int i = 0; i < 256; i++) {
        vector[i] = 0.0f;
    }
    
    // 文字の出現頻度をカウント
    for (size_t i = 0; i < strlen(doc->content); i++) {
        unsigned char c = (unsigned char)doc->content[i];
        vector[c] += 1.0f;
    }
    
    // 正規化
    float sum = 0.0f;
    for (int i = 0; i < 256; i++) {
        sum += vector[i] * vector[i];
    }
    
    if (sum > 0.0f) {
        float norm = sqrt(sum);
        for (int i = 0; i < 256; i++) {
            vector[i] /= norm;
        }
    }
    
    return vector;
}

// 知識ドキュメントの類似度計算
float knowledge_document_similarity(const KnowledgeDocument* doc1, const KnowledgeDocument* doc2) {
    if (!doc1 || !doc2) {
        return 0.0f;
    }
    
    float* vec1 = knowledge_document_vectorize(doc1);
    float* vec2 = knowledge_document_vectorize(doc2);
    
    if (!vec1 || !vec2) {
        if (vec1) free(vec1);
        if (vec2) free(vec2);
        return 0.0f;
    }
    
    // コサイン類似度を計算
    float dot_product = 0.0f;
    for (int i = 0; i < 256; i++) {
        dot_product += vec1[i] * vec2[i];
    }
    
    free(vec1);
    free(vec2);
    
    return dot_product;
}