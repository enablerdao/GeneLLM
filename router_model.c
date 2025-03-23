#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_STR_LEN 1024
#define MAX_AGENTS 10
#define MAX_KEYWORDS 20
#define MAX_RULES 50

// エージェントの種類
typedef enum {
    EMOTION_AGENT,     // 感情分析エージェント
    LOGIC_AGENT,       // 論理推論エージェント
    TRANSLATION_AGENT, // 翻訳エージェント
    SUMMARY_AGENT,     // 要約エージェント
    CREATIVE_AGENT,    // 創造的エージェント
    UNKNOWN_AGENT      // 不明なエージェント
} AgentType;

// ルールの構造体
typedef struct {
    char keyword[MAX_STR_LEN];  // キーワード
    AgentType agent_type;       // エージェントの種類
    float weight;               // 重み
} Rule;

// ルーターモデルの構造体
typedef struct {
    Rule rules[MAX_RULES];  // ルール配列
    int rule_count;         // ルール数
} RouterModel;

// エージェントの構造体
typedef struct {
    AgentType type;                 // エージェントの種類
    char name[MAX_STR_LEN];         // エージェントの名前
    char description[MAX_STR_LEN];  // エージェントの説明
    void (*process)(const char*);   // 処理関数
} Agent;

// ルーターモデルの初期化
void init_router_model(RouterModel* model) {
    model->rule_count = 0;
}

// ルールの追加
bool add_rule(RouterModel* model, const char* keyword, AgentType agent_type, float weight) {
    if (model->rule_count >= MAX_RULES) {
        return false;  // ルールが満杯
    }
    
    strncpy(model->rules[model->rule_count].keyword, keyword, MAX_STR_LEN - 1);
    model->rules[model->rule_count].keyword[MAX_STR_LEN - 1] = '\0';
    model->rules[model->rule_count].agent_type = agent_type;
    model->rules[model->rule_count].weight = weight;
    
    model->rule_count++;
    return true;
}

// 文字列を小文字に変換
void to_lower(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// 文字列内のキーワードを検索
bool contains_keyword(const char* text, const char* keyword) {
    char text_lower[MAX_STR_LEN];
    char keyword_lower[MAX_STR_LEN];
    
    strncpy(text_lower, text, MAX_STR_LEN - 1);
    text_lower[MAX_STR_LEN - 1] = '\0';
    
    strncpy(keyword_lower, keyword, MAX_STR_LEN - 1);
    keyword_lower[MAX_STR_LEN - 1] = '\0';
    
    to_lower(text_lower);
    to_lower(keyword_lower);
    
    return strstr(text_lower, keyword_lower) != NULL;
}

// 入力文に対して最適なエージェントを決定
AgentType route_to_agent(RouterModel* model, const char* input) {
    float scores[UNKNOWN_AGENT] = {0};  // 各エージェントのスコア
    
    // 各ルールを評価
    for (int i = 0; i < model->rule_count; i++) {
        if (contains_keyword(input, model->rules[i].keyword)) {
            scores[model->rules[i].agent_type] += model->rules[i].weight;
        }
    }
    
    // 最高スコアのエージェントを選択
    AgentType best_agent = UNKNOWN_AGENT;
    float max_score = 0.0f;
    
    for (int i = 0; i < UNKNOWN_AGENT; i++) {
        if (scores[i] > max_score) {
            max_score = scores[i];
            best_agent = (AgentType)i;
        }
    }
    
    return best_agent;
}

// 感情分析エージェントの処理
void emotion_agent_process(const char* input) {
    printf("感情分析エージェントが処理しています...\n");
    printf("入力: %s\n", input);
    
    // 簡易的な感情分析
    const char* positive_words[] = {"嬉しい", "楽しい", "幸せ", "良い", "好き", "happy", "joy", "good", "like", "love"};
    const char* negative_words[] = {"悲しい", "辛い", "苦しい", "嫌い", "悪い", "sad", "pain", "bad", "hate", "dislike"};
    
    int positive_count = 0;
    int negative_count = 0;
    
    for (int i = 0; i < 10; i++) {
        if (contains_keyword(input, positive_words[i])) {
            positive_count++;
        }
        if (contains_keyword(input, negative_words[i])) {
            negative_count++;
        }
    }
    
    printf("分析結果: ");
    if (positive_count > negative_count) {
        printf("ポジティブな感情が検出されました（ポジティブ: %d, ネガティブ: %d）\n", positive_count, negative_count);
    } else if (negative_count > positive_count) {
        printf("ネガティブな感情が検出されました（ポジティブ: %d, ネガティブ: %d）\n", positive_count, negative_count);
    } else {
        printf("中立的な感情が検出されました（ポジティブ: %d, ネガティブ: %d）\n", positive_count, negative_count);
    }
}

// 論理推論エージェントの処理
void logic_agent_process(const char* input) {
    printf("論理推論エージェントが処理しています...\n");
    printf("入力: %s\n", input);
    
    // 簡易的な論理推論
    const char* if_words[] = {"もし", "if", "when", "ならば"};
    const char* then_words[] = {"then", "そうしたら", "その場合", "その時"};
    const char* because_words[] = {"なぜなら", "because", "since", "理由は"};
    
    bool has_if = false;
    bool has_then = false;
    bool has_because = false;
    
    for (int i = 0; i < 4; i++) {
        if (contains_keyword(input, if_words[i])) {
            has_if = true;
        }
        if (contains_keyword(input, then_words[i])) {
            has_then = true;
        }
        if (contains_keyword(input, because_words[i])) {
            has_because = true;
        }
    }
    
    printf("分析結果: ");
    if (has_if && has_then) {
        printf("条件付き推論が検出されました（If-Then構造）\n");
    } else if (has_because) {
        printf("因果関係の推論が検出されました（Because構造）\n");
    } else {
        printf("明確な論理構造は検出されませんでした\n");
    }
}

// 翻訳エージェントの処理
void translation_agent_process(const char* input) {
    printf("翻訳エージェントが処理しています...\n");
    printf("入力: %s\n", input);
    
    // 簡易的な翻訳（実際の翻訳ではなく、単語置換のデモ）
    const char* jp_words[] = {"猫", "犬", "鳥", "食べる", "飲む", "見る", "嬉しい", "悲しい"};
    const char* en_words[] = {"cat", "dog", "bird", "eat", "drink", "see", "happy", "sad"};
    
    char result[MAX_STR_LEN];
    strncpy(result, input, MAX_STR_LEN - 1);
    result[MAX_STR_LEN - 1] = '\0';
    
    // 日本語から英語への簡易変換
    for (int i = 0; i < 8; i++) {
        char* pos = strstr(result, jp_words[i]);
        if (pos) {
            char temp[MAX_STR_LEN];
            int prefix_len = pos - result;
            strncpy(temp, result, prefix_len);
            temp[prefix_len] = '\0';
            strcat(temp, en_words[i]);
            strcat(temp, pos + strlen(jp_words[i]));
            strcpy(result, temp);
        }
    }
    
    printf("翻訳結果（簡易デモ）: %s\n", result);
}

// 要約エージェントの処理
void summary_agent_process(const char* input) {
    printf("要約エージェントが処理しています...\n");
    printf("入力: %s\n", input);
    
    // 簡易的な要約（最初の30文字と最後の30文字を抽出）
    int len = strlen(input);
    char summary[MAX_STR_LEN];
    
    if (len <= 60) {
        strcpy(summary, input);
    } else {
        strncpy(summary, input, 30);
        summary[30] = '\0';
        strcat(summary, "...");
        strcat(summary, input + len - 30);
    }
    
    printf("要約結果: %s\n", summary);
}

// 創造的エージェントの処理
void creative_agent_process(const char* input) {
    printf("創造的エージェントが処理しています...\n");
    printf("入力: %s\n", input);
    
    // 簡易的な創造的処理（入力を逆順にする）
    char reversed[MAX_STR_LEN];
    int len = strlen(input);
    
    for (int i = 0; i < len; i++) {
        reversed[i] = input[len - 1 - i];
    }
    reversed[len] = '\0';
    
    printf("創造的出力（逆順）: %s\n", reversed);
}

// エージェントの初期化
void init_agents(Agent agents[]) {
    // 感情分析エージェント
    agents[0].type = EMOTION_AGENT;
    strcpy(agents[0].name, "感情分析エージェント");
    strcpy(agents[0].description, "テキストから感情を分析します");
    agents[0].process = emotion_agent_process;
    
    // 論理推論エージェント
    agents[1].type = LOGIC_AGENT;
    strcpy(agents[1].name, "論理推論エージェント");
    strcpy(agents[1].description, "論理的な推論を行います");
    agents[1].process = logic_agent_process;
    
    // 翻訳エージェント
    agents[2].type = TRANSLATION_AGENT;
    strcpy(agents[2].name, "翻訳エージェント");
    strcpy(agents[2].description, "テキストを翻訳します");
    agents[2].process = translation_agent_process;
    
    // 要約エージェント
    agents[3].type = SUMMARY_AGENT;
    strcpy(agents[3].name, "要約エージェント");
    strcpy(agents[3].description, "テキストを要約します");
    agents[3].process = summary_agent_process;
    
    // 創造的エージェント
    agents[4].type = CREATIVE_AGENT;
    strcpy(agents[4].name, "創造的エージェント");
    strcpy(agents[4].description, "創造的な処理を行います");
    agents[4].process = creative_agent_process;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("使用方法: %s \"処理する文\"\n", argv[0]);
        return 1;
    }
    
    const char* input = argv[1];
    
    // ルーターモデルの初期化
    RouterModel model;
    init_router_model(&model);
    
    // ルールの追加
    // 感情分析エージェントのルール
    add_rule(&model, "嬉しい", EMOTION_AGENT, 0.8f);
    add_rule(&model, "悲しい", EMOTION_AGENT, 0.8f);
    add_rule(&model, "楽しい", EMOTION_AGENT, 0.7f);
    add_rule(&model, "辛い", EMOTION_AGENT, 0.7f);
    add_rule(&model, "感情", EMOTION_AGENT, 0.9f);
    add_rule(&model, "気持ち", EMOTION_AGENT, 0.6f);
    add_rule(&model, "happy", EMOTION_AGENT, 0.8f);
    add_rule(&model, "sad", EMOTION_AGENT, 0.8f);
    
    // 論理推論エージェントのルール
    add_rule(&model, "もし", LOGIC_AGENT, 0.7f);
    add_rule(&model, "ならば", LOGIC_AGENT, 0.7f);
    add_rule(&model, "なぜなら", LOGIC_AGENT, 0.8f);
    add_rule(&model, "理由", LOGIC_AGENT, 0.6f);
    add_rule(&model, "推論", LOGIC_AGENT, 0.9f);
    add_rule(&model, "論理", LOGIC_AGENT, 0.9f);
    add_rule(&model, "if", LOGIC_AGENT, 0.7f);
    add_rule(&model, "because", LOGIC_AGENT, 0.8f);
    
    // 翻訳エージェントのルール
    add_rule(&model, "翻訳", TRANSLATION_AGENT, 0.9f);
    add_rule(&model, "英語", TRANSLATION_AGENT, 0.7f);
    add_rule(&model, "日本語", TRANSLATION_AGENT, 0.7f);
    add_rule(&model, "言語", TRANSLATION_AGENT, 0.6f);
    add_rule(&model, "translate", TRANSLATION_AGENT, 0.9f);
    add_rule(&model, "english", TRANSLATION_AGENT, 0.7f);
    add_rule(&model, "japanese", TRANSLATION_AGENT, 0.7f);
    
    // 要約エージェントのルール
    add_rule(&model, "要約", SUMMARY_AGENT, 0.9f);
    add_rule(&model, "まとめ", SUMMARY_AGENT, 0.8f);
    add_rule(&model, "短く", SUMMARY_AGENT, 0.6f);
    add_rule(&model, "概要", SUMMARY_AGENT, 0.7f);
    add_rule(&model, "summary", SUMMARY_AGENT, 0.9f);
    add_rule(&model, "summarize", SUMMARY_AGENT, 0.9f);
    
    // 創造的エージェントのルール
    add_rule(&model, "創造", CREATIVE_AGENT, 0.8f);
    add_rule(&model, "アイデア", CREATIVE_AGENT, 0.7f);
    add_rule(&model, "面白い", CREATIVE_AGENT, 0.6f);
    add_rule(&model, "新しい", CREATIVE_AGENT, 0.6f);
    add_rule(&model, "creative", CREATIVE_AGENT, 0.8f);
    add_rule(&model, "idea", CREATIVE_AGENT, 0.7f);
    add_rule(&model, "interesting", CREATIVE_AGENT, 0.6f);
    
    // エージェントの初期化
    Agent agents[5];
    init_agents(agents);
    
    // 入力に対して最適なエージェントを決定
    AgentType agent_type = route_to_agent(&model, input);
    
    printf("入力: %s\n", input);
    printf("ルーティング結果: ");
    
    switch (agent_type) {
        case EMOTION_AGENT:
            printf("%s\n", agents[0].name);
            agents[0].process(input);
            break;
        case LOGIC_AGENT:
            printf("%s\n", agents[1].name);
            agents[1].process(input);
            break;
        case TRANSLATION_AGENT:
            printf("%s\n", agents[2].name);
            agents[2].process(input);
            break;
        case SUMMARY_AGENT:
            printf("%s\n", agents[3].name);
            agents[3].process(input);
            break;
        case CREATIVE_AGENT:
            printf("%s\n", agents[4].name);
            agents[4].process(input);
            break;
        default:
            printf("不明なエージェント\n");
            printf("どのエージェントにも明確にマッチしませんでした。\n");
            break;
    }
    
    return 0;
}