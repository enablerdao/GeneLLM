#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "response_evaluator.h"

// 最大文字列長
#define MAX_QUERY_LEN 1024
#define MAX_RESPONSE_LEN 4096
#define MAX_WORD_LEN 64
#define MAX_WORDS 200

// 単語の重要度を表す構造体
typedef struct {
    char word[MAX_WORD_LEN];
    float weight;
} KeywordWeight;

// グローバル変数
static KeywordWeight question_keywords[MAX_WORDS];
static int question_keyword_count = 0;

// 回答評価の初期化
void init_response_evaluator() {
    // 質問キーワードの初期化
    question_keyword_count = 0;
    
    // 質問を示す重要なキーワードとその重み
    const char* keywords[] = {
        "何", "なに", "どう", "どの", "いつ", "どこ", "だれ", "誰", "なぜ", "どうして",
        "教えて", "説明", "解説", "定義", "意味", "違い", "方法", "手順", "特徴", "例"
    };
    const float weights[] = {
        1.0f, 1.0f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f,
        0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.7f, 0.7f
    };
    
    // キーワードと重みを設定
    int keyword_count = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < keyword_count && i < MAX_WORDS; i++) {
        strncpy(question_keywords[i].word, keywords[i], MAX_WORD_LEN - 1);
        question_keywords[i].word[MAX_WORD_LEN - 1] = '\0';
        question_keywords[i].weight = weights[i];
        question_keyword_count++;
    }
}

// 文字列から単語を抽出
static int extract_words(const char* text, char words[][MAX_WORD_LEN], int max_words) {
    int word_count = 0;
    char text_copy[MAX_RESPONSE_LEN];
    strncpy(text_copy, text, sizeof(text_copy) - 1);
    text_copy[sizeof(text_copy) - 1] = '\0';
    
    // 簡易的な単語分割（スペース、句読点で区切る）
    char* token = strtok(text_copy, " ,.:;?!()[]{}\"'\n\t");
    while (token && word_count < max_words) {
        // 単語の長さチェック
        if (strlen(token) < MAX_WORD_LEN) {
            strncpy(words[word_count], token, MAX_WORD_LEN - 1);
            words[word_count][MAX_WORD_LEN - 1] = '\0';
            word_count++;
        }
        token = strtok(NULL, " ,.:;?!()[]{}\"'\n\t");
    }
    
    return word_count;
}

// 文字列の類似度を計算（ジャッカード係数）
static float calculate_jaccard_similarity(const char* str1, const char* str2) {
    char words1[MAX_WORDS][MAX_WORD_LEN];
    char words2[MAX_WORDS][MAX_WORD_LEN];
    
    int word_count1 = extract_words(str1, words1, MAX_WORDS);
    int word_count2 = extract_words(str2, words2, MAX_WORDS);
    
    if (word_count1 == 0 || word_count2 == 0) {
        return 0.0f;
    }
    
    // 共通単語数をカウント
    int common_count = 0;
    for (int i = 0; i < word_count1; i++) {
        for (int j = 0; j < word_count2; j++) {
            if (strcmp(words1[i], words2[j]) == 0) {
                common_count++;
                break;
            }
        }
    }
    
    // ジャッカード係数を計算
    int union_count = word_count1 + word_count2 - common_count;
    return (float)common_count / union_count;
}

// 文字列の類似度を計算（コサイン類似度の簡易版）
static float calculate_cosine_similarity(const char* str1, const char* str2) {
    char words1[MAX_WORDS][MAX_WORD_LEN];
    char words2[MAX_WORDS][MAX_WORD_LEN];
    
    int word_count1 = extract_words(str1, words1, MAX_WORDS);
    int word_count2 = extract_words(str2, words2, MAX_WORDS);
    
    if (word_count1 == 0 || word_count2 == 0) {
        return 0.0f;
    }
    
    // 単語の出現回数をカウント
    int unique_words = 0;
    char unique_word_list[MAX_WORDS * 2][MAX_WORD_LEN];
    int vec1[MAX_WORDS * 2] = {0};
    int vec2[MAX_WORDS * 2] = {0};
    
    // 最初の文字列の単語を処理
    for (int i = 0; i < word_count1; i++) {
        int found = 0;
        for (int j = 0; j < unique_words; j++) {
            if (strcmp(words1[i], unique_word_list[j]) == 0) {
                vec1[j]++;
                found = 1;
                break;
            }
        }
        if (!found && unique_words < MAX_WORDS * 2) {
            strncpy(unique_word_list[unique_words], words1[i], MAX_WORD_LEN - 1);
            unique_word_list[unique_words][MAX_WORD_LEN - 1] = '\0';
            vec1[unique_words] = 1;
            unique_words++;
        }
    }
    
    // 2番目の文字列の単語を処理
    for (int i = 0; i < word_count2; i++) {
        int found = 0;
        for (int j = 0; j < unique_words; j++) {
            if (strcmp(words2[i], unique_word_list[j]) == 0) {
                vec2[j]++;
                found = 1;
                break;
            }
        }
        if (!found && unique_words < MAX_WORDS * 2) {
            strncpy(unique_word_list[unique_words], words2[i], MAX_WORD_LEN - 1);
            unique_word_list[unique_words][MAX_WORD_LEN - 1] = '\0';
            vec2[unique_words] = 1;
            unique_words++;
        }
    }
    
    // コサイン類似度を計算
    float dot_product = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (int i = 0; i < unique_words; i++) {
        dot_product += vec1[i] * vec2[i];
        norm1 += vec1[i] * vec1[i];
        norm2 += vec2[i] * vec2[i];
    }
    
    if (norm1 == 0.0f || norm2 == 0.0f) {
        return 0.0f;
    }
    
    return dot_product / (sqrtf(norm1) * sqrtf(norm2));
}

// 質問タイプを判定
static float detect_question_type(const char* query) {
    float question_score = 0.0f;
    
    // 質問キーワードの存在をチェック
    for (int i = 0; i < question_keyword_count; i++) {
        if (strstr(query, question_keywords[i].word) != NULL) {
            question_score += question_keywords[i].weight;
        }
    }
    
    // 疑問符の存在をチェック
    if (strchr(query, '?') != NULL || strchr(query, '？') != NULL) {
        question_score += 0.5f;
    }
    
    // スコアを0〜1の範囲に正規化
    if (question_score > 1.0f) {
        question_score = 1.0f;
    }
    
    return question_score;
}

// 回答の情報量を評価
static float evaluate_information_content(const char* response) {
    // 単語数をカウント
    char words[MAX_WORDS][MAX_WORD_LEN];
    int word_count = extract_words(response, words, MAX_WORDS);
    
    // 単語の多様性を評価
    int unique_words = 0;
    char unique_word_list[MAX_WORDS][MAX_WORD_LEN];
    
    for (int i = 0; i < word_count; i++) {
        int found = 0;
        for (int j = 0; j < unique_words; j++) {
            if (strcmp(words[i], unique_word_list[j]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found && unique_words < MAX_WORDS) {
            strncpy(unique_word_list[unique_words], words[i], MAX_WORD_LEN - 1);
            unique_word_list[unique_words][MAX_WORD_LEN - 1] = '\0';
            unique_words++;
        }
    }
    
    // 情報量スコアを計算
    float diversity_ratio = (word_count > 0) ? (float)unique_words / word_count : 0.0f;
    float length_factor = (word_count > 100) ? 1.0f : (float)word_count / 100.0f;
    
    return diversity_ratio * 0.6f + length_factor * 0.4f;
}

// 関連性スコアの計算
float calculate_relevance_score(const char* query, const char* response) {
    // 類似度を計算
    float jaccard_sim = calculate_jaccard_similarity(query, response);
    float cosine_sim = calculate_cosine_similarity(query, response);
    
    // 質問タイプを判定
    float question_type_score = detect_question_type(query);
    
    // 関連性スコアを計算
    float relevance = jaccard_sim * 0.3f + cosine_sim * 0.5f + question_type_score * 0.2f;
    
    // スコアを0〜1の範囲に制限
    if (relevance > 1.0f) {
        relevance = 1.0f;
    }
    
    return relevance;
}

// 一貫性スコアの計算
float calculate_coherence_score(const char* response) {
    // 文の数をカウント
    int sentence_count = 0;
    for (int i = 0; response[i] != '\0'; i++) {
        if (response[i] == '.' || response[i] == '。' || 
            response[i] == '!' || response[i] == '！' || 
            response[i] == '?' || response[i] == '？') {
            sentence_count++;
        }
    }
    
    // 短すぎる回答は一貫性が低いと判断
    if (sentence_count < 2) {
        return 0.5f;
    }
    
    // 文字列の長さを取得
    size_t response_len = strlen(response);
    
    // 一文あたりの平均文字数を計算
    float avg_sentence_length = (float)response_len / sentence_count;
    
    // 適切な文の長さを評価（短すぎず長すぎない）
    float length_score = 0.0f;
    if (avg_sentence_length >= 15.0f && avg_sentence_length <= 50.0f) {
        length_score = 1.0f;
    } else if (avg_sentence_length < 15.0f) {
        length_score = avg_sentence_length / 15.0f;
    } else {
        length_score = 1.0f - ((avg_sentence_length - 50.0f) / 100.0f);
        if (length_score < 0.0f) length_score = 0.0f;
    }
    
    // 文の接続性を簡易評価（接続詞の存在など）
    const char* connectors[] = {"また", "そして", "しかし", "ただし", "一方", "例えば", "つまり", "したがって"};
    int connector_count = sizeof(connectors) / sizeof(connectors[0]);
    
    int connector_found = 0;
    for (int i = 0; i < connector_count; i++) {
        if (strstr(response, connectors[i]) != NULL) {
            connector_found++;
        }
    }
    
    float connector_score = (connector_found > 0) ? 
        fminf(1.0f, (float)connector_found / (sentence_count - 1)) : 0.0f;
    
    // 一貫性スコアを計算
    float coherence = length_score * 0.5f + connector_score * 0.5f;
    
    return coherence;
}

// 情報量スコアの計算
float calculate_informativeness_score(const char* response) {
    return evaluate_information_content(response);
}

// 回答が質問に直接答えているかを評価
bool is_direct_answer(const char* query, const char* response) {
    // 関連性スコアが高い場合は直接的な回答と判断
    float relevance = calculate_relevance_score(query, response);
    return relevance >= 0.7f;
}

// 回答の評価
EvaluationScore evaluate_response(const char* query, const char* response) {
    EvaluationScore score;
    
    // 各スコアを計算
    score.relevance = calculate_relevance_score(query, response);
    score.coherence = calculate_coherence_score(response);
    score.informativeness = calculate_informativeness_score(response);
    
    // 総合スコアを計算
    score.overall = score.relevance * 0.5f + score.coherence * 0.2f + score.informativeness * 0.3f;
    
    return score;
}

// 回答の改善提案を生成
char* generate_improvement_suggestions(const char* query, const char* response, const EvaluationScore* score) {
    char* suggestions = (char*)malloc(MAX_RESPONSE_LEN);
    if (!suggestions) {
        return NULL;
    }
    
    suggestions[0] = '\0';
    
    // 関連性が低い場合
    if (score->relevance < 0.5f) {
        strcat(suggestions, "質問に直接関連する内容を含めてください。\n");
    }
    
    // 一貫性が低い場合
    if (score->coherence < 0.5f) {
        strcat(suggestions, "文章の構成を改善し、論理的なつながりを強化してください。\n");
    }
    
    // 情報量が少ない場合
    if (score->informativeness < 0.5f) {
        strcat(suggestions, "より具体的な情報や例を追加してください。\n");
    }
    
    // 提案がない場合
    if (strlen(suggestions) == 0) {
        strcat(suggestions, "回答は良好です。特に改善の必要はありません。");
    }
    
    return suggestions;
}

// 複数の候補回答から最適なものを選択
char* select_best_response(const char* query, char** candidate_responses, int count) {
    if (count <= 0 || !candidate_responses) {
        return NULL;
    }
    
    // 1つしかない場合はそれを返す
    if (count == 1) {
        return strdup(candidate_responses[0]);
    }
    
    // 各候補を評価
    float best_score = -1.0f;
    int best_index = 0;
    
    for (int i = 0; i < count; i++) {
        EvaluationScore score = evaluate_response(query, candidate_responses[i]);
        
        if (score.overall > best_score) {
            best_score = score.overall;
            best_index = i;
        }
    }
    
    return strdup(candidate_responses[best_index]);
}

// 評価結果をデバッグ出力
void print_evaluation_result(const char* query, const char* response, const EvaluationScore* score) {
    printf("===== 回答評価結果 =====\n");
    printf("質問: %s\n", query);
    printf("回答: %s\n", response);
    printf("関連性スコア: %.2f\n", score->relevance);
    printf("一貫性スコア: %.2f\n", score->coherence);
    printf("情報量スコア: %.2f\n", score->informativeness);
    printf("総合スコア: %.2f\n", score->overall);
    
    char* suggestions = generate_improvement_suggestions(query, response, score);
    if (suggestions) {
        printf("改善提案:\n%s\n", suggestions);
        free(suggestions);
    }
    
    printf("========================\n");
}