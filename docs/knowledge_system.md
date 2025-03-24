# 知識管理システム詳細

`src/include/knowledge_manager.c` は知識ベースの管理を担当するコンポーネントです。ドキュメントの保存、検索、ベクトル化などの機能を提供します。

## データ構造

### 知識ドキュメント

```c
// 知識ドキュメント構造体
typedef struct {
    char title[256];
    char content[16384]; // 16KBに拡大
    char category[64];
    char tags[10][32];
    int tag_count;
    time_t created_at;
    time_t updated_at;
} KnowledgeDocument;
```

各ドキュメントは以下の情報を持ちます：
- タイトル：ドキュメントの識別子
- 内容：ドキュメントの本文
- カテゴリ：ドキュメントの分類
- タグ：関連キーワード（最大10個）
- 作成日時と更新日時

### 知識ベース

```c
// 知識ベース構造体
typedef struct {
    KnowledgeDocument* documents;
    int count;
    int capacity;
    char base_dir[256];
    VectorDB vector_db;  // ベクトルデータベース
} KnowledgeBase;
```

知識ベースは以下の情報を管理します：
- ドキュメントの配列
- 現在のドキュメント数
- 最大容量
- ベースディレクトリ（ファイル保存先）
- ベクトルデータベース（意味検索用）

## 主要機能

### 1. 知識ベースの初期化

```c
KnowledgeBase* knowledge_base_init(const char* base_dir) {
    KnowledgeBase* kb = (KnowledgeBase*)malloc(sizeof(KnowledgeBase));
    
    kb->documents = (KnowledgeDocument*)malloc(sizeof(KnowledgeDocument) * KB_INITIAL_CAPACITY);
    kb->count = 0;
    kb->capacity = KB_INITIAL_CAPACITY;
    
    strncpy(kb->base_dir, base_dir, sizeof(kb->base_dir) - 1);
    
    // ディレクトリが存在しない場合は作成
    struct stat st = {0};
    if (stat(base_dir, &st) == -1) {
        mkdir(base_dir, 0700);
    }
    
    // 既存のドキュメントを読み込む
    knowledge_base_load(kb);
    
    return kb;
}
```

### 2. ドキュメントの追加

```c
bool knowledge_base_add_document(KnowledgeBase* kb, const char* title, const char* content,
                                const char* category, const char** tags, int tag_count) {
    // 既存のドキュメントを検索
    for (int i = 0; i < kb->count; i++) {
        if (strcmp(kb->documents[i].title, title) == 0) {
            // 既存のドキュメントを更新
            strncpy(kb->documents[i].content, content, sizeof(kb->documents[i].content) - 1);
            kb->documents[i].updated_at = time(NULL);
            
            // ベクトル化して保存
            knowledge_document_vectorize(&kb->documents[i]);
            knowledge_base_save_document(kb, &kb->documents[i]);
            return true;
        }
    }
    
    // 容量が足りない場合は拡張
    if (kb->count >= kb->capacity) {
        kb->capacity *= 2;
        kb->documents = (KnowledgeDocument*)realloc(kb->documents, sizeof(KnowledgeDocument) * kb->capacity);
    }
    
    // 新しいドキュメントを追加
    KnowledgeDocument* doc = &kb->documents[kb->count];
    strncpy(doc->title, title, sizeof(doc->title) - 1);
    strncpy(doc->content, content, sizeof(doc->content) - 1);
    strncpy(doc->category, category, sizeof(doc->category) - 1);
    
    // タグをコピー
    doc->tag_count = (tag_count > 10) ? 10 : tag_count;
    for (int i = 0; i < doc->tag_count; i++) {
        strncpy(doc->tags[i], tags[i], sizeof(doc->tags[0]) - 1);
    }
    
    // タイムスタンプを設定
    doc->created_at = time(NULL);
    doc->updated_at = doc->created_at;
    
    kb->count++;
    
    // ベクトル化して保存
    knowledge_document_vectorize(doc);
    knowledge_base_save_document(kb, doc);
    
    return true;
}
```

### 3. ドキュメントの検索

#### タイトルによる検索

```c
KnowledgeDocument* knowledge_base_find_by_title(KnowledgeBase* kb, const char* title) {
    for (int i = 0; i < kb->count; i++) {
        if (strcmp(kb->documents[i].title, title) == 0) {
            return &kb->documents[i];
        }
    }
    return NULL;
}
```

#### 内容による検索

```c
KnowledgeDocument** knowledge_base_find_by_content(KnowledgeBase* kb, const char* query, int* count) {
    *count = 0;
    
    // 該当するドキュメントの数をカウント
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
    
    // 該当するドキュメントを結果配列に追加
    int index = 0;
    for (int i = 0; i < kb->count; i++) {
        if (strstr(kb->documents[i].content, query) != NULL) {
            results[index++] = &kb->documents[i];
        }
    }
    
    return results;
}
```

#### カテゴリによる検索

```c
KnowledgeDocument** knowledge_base_find_by_category(KnowledgeBase* kb, const char* category, int* count) {
    *count = 0;
    
    // 該当するドキュメントの数をカウント
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
    
    // 該当するドキュメントを結果配列に追加
    int index = 0;
    for (int i = 0; i < kb->count; i++) {
        if (strcmp(kb->documents[i].category, category) == 0) {
            results[index++] = &kb->documents[i];
        }
    }
    
    return results;
}
```

### 4. ドキュメントの保存

```c
bool knowledge_base_save_document(KnowledgeBase* kb, const KnowledgeDocument* doc) {
    // ファイル名を生成
    char filename[512];
    char safe_title[256];
    strncpy(safe_title, doc->title, sizeof(safe_title) - 1);
    
    // ファイルパスを構築
    strcpy(filename, kb->base_dir);
    strcat(filename, "/");
    strcat(filename, safe_title);
    strcat(filename, ".md");
    
    // ファイルを開く
    FILE* file = fopen(filename, "w");
    
    // メタデータを書き込む（YAMLフロントマター）
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
    
    // タイムスタンプをフォーマット
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
```

### 5. ベクトル検索との連携

```c
// ドキュメントをベクトル化
void knowledge_document_vectorize(KnowledgeDocument* doc) {
    // ドキュメントの内容からベクトルを生成
    float vector[VECTOR_DIM];
    text_to_vector(doc->content, vector);
    
    // ベクトルデータベースに追加
    add_vector_to_db(&global_vector_db, doc->title, vector);
}

// ドキュメント間の類似度計算
float knowledge_document_similarity(const KnowledgeDocument* doc1, const KnowledgeDocument* doc2) {
    float vec1[VECTOR_DIM];
    float vec2[VECTOR_DIM];
    
    // 各ドキュメントの内容をベクトル化
    text_to_vector(doc1->content, vec1);
    text_to_vector(doc2->content, vec2);
    
    // コサイン類似度を計算
    return cosine_similarity(vec1, vec2, VECTOR_DIM);
}
```

## ファイル形式

知識ベースのドキュメントはマークダウン形式で保存されます。各ファイルは以下の構造を持ちます：

```markdown
---
title: ドキュメントタイトル
category: カテゴリ
tags: タグ1, タグ2, タグ3
created_at: 2023-01-01 12:00:00
updated_at: 2023-01-02 15:30:00
---

ドキュメントの内容がここに記述されます。
複数行にわたるテキストや、マークダウン形式の書式も使用できます。

## 見出し

- リスト項目1
- リスト項目2

コードブロックなども含めることができます：

```c
printf("Hello, World!\n");
```

## 拡張ポイント

知識管理システムは以下の点で拡張可能です：

1. 検索アルゴリズムの改善（より高度な全文検索など）
2. ドキュメント形式の拡張（画像や構造化データの対応など）
3. インデックス機能の追加（高速検索のため）
4. バージョン管理機能の実装（ドキュメントの変更履歴）
5. 分散知識ベースのサポート（複数のソースからの統合）