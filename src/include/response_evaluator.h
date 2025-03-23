#ifndef RESPONSE_EVALUATOR_H
#define RESPONSE_EVALUATOR_H

#include <stdbool.h>

// 評価スコアの構造体
typedef struct {
    float relevance;      // 関連性スコア（0.0〜1.0）
    float coherence;      // 一貫性スコア（0.0〜1.0）
    float informativeness; // 情報量スコア（0.0〜1.0）
    float overall;        // 総合スコア（0.0〜1.0）
} EvaluationScore;

// 回答評価の初期化
void init_response_evaluator();

// 回答の評価
EvaluationScore evaluate_response(const char* query, const char* response);

// 関連性スコアの計算
float calculate_relevance_score(const char* query, const char* response);

// 一貫性スコアの計算
float calculate_coherence_score(const char* response);

// 情報量スコアの計算
float calculate_informativeness_score(const char* response);

// 回答が質問に直接答えているかを評価
bool is_direct_answer(const char* query, const char* response);

// 回答の改善提案を生成
char* generate_improvement_suggestions(const char* query, const char* response, const EvaluationScore* score);

// 複数の候補回答から最適なものを選択
char* select_best_response(const char* query, char** candidate_responses, int count);

// 評価結果をデバッグ出力
void print_evaluation_result(const char* query, const char* response, const EvaluationScore* score);

#endif // RESPONSE_EVALUATOR_H