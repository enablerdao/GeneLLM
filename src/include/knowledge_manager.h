#ifndef KNOWLEDGE_MANAGER_H
#define KNOWLEDGE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// 知識ドキュメント構造体
typedef struct {
    char title[256];
    char content[4096];
    char category[64];
    char tags[10][32];
    int tag_count;
    time_t created_at;
    time_t updated_at;
} KnowledgeDocument;

// 知識ベース構造体
typedef struct {
    KnowledgeDocument* documents;
    int count;
    int capacity;
    char base_dir[256];
} KnowledgeBase;

// 知識ベースの初期化
KnowledgeBase* knowledge_base_init(const char* base_dir);

// 知識ベースの解放
void knowledge_base_free(KnowledgeBase* kb);

// 知識ドキュメントの追加
bool knowledge_base_add_document(KnowledgeBase* kb, const char* title, const char* content, 
                                const char* category, const char** tags, int tag_count);

// 知識ドキュメントの検索（タイトルで）
KnowledgeDocument* knowledge_base_find_by_title(KnowledgeBase* kb, const char* title);

// 知識ドキュメントの検索（カテゴリで）
KnowledgeDocument** knowledge_base_find_by_category(KnowledgeBase* kb, const char* category, int* count);

// 知識ドキュメントの検索（タグで）
KnowledgeDocument** knowledge_base_find_by_tag(KnowledgeBase* kb, const char* tag, int* count);

// 知識ドキュメントの検索（内容で）
KnowledgeDocument** knowledge_base_find_by_content(KnowledgeBase* kb, const char* query, int* count);

// 知識ドキュメントの保存（ファイルに）
bool knowledge_base_save_document(KnowledgeBase* kb, const KnowledgeDocument* doc);

// 知識ベースの保存（すべてのドキュメント）
bool knowledge_base_save_all(KnowledgeBase* kb);

// 知識ベースの読み込み
bool knowledge_base_load(KnowledgeBase* kb);

// 知識ドキュメントのベクトル化
float* knowledge_document_vectorize(const KnowledgeDocument* doc);

// 知識ドキュメントの類似度計算
float knowledge_document_similarity(const KnowledgeDocument* doc1, const KnowledgeDocument* doc2);

#endif // KNOWLEDGE_MANAGER_H