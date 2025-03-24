# ルーターモデル詳細と改善案

GeneLLMのルーターモデルは、入力テキストを分析し、適切なエージェントに処理を振り分ける役割を担います。現在の実装には重複や統合不足の問題がありますが、以下に詳細な説明と改善案を示します。

## 現状の問題点

### 1. コードの重複と統合不足

- `main.c` と `improved_router_model.c` に類似した機能が重複して実装されています
- 両ファイルでそれぞれ独自のルーティングロジックが定義されています
- 改良型ルーターモデルが部分的にしか統合されていません

### 2. エージェントシステムの限定的な活用

- エージェントベースのアーキテクチャが実装されていますが、十分に活用されていません
- エージェント間の協調メカニズムが限定的です
- エージェントの評価と選択プロセスが最適化されていません

## 改善案

### 1. モジュール化と責任の分離

ルーティングロジックを専用のモジュールに分離し、各エージェントを個別のファイルに分割することで、責任を明確化します。

#### ルーターモデルのヘッダファイル

```c
// src/router/router_model.h
#ifndef ROUTER_MODEL_H
#define ROUTER_MODEL_H

#include <stdbool.h>
#include "../include/knowledge_manager.h"
#include "../include/learning_module.h"

// エージェントの種類
typedef enum {
    AGENT_GENERAL,       // 一般的な質問応答
    AGENT_PROGRAMMING,   // プログラミング関連
    AGENT_MATH,          // 数学関連
    AGENT_KNOWLEDGE,     // 知識ベース検索
    AGENT_LEARNING,      // 学習データベース検索
    AGENT_COUNT          // エージェントの総数
} AgentType;

// エージェント構造体
typedef struct {
    AgentType type;
    char name[64];
    float (*evaluate)(const char* query);
    bool (*process)(const char* query, char* response, size_t response_size);
} Agent;

// ルーターモデルの初期化
void router_init(KnowledgeBase* kb, LearningDB* ldb);

// ルーターモデルの終了処理
void router_cleanup();

// テキストのルーティング処理
bool route_text(const char* text, char* response, size_t response_size);

// エージェントの登録
bool register_agent(AgentType type, const char* name, 
                   float (*evaluate)(const char* query),
                   bool (*process)(const char* query, char* response, size_t response_size));

// デバッグ情報の出力設定
void router_set_debug(bool enable);

#endif // ROUTER_MODEL_H
```

#### ルーターモデルの実装

```c
// src/router/router_model.c
#include "router_model.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mecab.h>

// グローバル変数
static Agent agents[AGENT_COUNT];
static KnowledgeBase* knowledge_base = NULL;
static LearningDB* learning_db = NULL;
static bool debug_mode = false;

// ルーターモデルの初期化
void router_init(KnowledgeBase* kb, LearningDB* ldb) {
    knowledge_base = kb;
    learning_db = ldb;
    
    // エージェントの初期化
    memset(agents, 0, sizeof(agents));
    
    // デフォルトエージェントの登録
    register_default_agents();
}

// デフォルトエージェントの登録
static void register_default_agents() {
    // 一般的な質問応答エージェント
    register_agent(AGENT_GENERAL, "一般質問応答", 
                  evaluate_general_agent,
                  process_general_agent);
    
    // プログラミング関連エージェント
    register_agent(AGENT_PROGRAMMING, "プログラミング", 
                  evaluate_programming_agent,
                  process_programming_agent);
    
    // 知識ベース検索エージェント
    register_agent(AGENT_KNOWLEDGE, "知識ベース", 
                  evaluate_knowledge_agent,
                  process_knowledge_agent);
    
    // 学習データベース検索エージェント
    register_agent(AGENT_LEARNING, "学習データベース", 
                  evaluate_learning_agent,
                  process_learning_agent);
}

// エージェントの登録
bool register_agent(AgentType type, const char* name, 
                   float (*evaluate)(const char* query),
                   bool (*process)(const char* query, char* response, size_t response_size)) {
    if (type >= AGENT_COUNT || !evaluate || !process) {
        return false;
    }
    
    strncpy(agents[type].name, name, sizeof(agents[type].name) - 1);
    agents[type].type = type;
    agents[type].evaluate = evaluate;
    agents[type].process = process;
    
    return true;
}

// テキストのルーティング処理
bool route_text(const char* text, char* response, size_t response_size) {
    if (!text || !response || response_size == 0) {
        return false;
    }
    
    // 応答を初期化
    response[0] = '\0';
    
    // 各エージェントの評価スコアを計算
    float scores[AGENT_COUNT] = {0};
    float total_score = 0;
    int best_agent = -1;
    float best_score = -1;
    
    for (int i = 0; i < AGENT_COUNT; i++) {
        if (agents[i].evaluate) {
            scores[i] = agents[i].evaluate(text);
            total_score += scores[i];
            
            if (scores[i] > best_score) {
                best_score = scores[i];
                best_agent = i;
            }
            
            if (debug_mode) {
                printf("エージェント[%s]の評価スコア: %.4f\n", agents[i].name, scores[i]);
            }
        }
    }
    
    // 最適なエージェントが見つからない場合
    if (best_agent < 0 || best_score < 0.1) {
        snprintf(response, response_size, "申し訳ありませんが、「%s」についての情報は見つかりませんでした。", text);
        return false;
    }
    
    // 選択されたエージェントで処理
    if (debug_mode) {
        printf("選択されたエージェント: %s (スコア: %.4f)\n", agents[best_agent].name, best_score);
    }
    
    bool result = agents[best_agent].process(text, response, response_size);
    
    // 処理に失敗した場合のフォールバック
    if (!result || response[0] == '\0') {
        snprintf(response, response_size, "申し訳ありませんが、「%s」についての情報は見つかりませんでした。", text);
        return false;
    }
    
    return true;
}

// デバッグ情報の出力設定
void router_set_debug(bool enable) {
    debug_mode = enable;
}

// ルーターモデルの終了処理
void router_cleanup() {
    // 必要に応じてリソースを解放
}
```

### 2. エージェントシステムの強化

各エージェントを個別のファイルに分離し、責任を明確化します。

#### 一般的な質問応答エージェント

```c
// src/agents/general_agent.c
#include "../router/router_model.h"
#include <stdio.h>
#include <string.h>

// 一般的な質問応答エージェントの評価関数
float evaluate_general_agent(const char* query) {
    // 常に基本スコアを返す（フォールバックエージェント）
    return 0.5;
}

// 一般的な質問応答エージェントの処理関数
bool process_general_agent(const char* query, char* response, size_t response_size) {
    // 知識ベースから関連情報を検索
    int count = 0;
    KnowledgeDocument** docs = knowledge_base_find_by_content(knowledge_base, query, &count);
    
    if (docs && count > 0) {
        // 最も関連性の高いドキュメントから回答を生成
        generate_response_from_document(docs[0], query, response, response_size);
        free(docs);
        return true;
    }
    
    // ベクトル検索で意味的に類似したドキュメントを検索
    float query_vector[VECTOR_DIM];
    text_to_vector(query, query_vector);
    
    SearchResult result = search_nearest_vector(&knowledge_base->vector_db, query_vector);
    if (result.score >= 0.7) {
        KnowledgeDocument* doc = knowledge_base_find_by_title(knowledge_base, result.id);
        if (doc) {
            generate_response_from_document(doc, query, response, response_size);
            return true;
        }
    }
    
    // 一般的な応答を生成
    snprintf(response, response_size, "申し訳ありませんが、「%s」についての情報は見つかりませんでした。", query);
    return false;
}
```

#### プログラミング関連エージェント

```c
// src/agents/programming_agent.c
#include "../router/router_model.h"
#include <stdio.h>
#include <string.h>

// プログラミング関連キーワード
static const char* programming_keywords[] = {
    "プログラミング", "コード", "関数", "変数", "クラス", "オブジェクト",
    "C言語", "Python", "Java", "JavaScript", "コンパイル", "デバッグ",
    "アルゴリズム", "データ構造", "配列", "リスト", "ループ", "条件分岐",
    "ポインタ", "メモリ", "スタック", "ヒープ", "再帰", "ライブラリ"
};
static const int keyword_count = sizeof(programming_keywords) / sizeof(programming_keywords[0]);

// プログラミングエージェントの評価関数
float evaluate_programming_agent(const char* query) {
    float score = 0.0;
    
    // プログラミング関連キーワードの出現をチェック
    for (int i = 0; i < keyword_count; i++) {
        if (strstr(query, programming_keywords[i]) != NULL) {
            score += 0.2;  // キーワードごとにスコアを加算
        }
    }
    
    // スコアの上限を設定
    if (score > 0.9) {
        score = 0.9;
    }
    
    return score;
}

// プログラミングエージェントの処理関数
bool process_programming_agent(const char* query, char* response, size_t response_size) {
    // プログラミング関連の知識ベースから検索
    int count = 0;
    KnowledgeDocument** docs = knowledge_base_find_by_category(knowledge_base, "プログラミング", &count);
    
    if (docs && count > 0) {
        // 関連性の高いドキュメントを選択
        KnowledgeDocument* best_doc = NULL;
        float best_score = -1;
        
        for (int i = 0; i < count; i++) {
            float similarity = calculate_document_query_similarity(docs[i], query);
            if (similarity > best_score) {
                best_score = similarity;
                best_doc = docs[i];
            }
        }
        
        if (best_doc && best_score > 0.6) {
            generate_response_from_document(best_doc, query, response, response_size);
            free(docs);
            return true;
        }
        
        free(docs);
    }
    
    // プログラミング関連の一般的な応答を生成
    // ...
    
    return false;
}
```

### 3. メイン関数での統合

```c
// src/main.c (一部抜粋)
#include "router/router_model.h"

int main(int argc, char* argv[]) {
    // 初期化処理
    // ...
    
    // ルーターモデルの初期化
    router_init(knowledge_base, learning_db);
    
    // デバッグモードの設定
    if (debug_mode) {
        router_set_debug(true);
    }
    
    // テキスト処理
    char response[MAX_RESPONSE_LEN];
    route_text(text_buffer, response, sizeof(response));
    
    // 応答を表示
    printf("%s\n", response);
    
    // 終了処理
    router_cleanup();
    // ...
    
    return 0;
}
```

## 改善点のまとめ

### 1. モジュール化と責任の分離

- ルーティングロジックを専用のモジュールに分離
- 各エージェントを個別のファイルに分割し、責任を明確化
- 共通インターフェースによる一貫性の確保

### 2. エージェントシステムの強化

- エージェント登録メカニズムの導入
- 評価関数と処理関数の分離による柔軟性の向上
- エージェント間の協調メカニズムの基盤を整備

### 3. 拡張性の向上

- 新しいエージェントの追加が容易になるアーキテクチャ
- 評価ロジックと処理ロジックの分離による柔軟な拡張
- プラグイン的なエージェント追加の仕組み

### 4. デバッグ機能の強化

- エージェント選択プロセスの透明化
- 各エージェントの評価スコアの可視化
- 詳細なログ出力オプション

## 実装計画

1. ルーターモデルのヘッダファイルと基本実装の作成
2. 既存のルーティングロジックの移行
3. 基本エージェントの実装
4. メイン関数との統合
5. テストと最適化

この改善により、GeneLLMのルーターモデルはより整理され、拡張性が高まります。また、エージェントシステムの活用が促進され、より複雑なタスク処理や協調動作が可能になります。