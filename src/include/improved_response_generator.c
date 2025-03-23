#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "improved_response_generator.h"
#include "../vector_search/vector_search_improved.h"

// グローバル変数
static ResponseGeneratorOptions g_options;
static bool g_initialized = false;

// 回答テンプレート
static const char* TEMPLATES[] = {
    "%s", // そのまま返す
    "%sです。", // 簡潔な回答
    "%sについて説明します。%s", // 説明形式
    "%sは、%sという特徴があります。", // 特徴説明
    "「%s」とは、%sのことを指します。", // 定義説明
    "%sの主な特徴は次のとおりです：%s", // リスト形式
    "%sについて、%sということが言えます。", // 考察形式
    "%sを簡単に説明すると、%sとなります。" // 簡略説明
};

static const int TEMPLATE_COUNT = sizeof(TEMPLATES) / sizeof(TEMPLATES[0]);

// 回答生成器の初期化
void init_improved_response_generator() {
    if (g_initialized) {
        return;
    }
    
    // デフォルト設定
    g_options.use_knowledge_base = true;
    g_options.use_web_search = false;
    g_options.use_evaluation = true;
    g_options.max_candidates = MAX_CANDIDATE_RESPONSES;
    g_options.min_score = MIN_ACCEPTABLE_SCORE;
    g_options.max_response_length = 1024;
    
    // 回答評価器の初期化
    init_response_evaluator();
    
    // 乱数初期化
    srand((unsigned int)time(NULL));
    
    g_initialized = true;
}

// 回答生成器の設定
void configure_response_generator(ResponseGeneratorOptions options) {
    g_options = options;
}

// 質問に対する回答を生成
char* generate_improved_response(const char* query, KnowledgeBase* kb) {
    if (!g_initialized) {
        init_improved_response_generator();
    }
    
    if (!query || !kb) {
        return strdup("質問を理解できませんでした。もう一度お試しください。");
    }
    
    // 知識ベースから関連情報を検索
    int doc_count = 0;
    KnowledgeDocument** documents = search_relevant_knowledge(kb, query, &doc_count);
    
    if (!documents || doc_count == 0) {
        return strdup("申し訳ありませんが、その質問に関する情報が見つかりませんでした。");
    }
    
    // 候補回答を生成
    int candidate_count = 0;
    char** candidates = generate_candidate_responses(query, documents, doc_count, &candidate_count);
    
    // 検索結果のメモリを解放
    free(documents);
    
    if (!candidates || candidate_count == 0) {
        return strdup("回答を生成できませんでした。別の質問をお試しください。");
    }
    
    // 最適な回答を選択して調整
    char* best_response = select_and_refine_response(query, candidates, candidate_count);
    
    // 候補回答のメモリを解放
    for (int i = 0; i < candidate_count; i++) {
        free(candidates[i]);
    }
    free(candidates);
    
    if (!best_response) {
        return strdup("回答の評価中にエラーが発生しました。");
    }
    
    return best_response;
}

// 知識ベースから関連情報を検索
KnowledgeDocument** search_relevant_knowledge(KnowledgeBase* kb, const char* query, int* count) {
    if (!kb || !query || !count) {
        *count = 0;
        return NULL;
    }
    
    // クエリをベクトル化
    float* query_vector = (float*)malloc(sizeof(float) * VECTOR_DIM);
    if (!query_vector) {
        *count = 0;
        return NULL;
    }
    
    // 簡易的なベクトル化（実際の実装ではより高度な方法を使用）
    for (int i = 0; i < VECTOR_DIM; i++) {
        query_vector[i] = 0.0f;
    }
    
    for (size_t i = 0; i < strlen(query) && i < VECTOR_DIM; i++) {
        int idx = i % VECTOR_DIM;
        query_vector[idx] += (float)query[i] / 255.0f;
    }
    
    // ベクトルを正規化
    normalize_vector_improved(query_vector);
    
    // 最も関連性の高いドキュメントを検索
    SearchResult search_result = search_nearest_cosine_improved(&kb->vector_db, query_vector, 5);
    free(query_vector);
    
    if (search_result.count == 0) {
        *count = 0;
        return NULL;
    }
    
    // 検索結果からドキュメントを取得
    KnowledgeDocument** documents = (KnowledgeDocument**)malloc(sizeof(KnowledgeDocument*) * search_result.count);
    if (!documents) {
        *count = 0;
        return NULL;
    }
    
    *count = search_result.count;
    for (int i = 0; i < search_result.count; i++) {
        documents[i] = &kb->documents[search_result.ids[i]];
    }
    
    return documents;
}

// 検索結果から回答を生成
char* generate_response_from_knowledge(const char* query, KnowledgeDocument** documents, int count) {
    if (!documents || count <= 0) {
        return NULL;
    }
    
    // 最も関連性の高いドキュメントを使用
    KnowledgeDocument* doc = documents[0];
    
    // 回答用のメモリを確保
    char* response = (char*)malloc(g_options.max_response_length);
    if (!response) {
        return NULL;
    }
    
    // 回答テンプレートを選択
    const char* template = select_response_template(query);
    
    // テンプレートに応じて回答を生成
    if (strstr(template, "%s") == template) {
        // 単純なテンプレート
        snprintf(response, g_options.max_response_length, template, doc->content);
    } else if (strstr(template, "%s") && strstr(template, "%s") != strstr(template, "%s") + 2) {
        // 2つの%sを含むテンプレート
        char title[256];
        strncpy(title, doc->title, sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0';
        
        // タイトルの拡張子を削除
        char* dot = strrchr(title, '.');
        if (dot) {
            *dot = '\0';
        }
        
        snprintf(response, g_options.max_response_length, template, title, doc->content);
    } else {
        // デフォルト
        strncpy(response, doc->content, g_options.max_response_length - 1);
        response[g_options.max_response_length - 1] = '\0';
    }
    
    return response;
}

// 複数の候補回答を生成
char** generate_candidate_responses(const char* query, KnowledgeDocument** documents, int count, int* candidate_count) {
    if (!documents || count <= 0 || !candidate_count) {
        *candidate_count = 0;
        return NULL;
    }
    
    // 生成する候補数を決定
    int num_candidates = (count < g_options.max_candidates) ? count : g_options.max_candidates;
    
    // 候補回答用のメモリを確保
    char** candidates = (char**)malloc(sizeof(char*) * num_candidates);
    if (!candidates) {
        *candidate_count = 0;
        return NULL;
    }
    
    // 各候補を生成
    int generated = 0;
    for (int i = 0; i < count && generated < num_candidates; i++) {
        // 異なるテンプレートを使用して回答を生成
        const char* template = TEMPLATES[i % TEMPLATE_COUNT];
        
        char* response = (char*)malloc(g_options.max_response_length);
        if (!response) {
            continue;
        }
        
        if (strstr(template, "%s") == template) {
            // 単純なテンプレート
            snprintf(response, g_options.max_response_length, template, documents[i]->content);
        } else if (strstr(template, "%s") && strstr(template, "%s") != strstr(template, "%s") + 2) {
            // 2つの%sを含むテンプレート
            char title[256];
            strncpy(title, documents[i]->title, sizeof(title) - 1);
            title[sizeof(title) - 1] = '\0';
            
            // タイトルの拡張子を削除
            char* dot = strrchr(title, '.');
            if (dot) {
                *dot = '\0';
            }
            
            snprintf(response, g_options.max_response_length, template, title, documents[i]->content);
        } else {
            // デフォルト
            strncpy(response, documents[i]->content, g_options.max_response_length - 1);
            response[g_options.max_response_length - 1] = '\0';
        }
        
        candidates[generated++] = response;
    }
    
    *candidate_count = generated;
    return candidates;
}

// 回答を評価して最適なものを選択
char* select_and_refine_response(const char* query, char** candidates, int count) {
    if (!candidates || count <= 0) {
        return NULL;
    }
    
    // 評価を使用しない場合は最初の候補を返す
    if (!g_options.use_evaluation) {
        return strdup(candidates[0]);
    }
    
    // 各候補を評価
    float best_score = -1.0f;
    int best_index = 0;
    
    for (int i = 0; i < count; i++) {
        EvaluationScore score = evaluate_response(query, candidates[i]);
        
        if (score.overall > best_score) {
            best_score = score.overall;
            best_index = i;
        }
    }
    
    // 最適な回答を選択
    char* best_response = strdup(candidates[best_index]);
    
    // スコアが閾値を下回る場合は回答を調整
    if (best_score < g_options.min_score) {
        char* enhanced = enhance_response_quality(query, best_response);
        if (enhanced) {
            free(best_response);
            best_response = enhanced;
        }
    }
    
    return best_response;
}

// 回答を質問に合わせて調整
char* adapt_response_to_query(const char* query, const char* response) {
    if (!query || !response) {
        return NULL;
    }
    
    // 質問の種類を判断
    bool is_what_question = strstr(query, "何") != NULL || strstr(query, "なに") != NULL;
    bool is_how_question = strstr(query, "どう") != NULL || strstr(query, "方法") != NULL;
    bool is_why_question = strstr(query, "なぜ") != NULL || strstr(query, "どうして") != NULL;
    
    // 回答用のメモリを確保
    char* adapted = (char*)malloc(strlen(response) + 100);
    if (!adapted) {
        return NULL;
    }
    
    // 質問の種類に応じて回答を調整
    if (is_what_question) {
        sprintf(adapted, "%sです。", response);
    } else if (is_how_question) {
        sprintf(adapted, "%sという方法があります。", response);
    } else if (is_why_question) {
        sprintf(adapted, "それは%sからです。", response);
    } else {
        strcpy(adapted, response);
    }
    
    return adapted;
}

// 回答の品質を向上させる
char* enhance_response_quality(const char* query, const char* response) {
    if (!query || !response) {
        return NULL;
    }
    
    // 回答が短すぎる場合は拡張
    if (strlen(response) < 20) {
        char* enhanced = (char*)malloc(g_options.max_response_length);
        if (!enhanced) {
            return NULL;
        }
        
        sprintf(enhanced, "%sについて詳しく説明すると、%sということになります。より詳細な情報が必要でしたら、お知らせください。", query, response);
        return enhanced;
    }
    
    return strdup(response);
}

// 回答テンプレートを選択
const char* select_response_template(const char* query) {
    // 質問の種類に基づいてテンプレートを選択
    if (strstr(query, "とは") != NULL || strstr(query, "意味") != NULL) {
        return TEMPLATES[4]; // 定義説明
    } else if (strstr(query, "特徴") != NULL || strstr(query, "性質") != NULL) {
        return TEMPLATES[3]; // 特徴説明
    } else if (strstr(query, "説明") != NULL) {
        return TEMPLATES[2]; // 説明形式
    } else if (strstr(query, "簡単") != NULL || strstr(query, "要約") != NULL) {
        return TEMPLATES[7]; // 簡略説明
    } else {
        // ランダムに選択
        return TEMPLATES[rand() % TEMPLATE_COUNT];
    }
}

// 回答生成器の解放
void free_improved_response_generator() {
    g_initialized = false;
}