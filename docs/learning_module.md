# 学習モジュール詳細

`src/include/learning_module.c` は過去の質問と回答のパターンを学習し、類似の質問に対して既存の回答を再利用する機能を提供します。

## データ構造

### 学習エントリ

```c
// 学習エントリ構造体
typedef struct {
    char question[MAX_QUESTION_LEN];
    char answer[MAX_ANSWER_LEN];
    float confidence;
} LearningEntry;
```

各エントリは以下の情報を持ちます：
- 質問：ユーザーからの入力テキスト
- 回答：システムが生成した応答
- 確信度：回答の信頼性（0.0〜1.0）

### 学習データベース

```c
// 学習データベース構造体
typedef struct {
    LearningEntry* entries;
    int count;
    int capacity;
    char filename[256];
} LearningDB;
```

学習データベースは以下の情報を管理します：
- エントリの配列
- 現在のエントリ数
- 最大容量
- データベースファイルのパス

## 主要機能

### 1. 学習データベースの初期化

```c
LearningDB* learning_db_init(const char* filename) {
    LearningDB* db = (LearningDB*)malloc(sizeof(LearningDB));
    
    db->entries = (LearningEntry*)malloc(sizeof(LearningEntry) * INITIAL_CAPACITY);
    db->count = 0;
    db->capacity = INITIAL_CAPACITY;
    strncpy(db->filename, filename, sizeof(db->filename) - 1);
    
    // 既存のデータを読み込む
    learning_db_load(db);
    
    return db;
}
```

### 2. 学習データの追加

```c
bool learning_db_add(LearningDB* db, const char* question, const char* answer, float confidence) {
    // 既存のエントリを検索
    for (int i = 0; i < db->count; i++) {
        if (strcmp(db->entries[i].question, question) == 0) {
            // 既存のエントリを更新
            strncpy(db->entries[i].answer, answer, sizeof(db->entries[i].answer) - 1);
            db->entries[i].confidence = confidence;
            
            // データベースを保存
            learning_db_save(db);
            return true;
        }
    }
    
    // 容量が足りない場合は拡張
    if (db->count >= db->capacity) {
        db->capacity *= 2;
        db->entries = (LearningEntry*)realloc(db->entries, sizeof(LearningEntry) * db->capacity);
    }
    
    // 新しいエントリを追加
    LearningEntry* entry = &db->entries[db->count];
    strncpy(entry->question, question, sizeof(entry->question) - 1);
    strncpy(entry->answer, answer, sizeof(entry->answer) - 1);
    entry->confidence = confidence;
    
    db->count++;
    
    // データベースを保存
    learning_db_save(db);
    return true;
}
```

### 3. 類似質問の検索

```c
LearningEntry* learning_db_find(LearningDB* db, const char* question, float* similarity) {
    if (db->count == 0) {
        return NULL;
    }
    
    float max_similarity = 0.0f;
    int best_match = -1;
    
    // 各エントリとの類似度を計算
    for (int i = 0; i < db->count; i++) {
        float sim = calculate_question_similarity(question, db->entries[i].question);
        
        if (sim > max_similarity) {
            max_similarity = sim;
            best_match = i;
        }
    }
    
    // 類似度が閾値を超えた場合のみ結果を返す
    if (max_similarity >= SIMILARITY_THRESHOLD && best_match >= 0) {
        if (similarity) {
            *similarity = max_similarity;
        }
        return &db->entries[best_match];
    }
    
    return NULL;
}
```

### 4. 質問の類似度計算

```c
float calculate_question_similarity(const char* q1, const char* q2) {
    // ベクトル化
    float vec1[VECTOR_DIM];
    float vec2[VECTOR_DIM];
    
    text_to_vector(q1, vec1);
    text_to_vector(q2, vec2);
    
    // コサイン類似度を計算
    return cosine_similarity(vec1, vec2, VECTOR_DIM);
}
```

### 5. 学習データの保存

```c
bool learning_db_save(LearningDB* db) {
    FILE* file = fopen(db->filename, "w");
    if (!file) {
        return false;
    }
    
    // 各エントリを保存
    for (int i = 0; i < db->count; i++) {
        LearningEntry* entry = &db->entries[i];
        
        // 質問と回答をエスケープ
        char escaped_question[MAX_QUESTION_LEN * 2];
        char escaped_answer[MAX_ANSWER_LEN * 2];
        
        escape_string(entry->question, escaped_question);
        escape_string(entry->answer, escaped_answer);
        
        // エントリを書き込む
        fprintf(file, "%s|%s|%.4f\n", escaped_question, escaped_answer, entry->confidence);
    }
    
    fclose(file);
    return true;
}
```

### 6. 学習データの読み込み

```c
bool learning_db_load(LearningDB* db) {
    FILE* file = fopen(db->filename, "r");
    if (!file) {
        return false;
    }
    
    char line[MAX_LINE_LEN];
    db->count = 0;
    
    // 各行を読み込む
    while (fgets(line, sizeof(line), file) && db->count < db->capacity) {
        // 改行を削除
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        
        // 行を解析
        char* question_part = strtok(line, "|");
        char* answer_part = strtok(NULL, "|");
        char* confidence_part = strtok(NULL, "|");
        
        if (!question_part || !answer_part || !confidence_part) {
            continue;
        }
        
        // エントリを作成
        LearningEntry* entry = &db->entries[db->count];
        
        // エスケープされた文字列を元に戻す
        unescape_string(question_part, entry->question);
        unescape_string(answer_part, entry->answer);
        
        // 確信度を解析
        entry->confidence = atof(confidence_part);
        
        db->count++;
    }
    
    fclose(file);
    return true;
}
```

### 7. 文字列のエスケープと復元

```c
// 文字列をエスケープ（区切り文字や制御文字を処理）
void escape_string(const char* src, char* dest) {
    int j = 0;
    for (int i = 0; src[i] != '\0'; i++) {
        if (src[i] == '|' || src[i] == '\\' || src[i] == '\n' || src[i] == '\r') {
            dest[j++] = '\\';
        }
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}

// エスケープされた文字列を元に戻す
void unescape_string(const char* src, char* dest) {
    int j = 0;
    for (int i = 0; src[i] != '\0'; i++) {
        if (src[i] == '\\' && src[i+1] != '\0') {
            i++;
        }
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}
```

## 学習データベースの活用

学習モジュールは、メインプログラムの質問処理フローで以下のように活用されます：

```c
// ユーザーからの質問を処理
void process_question(const char* question, char* response) {
    // 学習データベースで類似の質問を検索
    float similarity = 0.0f;
    LearningEntry* entry = learning_db_find(learning_db, question, &similarity);
    
    // 十分に類似した質問が見つかった場合
    if (entry && similarity >= 0.8) {
        // 既存の回答を再利用
        strncpy(response, entry->answer, MAX_RESPONSE_LEN - 1);
        
        if (debug_mode) {
            printf("学習データベースから回答が見つかりました (確信度: %.4f)\n", entry->confidence);
        }
        
        return;
    }
    
    // 類似の質問が見つからない場合は新しい回答を生成
    generate_new_response(question, response);
    
    // 新しい質問と回答のペアを学習データベースに追加
    learning_db_add(learning_db, question, response, 0.7);
}
```

## ファイル形式

学習データベースは以下の形式のテキストファイルとして保存されます：

```
質問1|回答1|0.9000
質問2|回答2|0.8500
質問3|回答3|0.7200
...
```

各行は質問、回答、確信度の3つのフィールドを `|` で区切って表現します。特殊文字（`|`, `\`, `\n`, `\r`）はバックスラッシュでエスケープされます。

## 拡張ポイント

学習モジュールは以下の点で拡張可能です：

1. 確信度の動的調整（フィードバックに基づく更新）
2. 複数の回答候補の管理（同じ質問に対する複数の回答）
3. 時間経過による確信度の減衰（古い学習データの重要度低下）
4. クラスタリングによる類似質問のグループ化
5. 構造化データの学習（質問のカテゴリや属性など）
6. 分散学習データベースのサポート