#ifndef IMPROVED_RESPONSE_GENERATOR_H
#define IMPROVED_RESPONSE_GENERATOR_H

#include "knowledge_manager.h"
#include "response_evaluator.h"

#define MAX_CANDIDATE_RESPONSES 5
#define MIN_ACCEPTABLE_SCORE 0.6f

// 回答生成オプション
typedef struct {
    bool use_knowledge_base;     // 知識ベースを使用するか
    bool use_web_search;         // Web検索を使用するか
    bool use_evaluation;         // 回答評価を使用するか
    int max_candidates;          // 生成する候補回答の最大数
    float min_score;             // 許容する最小スコア
    int max_response_length;     // 回答の最大長
} ResponseGeneratorOptions;

// 回答生成器の初期化
void init_improved_response_generator();

// 回答生成器の設定
void configure_response_generator(ResponseGeneratorOptions options);

// 質問に対する回答を生成
char* generate_improved_response(const char* query, KnowledgeBase* kb);

// 知識ベースから関連情報を検索
KnowledgeDocument** search_relevant_knowledge(KnowledgeBase* kb, const char* query, int* count);

// 検索結果から回答を生成
char* generate_response_from_knowledge(const char* query, KnowledgeDocument** documents, int count);

// 複数の候補回答を生成
char** generate_candidate_responses(const char* query, KnowledgeDocument** documents, int count, int* candidate_count);

// 回答を評価して最適なものを選択
char* select_and_refine_response(const char* query, char** candidates, int count);

// 回答を質問に合わせて調整
char* adapt_response_to_query(const char* query, const char* response);

// 回答の品質を向上させる
char* enhance_response_quality(const char* query, const char* response);

// 回答を生成するためのテンプレートを選択
const char* select_response_template(const char* query);

// 回答生成器の解放
void free_improved_response_generator();

#endif // IMPROVED_RESPONSE_GENERATOR_H