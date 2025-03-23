#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mecab.h>
#include <locale.h>
#include <time.h>
#include <unistd.h>

// 外部LLMインターフェースのインクルード
#include "llm_interface.c"

#define MAX_WORD_LEN 64
#define MAX_TOKENS 1000
#define MAX_LINE_LEN 1024
#define MAX_TEXT_LEN 102400
#define MAX_PATTERNS 100
#define MAX_AGENTS 10
#define MAX_AGENT_NAME 32
#define MAX_RESPONSE_LEN 2048
#define MAX_TOPICS 20
#define MAX_TOPIC_NAME 32
#define MAX_API_KEY_LEN 256
#define USE_EXTERNAL_LLM 1  // 外部LLMを使用するかどうかのフラグ

// APIキーを保存する変数
char api_key[MAX_API_KEY_LEN] = "";
#define MAX_KNOWLEDGE_ENTRIES 100
#define MAX_KNOWLEDGE_TEXT 1024

// 論理推論用の定義
#define MAX_TOPICS_LOGIC 20
#define MAX_TOPIC_NAME_LOGIC 32
#define MAX_RELATED_TOPICS 5
#define MAX_INFERENCE_RULES 100
#define MAX_RULE_TEXT 256

// 概念タイプ
typedef enum {
    ENTITY,     // 主語・実体
    CONCEPT,    // 概念・動詞
    RELATION,   // 関係・結果
    ATTRIBUTE,  // 属性（形容詞など）
    TIME,       // 時間
    LOCATION,   // 場所
    QUANTITY,   // 量
    UNKNOWN     // 不明
} ConceptType;

// 形態素情報
typedef struct {
    char surface[MAX_WORD_LEN]; // 表層形
    char base[MAX_WORD_LEN];    // 基本形
    char pos[MAX_WORD_LEN];     // 品詞
    char pos_detail[MAX_WORD_LEN]; // 品詞細分類
    ConceptType type;           // 概念タイプ
} Token;

// パターン構造体
typedef struct {
    char keyword[MAX_WORD_LEN]; // キーワード
    char pos[MAX_WORD_LEN];     // 品詞
    ConceptType type;           // 概念タイプ
    float weight;               // 重み
} Pattern;

// 知識エントリ構造体
typedef struct {
    char text[MAX_KNOWLEDGE_TEXT];  // 知識テキスト
    float relevance;                // 関連度
} KnowledgeEntry;

// トピック構造体
typedef struct {
    char name[MAX_TOPIC_NAME];      // トピック名
    Pattern patterns[MAX_PATTERNS]; // パターン
    int pattern_count;              // パターン数
    KnowledgeEntry knowledge[MAX_KNOWLEDGE_ENTRIES]; // 知識ベース
    int knowledge_count;            // 知識エントリ数
} Topic;

// エージェント構造体
typedef struct {
    char name[MAX_AGENT_NAME];  // エージェント名
    Pattern patterns[MAX_PATTERNS]; // パターン
    int pattern_count;          // パターン数
    float threshold;            // しきい値
    char (*handler)(const char*, char*, Topic*, int); // ハンドラ関数
} Agent;

// トピック関連性構造体
typedef struct {
    char topic_name[MAX_TOPIC_NAME_LOGIC];
    char related_topics[MAX_RELATED_TOPICS][MAX_TOPIC_NAME_LOGIC];
    float relation_strength[MAX_RELATED_TOPICS];
    int related_count;
} TopicRelation;

// 推論ルール構造体
typedef struct {
    char condition[MAX_RULE_TEXT];
    char conclusion[MAX_RULE_TEXT];
    float confidence;
} InferenceRule;

// グローバル変数
char text_buffer[MAX_TEXT_LEN];
Token tokens[MAX_TOKENS];
int token_count = 0;
Agent agents[MAX_AGENTS];
int agent_count = 0;
Topic topics[MAX_TOPICS];
int topic_count = 0;

// 論理推論用グローバル変数
TopicRelation topic_relations[MAX_TOPICS_LOGIC];
int relation_count = 0;
InferenceRule inference_rules[MAX_INFERENCE_RULES];
int rule_count = 0;

// 関数プロトタイプ
void tokenize_text(const char* text);
void init_agents();
void init_topics();
void init_topic_relations();
void init_inference_rules();
void add_agent(const char* name, float threshold, char (*handler)(const char*, char*, Topic*, int));
void add_pattern(int agent_id, const char* keyword, const char* pos, ConceptType type, float weight);
void add_topic(const char* name);
void add_topic_pattern(int topic_id, const char* keyword, const char* pos, ConceptType type, float weight);
void add_knowledge(int topic_id, const char* text, float relevance);
void add_topic_relation(const char* topic, const char* related_topic, float strength);
void add_inference_rule(const char* condition, const char* conclusion, float confidence);
int route_text(const char* text, char* response);
float calculate_score(int agent_id, const Token* tokens, int token_count);
float calculate_topic_score(int topic_id, const Token* tokens, int token_count);
int find_best_topic(const Token* tokens, int token_count);
const char* concept_type_to_string(ConceptType type);
ConceptType string_to_concept_type(const char* str);
void load_knowledge_from_file(const char* filename, int topic_id);
bool find_related_topics(const char* topic, char related_topics[MAX_RELATED_TOPICS][MAX_TOPIC_NAME_LOGIC], float strengths[MAX_RELATED_TOPICS], int* count);
bool apply_inference_rules(const char* input, char* response);
float calculate_topic_similarity(const char* topic1, const char* topic2);

// エージェントハンドラ関数
char handle_greeting(const char* text, char* response, Topic* topics, int topic_count);
char handle_question(const char* text, char* response, Topic* topics, int topic_count);
char handle_command(const char* text, char* response, Topic* topics, int topic_count);
char handle_statement(const char* text, char* response, Topic* topics, int topic_count);
char handle_emotion(const char* text, char* response, Topic* topics, int topic_count);
char handle_fallback(const char* text, char* response, Topic* topics, int topic_count);

// MeCabを使用してテキストをトークン化する
void tokenize_text(const char* text) {
    // MeCabの初期化
    mecab_t* mecab = mecab_new2("");
    if (!mecab) {
        fprintf(stderr, "MeCabの初期化に失敗しました: %s\n", mecab_strerror(NULL));
        return;
    }
    
    // 形態素解析を実行
    const mecab_node_t* node = mecab_sparse_tonode(mecab, text);
    if (!node) {
        fprintf(stderr, "形態素解析に失敗しました: %s\n", mecab_strerror(mecab));
        mecab_destroy(mecab);
        return;
    }
    
    // 結果を処理
    token_count = 0;
    for (; node; node = node->next) {
        // EOSノードをスキップ
        if (node->stat == MECAB_EOS_NODE) {
            continue;
        }
        
        // 表層形をコピー
        char surface[MAX_WORD_LEN];
        strncpy(surface, node->surface, node->length);
        surface[node->length] = '\0';
        
        // 空白をスキップ
        if (surface[0] == ' ' || surface[0] == '\t' || surface[0] == '\n' || surface[0] == '\r') {
            continue;
        }
        
        // 素性情報を解析
        char feature[MAX_LINE_LEN];
        strncpy(feature, node->feature, MAX_LINE_LEN - 1);
        feature[MAX_LINE_LEN - 1] = '\0';
        
        // 素性情報をカンマで分割
        char feature_copy[MAX_LINE_LEN];
        strcpy(feature_copy, feature);
        
        char* pos = strtok(feature_copy, ",");
        char* pos_detail1 = pos ? strtok(NULL, ",") : NULL;
        char* pos_detail2 = pos_detail1 ? strtok(NULL, ",") : NULL;
        char* pos_detail3 = pos_detail2 ? strtok(NULL, ",") : NULL;
        char* conjugation1 = pos_detail3 ? strtok(NULL, ",") : NULL;
        char* conjugation2 = conjugation1 ? strtok(NULL, ",") : NULL;
        char* base_form = conjugation2 ? strtok(NULL, ",") : NULL;
        
        // トークンに追加
        if (token_count < MAX_TOKENS) {
            strncpy(tokens[token_count].surface, surface, MAX_WORD_LEN - 1);
            tokens[token_count].surface[MAX_WORD_LEN - 1] = '\0';
            
            if (base_form && strcmp(base_form, "*") != 0) {
                strncpy(tokens[token_count].base, base_form, MAX_WORD_LEN - 1);
            } else {
                strncpy(tokens[token_count].base, surface, MAX_WORD_LEN - 1);
            }
            tokens[token_count].base[MAX_WORD_LEN - 1] = '\0';
            
            if (pos) {
                strncpy(tokens[token_count].pos, pos, MAX_WORD_LEN - 1);
            } else {
                strcpy(tokens[token_count].pos, "未知語");
            }
            tokens[token_count].pos[MAX_WORD_LEN - 1] = '\0';
            
            if (pos_detail1) {
                strncpy(tokens[token_count].pos_detail, pos_detail1, MAX_WORD_LEN - 1);
            } else {
                strcpy(tokens[token_count].pos_detail, "*");
            }
            tokens[token_count].pos_detail[MAX_WORD_LEN - 1] = '\0';
            
            // 概念タイプを設定（初期値は不明）
            tokens[token_count].type = UNKNOWN;
            
            // 品詞に基づいて概念タイプを設定
            if (strcmp(tokens[token_count].pos, "名詞") == 0) {
                if (strcmp(tokens[token_count].pos_detail, "固有名詞") == 0) {
                    tokens[token_count].type = ENTITY;
                } else if (strcmp(tokens[token_count].pos_detail, "時詞") == 0 || 
                           strstr(tokens[token_count].surface, "年") != NULL || 
                           strstr(tokens[token_count].surface, "月") != NULL || 
                           strstr(tokens[token_count].surface, "日") != NULL || 
                           strstr(tokens[token_count].surface, "時") != NULL) {
                    tokens[token_count].type = TIME;
                } else if (strcmp(tokens[token_count].pos_detail, "場所") == 0 || 
                           strstr(tokens[token_count].surface, "市") != NULL || 
                           strstr(tokens[token_count].surface, "県") != NULL || 
                           strstr(tokens[token_count].surface, "町") != NULL) {
                    tokens[token_count].type = LOCATION;
                } else if (strcmp(tokens[token_count].pos_detail, "数") == 0) {
                    tokens[token_count].type = QUANTITY;
                } else {
                    tokens[token_count].type = ENTITY;
                }
            } else if (strcmp(tokens[token_count].pos, "動詞") == 0) {
                tokens[token_count].type = CONCEPT;
            } else if (strcmp(tokens[token_count].pos, "形容詞") == 0) {
                tokens[token_count].type = ATTRIBUTE;
            }
            
            token_count++;
        }
    }
    
    // MeCabを解放
    mecab_destroy(mecab);
}

// エージェントを初期化
void init_agents() {
    agent_count = 0;
    
    // 挨拶エージェント
    add_agent("greeting", 0.1f, handle_greeting);
    add_pattern(0, "こんにちは", "感動詞", UNKNOWN, 1.0f);
    add_pattern(0, "おはよう", "感動詞", UNKNOWN, 1.0f);
    add_pattern(0, "こんばんは", "感動詞", UNKNOWN, 1.0f);
    add_pattern(0, "さようなら", "感動詞", UNKNOWN, 1.0f);
    add_pattern(0, "ありがとう", "感動詞", UNKNOWN, 0.8f);
    add_pattern(0, "よろしく", "感動詞", UNKNOWN, 0.8f);
    add_pattern(0, "こんにちは", "名詞", ENTITY, 1.0f);
    add_pattern(0, "おはよう", "名詞", ENTITY, 1.0f);
    add_pattern(0, "こんばんは", "名詞", ENTITY, 1.0f);
    add_pattern(0, "元気", "名詞", ENTITY, 0.8f);
    
    // 質問エージェント
    add_agent("question", 0.1f, handle_question);
    add_pattern(1, "何", "代名詞", UNKNOWN, 0.9f);
    add_pattern(1, "誰", "代名詞", UNKNOWN, 0.9f);
    add_pattern(1, "どこ", "代名詞", UNKNOWN, 0.9f);
    add_pattern(1, "いつ", "代名詞", UNKNOWN, 0.9f);
    add_pattern(1, "なぜ", "副詞", UNKNOWN, 0.9f);
    add_pattern(1, "どうして", "副詞", UNKNOWN, 0.9f);
    add_pattern(1, "か", "助詞", UNKNOWN, 0.7f);
    add_pattern(1, "ですか", "助動詞", UNKNOWN, 0.8f);
    add_pattern(1, "？", "記号", UNKNOWN, 1.0f);
    add_pattern(1, "教えて", "動詞", CONCEPT, 0.8f);
    add_pattern(1, "について", "助詞", UNKNOWN, 0.7f);
    add_pattern(1, "とは", "助詞", UNKNOWN, 0.7f);
    add_pattern(1, "どう", "副詞", UNKNOWN, 0.8f);
    add_pattern(1, "どの", "連体詞", UNKNOWN, 0.8f);
    add_pattern(1, "どんな", "連体詞", UNKNOWN, 0.8f);
    add_pattern(1, "何", "名詞", ENTITY, 0.9f);
    add_pattern(1, "教えて", "名詞", ENTITY, 0.8f);
    add_pattern(1, "説明", "名詞", ENTITY, 0.8f);
    
    // 命令エージェント
    add_agent("command", 0.3f, handle_command);
    add_pattern(2, "して", "動詞", CONCEPT, 0.8f);
    add_pattern(2, "ください", "動詞", CONCEPT, 0.9f);
    add_pattern(2, "なさい", "動詞", CONCEPT, 0.9f);
    add_pattern(2, "下さい", "動詞", CONCEPT, 0.9f);
    add_pattern(2, "頂戴", "動詞", CONCEPT, 0.8f);
    add_pattern(2, "命令", "名詞", ENTITY, 0.7f);
    add_pattern(2, "指示", "名詞", ENTITY, 0.7f);
    add_pattern(2, "教えて", "動詞", CONCEPT, 0.8f);
    add_pattern(2, "説明して", "動詞", CONCEPT, 0.8f);
    add_pattern(2, "解説して", "動詞", CONCEPT, 0.8f);
    
    // 陳述エージェント
    add_agent("statement", 0.3f, handle_statement);
    add_pattern(3, "です", "助動詞", UNKNOWN, 0.6f);
    add_pattern(3, "ます", "助動詞", UNKNOWN, 0.6f);
    add_pattern(3, "だ", "助動詞", UNKNOWN, 0.6f);
    add_pattern(3, "である", "助動詞", UNKNOWN, 0.7f);
    add_pattern(3, "。", "記号", UNKNOWN, 0.5f);
    add_pattern(3, "思います", "動詞", CONCEPT, 0.7f);
    add_pattern(3, "考えます", "動詞", CONCEPT, 0.7f);
    add_pattern(3, "感じます", "動詞", CONCEPT, 0.7f);
    
    // 感情エージェント
    add_agent("emotion", 0.3f, handle_emotion);
    add_pattern(4, "嬉しい", "形容詞", ATTRIBUTE, 1.0f);
    add_pattern(4, "悲しい", "形容詞", ATTRIBUTE, 1.0f);
    add_pattern(4, "楽しい", "形容詞", ATTRIBUTE, 1.0f);
    add_pattern(4, "怒り", "名詞", ENTITY, 0.9f);
    add_pattern(4, "不安", "名詞", ENTITY, 0.9f);
    add_pattern(4, "恐怖", "名詞", ENTITY, 0.9f);
    add_pattern(4, "愛", "名詞", ENTITY, 0.8f);
    add_pattern(4, "好き", "形容詞", ATTRIBUTE, 0.9f);
    add_pattern(4, "嫌い", "形容詞", ATTRIBUTE, 0.9f);
    add_pattern(4, "！", "記号", UNKNOWN, 0.7f);
    add_pattern(4, "幸せ", "名詞", ENTITY, 0.9f);
    add_pattern(4, "幸福", "名詞", ENTITY, 0.9f);
    add_pattern(4, "喜び", "名詞", ENTITY, 0.9f);
    
    // フォールバックエージェント
    add_agent("fallback", 0.0f, handle_fallback);
    
    printf("%d個のエージェントを初期化しました\n", agent_count);
}

// トピックを初期化
void init_topics() {
    topic_count = 0;
    
    // 幸せトピック
    add_topic("happiness");
    add_topic_pattern(0, "幸せ", "名詞", ENTITY, 1.0f);
    add_topic_pattern(0, "幸福", "名詞", ENTITY, 1.0f);
    add_topic_pattern(0, "喜び", "名詞", ENTITY, 0.8f);
    add_topic_pattern(0, "楽しい", "形容詞", ATTRIBUTE, 0.8f);
    add_topic_pattern(0, "嬉しい", "形容詞", ATTRIBUTE, 0.8f);
    add_topic_pattern(0, "コツ", "名詞", ENTITY, 0.7f);
    add_topic_pattern(0, "方法", "名詞", ENTITY, 0.7f);
    add_topic_pattern(0, "秘訣", "名詞", ENTITY, 0.7f);
    
    // 愛トピック
    add_topic("love");
    add_topic_pattern(1, "愛", "名詞", ENTITY, 1.0f);
    add_topic_pattern(1, "恋愛", "名詞", ENTITY, 0.9f);
    add_topic_pattern(1, "恋", "名詞", ENTITY, 0.9f);
    add_topic_pattern(1, "好き", "形容詞", ATTRIBUTE, 0.8f);
    add_topic_pattern(1, "愛情", "名詞", ENTITY, 0.8f);
    add_topic_pattern(1, "恋人", "名詞", ENTITY, 0.7f);
    add_topic_pattern(1, "結婚", "名詞", ENTITY, 0.7f);
    add_topic_pattern(1, "パートナー", "名詞", ENTITY, 0.7f);
    
    // 健康トピック
    add_topic("health");
    add_topic_pattern(2, "健康", "名詞", ENTITY, 1.0f);
    add_topic_pattern(2, "運動", "名詞", ENTITY, 0.9f);
    add_topic_pattern(2, "食事", "名詞", ENTITY, 0.9f);
    add_topic_pattern(2, "睡眠", "名詞", ENTITY, 0.9f);
    add_topic_pattern(2, "ストレス", "名詞", ENTITY, 0.8f);
    add_topic_pattern(2, "病気", "名詞", ENTITY, 0.8f);
    add_topic_pattern(2, "予防", "名詞", ENTITY, 0.8f);
    add_topic_pattern(2, "体", "名詞", ENTITY, 0.7f);
    add_topic_pattern(2, "医者", "名詞", ENTITY, 0.7f);
    add_topic_pattern(2, "診断", "名詞", ENTITY, 0.7f);
    
    // 仕事トピック
    add_topic("work");
    add_topic_pattern(3, "仕事", "名詞", ENTITY, 1.0f);
    add_topic_pattern(3, "働く", "動詞", CONCEPT, 0.9f);
    add_topic_pattern(3, "キャリア", "名詞", ENTITY, 0.9f);
    add_topic_pattern(3, "職場", "名詞", ENTITY, 0.9f);
    add_topic_pattern(3, "効率", "名詞", ENTITY, 0.8f);
    add_topic_pattern(3, "生産性", "名詞", ENTITY, 0.8f);
    add_topic_pattern(3, "タスク", "名詞", ENTITY, 0.8f);
    add_topic_pattern(3, "目標", "名詞", ENTITY, 0.7f);
    add_topic_pattern(3, "成功", "名詞", ENTITY, 0.7f);
    add_topic_pattern(3, "スキル", "名詞", ENTITY, 0.7f);
    
    // 学習トピック
    add_topic("learning");
    add_topic_pattern(4, "学習", "名詞", ENTITY, 1.0f);
    add_topic_pattern(4, "勉強", "名詞", ENTITY, 1.0f);
    add_topic_pattern(4, "学ぶ", "動詞", CONCEPT, 0.9f);
    add_topic_pattern(4, "記憶", "名詞", ENTITY, 0.9f);
    add_topic_pattern(4, "理解", "名詞", ENTITY, 0.8f);
    add_topic_pattern(4, "知識", "名詞", ENTITY, 0.8f);
    add_topic_pattern(4, "教育", "名詞", ENTITY, 0.8f);
    add_topic_pattern(4, "効率", "名詞", ENTITY, 0.7f);
    add_topic_pattern(4, "集中", "名詞", ENTITY, 0.7f);
    add_topic_pattern(4, "復習", "名詞", ENTITY, 0.7f);
    
    // 数学トピック
    add_topic("math");
    add_topic_pattern(5, "数学", "名詞", ENTITY, 1.0f);
    add_topic_pattern(5, "数", "名詞", ENTITY, 0.9f);
    add_topic_pattern(5, "計算", "名詞", ENTITY, 0.9f);
    add_topic_pattern(5, "定理", "名詞", ENTITY, 0.9f);
    add_topic_pattern(5, "方程式", "名詞", ENTITY, 0.8f);
    add_topic_pattern(5, "関数", "名詞", ENTITY, 0.8f);
    add_topic_pattern(5, "幾何学", "名詞", ENTITY, 0.8f);
    add_topic_pattern(5, "代数", "名詞", ENTITY, 0.8f);
    add_topic_pattern(5, "統計", "名詞", ENTITY, 0.7f);
    add_topic_pattern(5, "確率", "名詞", ENTITY, 0.7f);
    add_topic_pattern(5, "証明", "名詞", ENTITY, 0.7f);
    
    // 物理トピック
    add_topic("physics");
    add_topic_pattern(6, "物理", "名詞", ENTITY, 1.0f);
    add_topic_pattern(6, "物理学", "名詞", ENTITY, 1.0f);
    add_topic_pattern(6, "力", "名詞", ENTITY, 0.9f);
    add_topic_pattern(6, "運動", "名詞", ENTITY, 0.9f);
    add_topic_pattern(6, "エネルギー", "名詞", ENTITY, 0.9f);
    add_topic_pattern(6, "質量", "名詞", ENTITY, 0.8f);
    add_topic_pattern(6, "光", "名詞", ENTITY, 0.8f);
    add_topic_pattern(6, "電気", "名詞", ENTITY, 0.8f);
    add_topic_pattern(6, "磁気", "名詞", ENTITY, 0.8f);
    add_topic_pattern(6, "原子", "名詞", ENTITY, 0.7f);
    add_topic_pattern(6, "素粒子", "名詞", ENTITY, 0.7f);
    add_topic_pattern(6, "宇宙", "名詞", ENTITY, 0.7f);
    
    // プログラミングトピック
    add_topic("programming");
    add_topic_pattern(7, "プログラミング", "名詞", ENTITY, 1.0f);
    add_topic_pattern(7, "プログラム", "名詞", ENTITY, 1.0f);
    add_topic_pattern(7, "コード", "名詞", ENTITY, 0.9f);
    add_topic_pattern(7, "開発", "名詞", ENTITY, 0.9f);
    add_topic_pattern(7, "言語", "名詞", ENTITY, 0.9f);
    add_topic_pattern(7, "変数", "名詞", ENTITY, 0.8f);
    add_topic_pattern(7, "関数", "名詞", ENTITY, 0.8f);
    add_topic_pattern(7, "オブジェクト", "名詞", ENTITY, 0.8f);
    add_topic_pattern(7, "クラス", "名詞", ENTITY, 0.8f);
    add_topic_pattern(7, "アルゴリズム", "名詞", ENTITY, 0.7f);
    add_topic_pattern(7, "デバッグ", "名詞", ENTITY, 0.7f);
    add_topic_pattern(7, "コンパイル", "名詞", ENTITY, 0.7f);
    
    // ギタートピック
    add_topic("guitar");
    add_topic_pattern(8, "ギター", "名詞", ENTITY, 1.0f);
    add_topic_pattern(8, "弦", "名詞", ENTITY, 0.9f);
    add_topic_pattern(8, "コード", "名詞", ENTITY, 0.9f);
    add_topic_pattern(8, "演奏", "名詞", ENTITY, 0.9f);
    add_topic_pattern(8, "音", "名詞", ENTITY, 0.8f);
    add_topic_pattern(8, "弾く", "動詞", CONCEPT, 0.8f);
    add_topic_pattern(8, "ピック", "名詞", ENTITY, 0.8f);
    add_topic_pattern(8, "フレット", "名詞", ENTITY, 0.8f);
    add_topic_pattern(8, "チューニング", "名詞", ENTITY, 0.7f);
    add_topic_pattern(8, "アコースティック", "名詞", ENTITY, 0.7f);
    add_topic_pattern(8, "エレクトリック", "名詞", ENTITY, 0.7f);
    
    // サバイバルトピック
    add_topic("survival");
    add_topic_pattern(9, "サバイバル", "名詞", ENTITY, 1.0f);
    add_topic_pattern(9, "生存", "名詞", ENTITY, 1.0f);
    add_topic_pattern(9, "野外", "名詞", ENTITY, 0.9f);
    add_topic_pattern(9, "緊急", "名詞", ENTITY, 0.9f);
    add_topic_pattern(9, "災害", "名詞", ENTITY, 0.9f);
    add_topic_pattern(9, "水", "名詞", ENTITY, 0.8f);
    add_topic_pattern(9, "食料", "名詞", ENTITY, 0.8f);
    add_topic_pattern(9, "火", "名詞", ENTITY, 0.8f);
    add_topic_pattern(9, "シェルター", "名詞", ENTITY, 0.8f);
    add_topic_pattern(9, "応急処置", "名詞", ENTITY, 0.7f);
    add_topic_pattern(9, "道具", "名詞", ENTITY, 0.7f);
    add_topic_pattern(9, "ナビゲーション", "名詞", ENTITY, 0.7f);
    
    // 量子コンピュータトピック
    add_topic("quantum");
    add_topic_pattern(10, "量子", "名詞", ENTITY, 1.0f);
    add_topic_pattern(10, "量子コンピュータ", "名詞", ENTITY, 1.0f);
    add_topic_pattern(10, "キュービット", "名詞", ENTITY, 0.9f);
    add_topic_pattern(10, "量子ビット", "名詞", ENTITY, 0.9f);
    add_topic_pattern(10, "重ね合わせ", "名詞", ENTITY, 0.9f);
    add_topic_pattern(10, "量子もつれ", "名詞", ENTITY, 0.8f);
    add_topic_pattern(10, "量子ゲート", "名詞", ENTITY, 0.8f);
    add_topic_pattern(10, "量子回路", "名詞", ENTITY, 0.8f);
    add_topic_pattern(10, "量子アルゴリズム", "名詞", ENTITY, 0.8f);
    add_topic_pattern(10, "ショア", "名詞", ENTITY, 0.7f);
    add_topic_pattern(10, "グローバー", "名詞", ENTITY, 0.7f);
    add_topic_pattern(10, "量子優位性", "名詞", ENTITY, 0.7f);
    
    // 料理トピック
    add_topic("cooking");
    add_topic_pattern(11, "料理", "名詞", ENTITY, 1.0f);
    add_topic_pattern(11, "調理", "名詞", ENTITY, 1.0f);
    add_topic_pattern(11, "食事", "名詞", ENTITY, 0.9f);
    add_topic_pattern(11, "レシピ", "名詞", ENTITY, 0.9f);
    add_topic_pattern(11, "食材", "名詞", ENTITY, 0.9f);
    add_topic_pattern(11, "和食", "名詞", ENTITY, 0.8f);
    add_topic_pattern(11, "洋食", "名詞", ENTITY, 0.8f);
    add_topic_pattern(11, "中華", "名詞", ENTITY, 0.8f);
    add_topic_pattern(11, "調味料", "名詞", ENTITY, 0.8f);
    add_topic_pattern(11, "栄養", "名詞", ENTITY, 0.7f);
    add_topic_pattern(11, "味", "名詞", ENTITY, 0.7f);
    add_topic_pattern(11, "作り方", "名詞", ENTITY, 0.7f);
    
    // フィットネストピック
    add_topic("fitness");
    add_topic_pattern(12, "フィットネス", "名詞", ENTITY, 1.0f);
    add_topic_pattern(12, "運動", "名詞", ENTITY, 1.0f);
    add_topic_pattern(12, "トレーニング", "名詞", ENTITY, 0.9f);
    add_topic_pattern(12, "筋トレ", "名詞", ENTITY, 0.9f);
    add_topic_pattern(12, "有酸素運動", "名詞", ENTITY, 0.9f);
    add_topic_pattern(12, "ストレッチ", "名詞", ENTITY, 0.8f);
    add_topic_pattern(12, "ヨガ", "名詞", ENTITY, 0.8f);
    add_topic_pattern(12, "ピラティス", "名詞", ENTITY, 0.8f);
    add_topic_pattern(12, "体脂肪", "名詞", ENTITY, 0.8f);
    add_topic_pattern(12, "筋肉", "名詞", ENTITY, 0.7f);
    add_topic_pattern(12, "ダイエット", "名詞", ENTITY, 0.7f);
    add_topic_pattern(12, "健康", "名詞", ENTITY, 0.7f);
    
    // 旅行トピック
    add_topic("travel");
    add_topic_pattern(13, "旅行", "名詞", ENTITY, 1.0f);
    add_topic_pattern(13, "観光", "名詞", ENTITY, 1.0f);
    add_topic_pattern(13, "旅", "名詞", ENTITY, 0.9f);
    add_topic_pattern(13, "ツアー", "名詞", ENTITY, 0.9f);
    add_topic_pattern(13, "ホテル", "名詞", ENTITY, 0.9f);
    add_topic_pattern(13, "航空券", "名詞", ENTITY, 0.8f);
    add_topic_pattern(13, "パスポート", "名詞", ENTITY, 0.8f);
    add_topic_pattern(13, "ビザ", "名詞", ENTITY, 0.8f);
    add_topic_pattern(13, "観光地", "名詞", ENTITY, 0.8f);
    add_topic_pattern(13, "海外", "名詞", ENTITY, 0.7f);
    add_topic_pattern(13, "国内", "名詞", ENTITY, 0.7f);
    add_topic_pattern(13, "旅程", "名詞", ENTITY, 0.7f);
    
    // 金融トピック
    add_topic("finance");
    add_topic_pattern(14, "金融", "名詞", ENTITY, 1.0f);
    add_topic_pattern(14, "投資", "名詞", ENTITY, 1.0f);
    add_topic_pattern(14, "お金", "名詞", ENTITY, 0.9f);
    add_topic_pattern(14, "資産", "名詞", ENTITY, 0.9f);
    add_topic_pattern(14, "株式", "名詞", ENTITY, 0.9f);
    add_topic_pattern(14, "債券", "名詞", ENTITY, 0.8f);
    add_topic_pattern(14, "投資信託", "名詞", ENTITY, 0.8f);
    add_topic_pattern(14, "貯金", "名詞", ENTITY, 0.8f);
    add_topic_pattern(14, "税金", "名詞", ENTITY, 0.8f);
    add_topic_pattern(14, "リスク", "名詞", ENTITY, 0.7f);
    add_topic_pattern(14, "リターン", "名詞", ENTITY, 0.7f);
    add_topic_pattern(14, "複利", "名詞", ENTITY, 0.7f);
    
    // エンターテイメントトピック
    add_topic("entertainment");
    add_topic_pattern(15, "エンターテイメント", "名詞", ENTITY, 1.0f);
    add_topic_pattern(15, "映画", "名詞", ENTITY, 1.0f);
    add_topic_pattern(15, "音楽", "名詞", ENTITY, 1.0f);
    add_topic_pattern(15, "ドラマ", "名詞", ENTITY, 0.9f);
    add_topic_pattern(15, "アニメ", "名詞", ENTITY, 0.9f);
    add_topic_pattern(15, "ゲーム", "名詞", ENTITY, 0.9f);
    add_topic_pattern(15, "アーティスト", "名詞", ENTITY, 0.8f);
    add_topic_pattern(15, "俳優", "名詞", ENTITY, 0.8f);
    add_topic_pattern(15, "監督", "名詞", ENTITY, 0.8f);
    add_topic_pattern(15, "ジャンル", "名詞", ENTITY, 0.8f);
    add_topic_pattern(15, "ストリーミング", "名詞", ENTITY, 0.7f);
    add_topic_pattern(15, "コンサート", "名詞", ENTITY, 0.7f);
    
    // 知識ファイルを読み込む
    load_knowledge_from_file("happiness_corpus.txt", 0);
    load_knowledge_from_file("love_corpus.txt", 1);
    load_knowledge_from_file("health_corpus.txt", 2);
    load_knowledge_from_file("work_corpus.txt", 3);
    load_knowledge_from_file("learning_corpus.txt", 4);
    load_knowledge_from_file("math_corpus.txt", 5);
    load_knowledge_from_file("physics_corpus.txt", 6);
    load_knowledge_from_file("programming_corpus.txt", 7);
    load_knowledge_from_file("guitar_corpus.txt", 8);
    load_knowledge_from_file("survival_corpus.txt", 9);
    load_knowledge_from_file("quantum_corpus.txt", 10);
    load_knowledge_from_file("cooking_corpus.txt", 11);
    load_knowledge_from_file("fitness_corpus.txt", 12);
    load_knowledge_from_file("travel_corpus.txt", 13);
    load_knowledge_from_file("finance_corpus.txt", 14);
    load_knowledge_from_file("entertainment_corpus.txt", 15);
    
    printf("%d個のトピックを初期化しました\n", topic_count);
}

// ファイルから知識を読み込む
void load_knowledge_from_file(const char* filename, int topic_id) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "知識ファイルを開けませんでした: %s\n", filename);
        return;
    }
    
    char line[MAX_KNOWLEDGE_TEXT];
    while (fgets(line, sizeof(line), file)) {
        // 改行を削除
        size_t len = strlen(line);
        if (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
        }
        
        // 空行をスキップ
        if (strlen(line) == 0) {
            continue;
        }
        
        // 知識を追加
        add_knowledge(topic_id, line, 1.0f);
    }
    
    fclose(file);
    printf("知識を読み込みました: %s (%d個)\n", 
           filename, topics[topic_id].knowledge_count);
}

// エージェントを追加
void add_agent(const char* name, float threshold, char (*handler)(const char*, char*, Topic*, int)) {
    if (agent_count >= MAX_AGENTS) {
        fprintf(stderr, "警告: エージェント数が上限に達しました\n");
        return;
    }
    
    strncpy(agents[agent_count].name, name, MAX_AGENT_NAME - 1);
    agents[agent_count].name[MAX_AGENT_NAME - 1] = '\0';
    agents[agent_count].pattern_count = 0;
    agents[agent_count].threshold = threshold;
    agents[agent_count].handler = handler;
    
    agent_count++;
}

// トピックを追加
void add_topic(const char* name) {
    if (topic_count >= MAX_TOPICS) {
        fprintf(stderr, "警告: トピック数が上限に達しました\n");
        return;
    }
    
    strncpy(topics[topic_count].name, name, MAX_TOPIC_NAME - 1);
    topics[topic_count].name[MAX_TOPIC_NAME - 1] = '\0';
    topics[topic_count].pattern_count = 0;
    topics[topic_count].knowledge_count = 0;
    
    topic_count++;
}

// パターンを追加
void add_pattern(int agent_id, const char* keyword, const char* pos, ConceptType type, float weight) {
    if (agent_id < 0 || agent_id >= agent_count) {
        fprintf(stderr, "警告: 無効なエージェントID: %d\n", agent_id);
        return;
    }
    
    if (agents[agent_id].pattern_count >= MAX_PATTERNS) {
        fprintf(stderr, "警告: パターン数が上限に達しました（エージェント: %s）\n", agents[agent_id].name);
        return;
    }
    
    int pattern_id = agents[agent_id].pattern_count;
    
    strncpy(agents[agent_id].patterns[pattern_id].keyword, keyword, MAX_WORD_LEN - 1);
    agents[agent_id].patterns[pattern_id].keyword[MAX_WORD_LEN - 1] = '\0';
    
    strncpy(agents[agent_id].patterns[pattern_id].pos, pos, MAX_WORD_LEN - 1);
    agents[agent_id].patterns[pattern_id].pos[MAX_WORD_LEN - 1] = '\0';
    
    agents[agent_id].patterns[pattern_id].type = type;
    agents[agent_id].patterns[pattern_id].weight = weight;
    
    agents[agent_id].pattern_count++;
}

// トピックにパターンを追加
void add_topic_pattern(int topic_id, const char* keyword, const char* pos, ConceptType type, float weight) {
    if (topic_id < 0 || topic_id >= topic_count) {
        fprintf(stderr, "警告: 無効なトピックID: %d\n", topic_id);
        return;
    }
    
    if (topics[topic_id].pattern_count >= MAX_PATTERNS) {
        fprintf(stderr, "警告: パターン数が上限に達しました（トピック: %s）\n", topics[topic_id].name);
        return;
    }
    
    int pattern_id = topics[topic_id].pattern_count;
    
    strncpy(topics[topic_id].patterns[pattern_id].keyword, keyword, MAX_WORD_LEN - 1);
    topics[topic_id].patterns[pattern_id].keyword[MAX_WORD_LEN - 1] = '\0';
    
    strncpy(topics[topic_id].patterns[pattern_id].pos, pos, MAX_WORD_LEN - 1);
    topics[topic_id].patterns[pattern_id].pos[MAX_WORD_LEN - 1] = '\0';
    
    topics[topic_id].patterns[pattern_id].type = type;
    topics[topic_id].patterns[pattern_id].weight = weight;
    
    topics[topic_id].pattern_count++;
}

// 知識を追加
void add_knowledge(int topic_id, const char* text, float relevance) {
    if (topic_id < 0 || topic_id >= topic_count) {
        fprintf(stderr, "警告: 無効なトピックID: %d\n", topic_id);
        return;
    }
    
    if (topics[topic_id].knowledge_count >= MAX_KNOWLEDGE_ENTRIES) {
        fprintf(stderr, "警告: 知識エントリ数が上限に達しました（トピック: %s）\n", topics[topic_id].name);
        return;
    }
    
    int knowledge_id = topics[topic_id].knowledge_count;
    
    strncpy(topics[topic_id].knowledge[knowledge_id].text, text, MAX_KNOWLEDGE_TEXT - 1);
    topics[topic_id].knowledge[knowledge_id].text[MAX_KNOWLEDGE_TEXT - 1] = '\0';
    topics[topic_id].knowledge[knowledge_id].relevance = relevance;
    
    topics[topic_id].knowledge_count++;
}

// テキストをルーティング
int route_text(const char* text, char* response) {
    // テキストをトークン化
    tokenize_text(text);
    
    // 各エージェントのスコアを計算
    float max_score = -1.0f;
    int best_agent = -1;
    
    for (int i = 0; i < agent_count; i++) {
        float score = calculate_score(i, tokens, token_count);
        
        printf("エージェント '%s' のスコア: %.4f (しきい値: %.4f)\n", 
               agents[i].name, score, agents[i].threshold);
        
        if (score >= agents[i].threshold && score > max_score) {
            max_score = score;
            best_agent = i;
        }
    }
    
    // 最適なエージェントがない場合はフォールバック
    if (best_agent == -1) {
        best_agent = agent_count - 1;  // フォールバックエージェント
    }
    
    printf("選択されたエージェント: %s (スコア: %.4f)\n", 
           agents[best_agent].name, max_score);
    
    // 最適なトピックを見つける
    int best_topic = find_best_topic(tokens, token_count);
    
    // エージェントのハンドラを呼び出す
    char result = agents[best_agent].handler(text, response, topics, best_topic);
    (void)result; // 未使用変数の警告を抑制
    
    return best_agent;
}

// エージェントのスコアを計算
float calculate_score(int agent_id, const Token* tokens, int token_count) {
    if (agent_id < 0 || agent_id >= agent_count) {
        return 0.0f;
    }
    
    float score = 0.0f;
    int matches = 0;
    
    // 各パターンについて
    for (int i = 0; i < agents[agent_id].pattern_count; i++) {
        Pattern* pattern = &agents[agent_id].patterns[i];
        
        // 各トークンについて
        for (int j = 0; j < token_count; j++) {
            bool keyword_match = false;
            bool pos_match = false;
            bool type_match = false;
            
            // キーワードのマッチ
            if (strcmp(pattern->keyword, "*") == 0 || 
                strcmp(pattern->keyword, tokens[j].surface) == 0 || 
                strcmp(pattern->keyword, tokens[j].base) == 0) {
                keyword_match = true;
            }
            
            // 品詞のマッチ
            if (strcmp(pattern->pos, "*") == 0 || 
                strcmp(pattern->pos, tokens[j].pos) == 0 || 
                strcmp(pattern->pos, tokens[j].pos_detail) == 0) {
                pos_match = true;
            }
            
            // タイプのマッチ
            if (pattern->type == UNKNOWN || pattern->type == tokens[j].type) {
                type_match = true;
            }
            
            // すべての条件がマッチした場合
            if (keyword_match && pos_match && type_match) {
                score += pattern->weight;
                matches++;
                break;  // 同じパターンは一度だけカウント
            }
        }
    }
    
    // パターン数で正規化
    if (agents[agent_id].pattern_count > 0) {
        score /= agents[agent_id].pattern_count;
    }
    
    return score;
}

// トピックのスコアを計算
float calculate_topic_score(int topic_id, const Token* tokens, int token_count) {
    if (topic_id < 0 || topic_id >= topic_count) {
        return 0.0f;
    }
    
    float score = 0.0f;
    int matches = 0;
    
    // 各パターンについて
    for (int i = 0; i < topics[topic_id].pattern_count; i++) {
        Pattern* pattern = &topics[topic_id].patterns[i];
        
        // 各トークンについて
        for (int j = 0; j < token_count; j++) {
            bool keyword_match = false;
            bool pos_match = false;
            bool type_match = false;
            
            // キーワードのマッチ
            if (strcmp(pattern->keyword, "*") == 0 || 
                strcmp(pattern->keyword, tokens[j].surface) == 0 || 
                strcmp(pattern->keyword, tokens[j].base) == 0) {
                keyword_match = true;
            }
            
            // 品詞のマッチ
            if (strcmp(pattern->pos, "*") == 0 || 
                strcmp(pattern->pos, tokens[j].pos) == 0 || 
                strcmp(pattern->pos, tokens[j].pos_detail) == 0) {
                pos_match = true;
            }
            
            // タイプのマッチ
            if (pattern->type == UNKNOWN || pattern->type == tokens[j].type) {
                type_match = true;
            }
            
            // すべての条件がマッチした場合
            if (keyword_match && pos_match && type_match) {
                score += pattern->weight;
                matches++;
                break;  // 同じパターンは一度だけカウント
            }
        }
    }
    
    // パターン数で正規化
    if (topics[topic_id].pattern_count > 0) {
        score /= topics[topic_id].pattern_count;
    }
    
    return score;
}

// 最適なトピックを見つける
int find_best_topic(const Token* tokens, int token_count) {
    float max_score = -1.0f;
    int best_topic = -1;
    
    for (int i = 0; i < topic_count; i++) {
        float score = calculate_topic_score(i, tokens, token_count);
        
        printf("トピック '%s' のスコア: %.4f\n", topics[i].name, score);
        
        if (score > max_score) {
            max_score = score;
            best_topic = i;
        }
    }
    
    if (best_topic != -1) {
        printf("選択されたトピック: %s (スコア: %.4f)\n", 
               topics[best_topic].name, max_score);
    } else {
        printf("適切なトピックが見つかりませんでした\n");
    }
    
    return best_topic;
}

// 挨拶エージェントのハンドラ
char handle_greeting(const char* text, char* response, Topic* topics, int topic_id) {
    // 時間に応じた挨拶
    time_t now = time(NULL);
    struct tm* local = localtime(&now);
    int hour = local->tm_hour;
    
    if (strstr(text, "おはよう") != NULL) {
        strcpy(response, "おはようございます！今日も良い一日をお過ごしください。");
    } else if (strstr(text, "こんにちは") != NULL) {
        strcpy(response, "こんにちは！お元気ですか？");
    } else if (strstr(text, "こんばんは") != NULL) {
        strcpy(response, "こんばんは！今日も一日お疲れ様でした。");
    } else if (strstr(text, "さようなら") != NULL || strstr(text, "バイバイ") != NULL) {
        strcpy(response, "さようなら！またお会いしましょう。");
    } else if (strstr(text, "ありがとう") != NULL) {
        strcpy(response, "どういたしまして！お役に立てて嬉しいです。");
    } else {
        // 時間に応じたデフォルトの挨拶
        if (hour >= 5 && hour < 12) {
            strcpy(response, "おはようございます！何かお手伝いできることはありますか？");
        } else if (hour >= 12 && hour < 18) {
            strcpy(response, "こんにちは！何かお手伝いできることはありますか？");
        } else {
            strcpy(response, "こんばんは！何かお手伝いできることはありますか？");
        }
    }
    
    return 1;  // 成功
}

// 質問エージェントのハンドラ
char handle_question(const char* text, char* response, Topic* topics, int topic_id) {
    // 推論ルールを適用して応答を生成
    printf("推論ルールを適用します...\n");
    if (apply_inference_rules(text, response)) {
        printf("推論ルールが適用されました: %s\n", response);
        return 1;  // 推論ルールが適用された
    }
    printf("適用可能な推論ルールが見つかりませんでした\n");
    
    // トピックが見つかった場合は、そのトピックの知識から回答を生成
    if (topic_id >= 0 && topic_id < topic_count) {
        // ランダムに知識エントリを選択
        if (topics[topic_id].knowledge_count > 0) {
            int knowledge_id = rand() % topics[topic_id].knowledge_count;
            strcpy(response, topics[topic_id].knowledge[knowledge_id].text);
            return 1;
        }
    }
    
    // 外部LLMを使用する場合
    if (USE_EXTERNAL_LLM) {
        printf("外部LLMに問い合わせます...\n");
        char llm_response[MAX_RESPONSE_LEN];
        if (query_external_llm(text, llm_response, MAX_RESPONSE_LEN, api_key)) {
            strcpy(response, llm_response);
            return 1;
        }
        printf("外部LLMからの応答取得に失敗しました\n");
    }
    
    // 外部LLMが使用できない場合や失敗した場合は、質問の種類に応じた一般的な回答
    if (strstr(text, "何") != NULL || strstr(text, "なに") != NULL) {
        strcpy(response, "それについては詳しい情報を持っていません。もう少し具体的に教えていただけますか？");
    } else if (strstr(text, "誰") != NULL || strstr(text, "だれ") != NULL) {
        strcpy(response, "特定の人物についての情報は持っていません。");
    } else if (strstr(text, "どこ") != NULL) {
        strcpy(response, "場所についての情報は持っていません。");
    } else if (strstr(text, "いつ") != NULL) {
        strcpy(response, "時間についての情報は持っていません。");
    } else if (strstr(text, "なぜ") != NULL || strstr(text, "どうして") != NULL) {
        strcpy(response, "理由については分析が必要です。もう少し情報をいただけますか？");
    } else if (strstr(text, "どう") != NULL || strstr(text, "どの") != NULL) {
        strcpy(response, "それについては判断が難しいです。もう少し具体的に教えていただけますか？");
    } else {
        strcpy(response, "ご質問ありがとうございます。申し訳ありませんが、その質問にはお答えできません。");
    }
    
    return 1;  // 成功
}

// 命令エージェントのハンドラ
char handle_command(const char* text, char* response, Topic* topics, int topic_id) {
    // トピックが見つかった場合は、そのトピックの知識から回答を生成
    if (topic_id >= 0 && topic_id < topic_count) {
        // ランダムに知識エントリを選択
        if (topics[topic_id].knowledge_count > 0) {
            int knowledge_id = rand() % topics[topic_id].knowledge_count;
            strcpy(response, topics[topic_id].knowledge[knowledge_id].text);
            return 1;
        }
    }
    
    // トピックが見つからない場合や知識がない場合は、命令の種類を判断
    if (strstr(text, "教えて") != NULL) {
        strcpy(response, "申し訳ありませんが、現在その情報は持っていません。");
    } else if (strstr(text, "探して") != NULL || strstr(text, "検索") != NULL) {
        strcpy(response, "検索機能は現在実装されていません。");
    } else if (strstr(text, "計算") != NULL) {
        strcpy(response, "計算機能は現在実装されていません。");
    } else if (strstr(text, "翻訳") != NULL) {
        strcpy(response, "翻訳機能は現在実装されていません。");
    } else if (strstr(text, "作成") != NULL || strstr(text, "作って") != NULL) {
        strcpy(response, "作成機能は現在実装されていません。");
    } else if (strstr(text, "予約") != NULL) {
        strcpy(response, "予約機能は現在実装されていません。");
    } else {
        strcpy(response, "申し訳ありませんが、その命令は実行できません。");
    }
    
    return 1;  // 成功
}

// 陳述エージェントのハンドラ
char handle_statement(const char* text, char* response, Topic* topics, int topic_id) {
    // トピックが見つかった場合は、そのトピックの知識から回答を生成
    if (topic_id >= 0 && topic_id < topic_count) {
        // ランダムに知識エントリを選択
        if (topics[topic_id].knowledge_count > 0) {
            int knowledge_id = rand() % topics[topic_id].knowledge_count;
            strcpy(response, topics[topic_id].knowledge[knowledge_id].text);
            return 1;
        }
    }
    
    // ランダムな応答を選択
    int random = rand() % 5;
    
    switch (random) {
        case 0:
            strcpy(response, "なるほど、理解しました。");
            break;
        case 1:
            strcpy(response, "その情報は参考になります。");
            break;
        case 2:
            strcpy(response, "ありがとうございます。他に何かありますか？");
            break;
        case 3:
            strcpy(response, "それは興味深いですね。");
            break;
        case 4:
            strcpy(response, "そうなんですね。もう少し詳しく教えていただけますか？");
            break;
        default:
            strcpy(response, "ご意見ありがとうございます。");
            break;
    }
    
    return 1;  // 成功
}

// 感情エージェントのハンドラ
char handle_emotion(const char* text, char* response, Topic* topics, int topic_id) {
    // トピックが見つかった場合は、そのトピックの知識から回答を生成
    if (topic_id >= 0 && topic_id < topic_count) {
        // ランダムに知識エントリを選択
        if (topics[topic_id].knowledge_count > 0) {
            int knowledge_id = rand() % topics[topic_id].knowledge_count;
            strcpy(response, topics[topic_id].knowledge[knowledge_id].text);
            return 1;
        }
    }
    
    // 感情の種類を判断
    if (strstr(text, "嬉しい") != NULL || strstr(text, "楽しい") != NULL || strstr(text, "幸せ") != NULL) {
        strcpy(response, "それは良かったですね！私もあなたが嬉しいと嬉しいです。");
    } else if (strstr(text, "悲しい") != NULL || strstr(text, "辛い") != NULL || strstr(text, "苦しい") != NULL) {
        strcpy(response, "辛い気持ちになってしまって申し訳ありません。何かお力になれることはありますか？");
    } else if (strstr(text, "怒り") != NULL || strstr(text, "腹立たしい") != NULL || strstr(text, "イライラ") != NULL) {
        strcpy(response, "お怒りのようですね。落ち着いて話し合えるといいですね。");
    } else if (strstr(text, "不安") != NULL || strstr(text, "心配") != NULL || strstr(text, "恐怖") != NULL) {
        strcpy(response, "不安なお気持ちになってしまって申し訳ありません。何かお力になれることはありますか？");
    } else if (strstr(text, "好き") != NULL || strstr(text, "愛") != NULL) {
        strcpy(response, "ありがとうございます。私もあなたとお話しできて嬉しいです。");
    } else if (strstr(text, "嫌い") != NULL || strstr(text, "憎") != NULL) {
        strcpy(response, "不快な思いをさせてしまって申し訳ありません。改善できるよう努力します。");
    } else {
        strcpy(response, "あなたのお気持ちを理解しようと努めています。もう少し教えていただけますか？");
    }
    
    return 1;  // 成功
}

// フォールバックエージェントのハンドラ
char handle_fallback(const char* text, char* response, Topic* topics, int topic_id) {
    // 推論ルールを適用して応答を生成
    printf("推論ルールを適用します...\n");
    if (apply_inference_rules(text, response)) {
        printf("推論ルールが適用されました: %s\n", response);
        return 1;  // 推論ルールが適用された
    }
    printf("適用可能な推論ルールが見つかりませんでした\n");
    
    // トピックが見つかった場合は、そのトピックの知識から回答を生成
    if (topic_id >= 0 && topic_id < topic_count) {
        // ランダムに知識エントリを選択
        if (topics[topic_id].knowledge_count > 0) {
            int knowledge_id = rand() % topics[topic_id].knowledge_count;
            strcpy(response, topics[topic_id].knowledge[knowledge_id].text);
            return 1;
        }
    }
    
    // 外部LLMを使用する場合
    if (USE_EXTERNAL_LLM) {
        printf("外部LLMに問い合わせます...\n");
        char llm_response[MAX_RESPONSE_LEN];
        if (query_external_llm(text, llm_response, MAX_RESPONSE_LEN, api_key)) {
            strcpy(response, llm_response);
            return 1;
        }
        printf("外部LLMからの応答取得に失敗しました\n");
    }
    
    // ランダムなフォールバック応答を選択
    int random = rand() % 5;
    
    switch (random) {
        case 0:
            strcpy(response, "申し訳ありませんが、よく理解できませんでした。別の言い方でお試しいただけますか？");
            break;
        case 1:
            strcpy(response, "もう少し具体的に教えていただけますか？");
            break;
        case 2:
            strcpy(response, "すみません、その内容については対応できません。");
            break;
        case 3:
            strcpy(response, "別の質問や話題についてお聞きしたいことはありますか？");
            break;
        case 4:
            strcpy(response, "申し訳ありませんが、その内容は私の知識の範囲外です。");
            break;
        default:
            strcpy(response, "ご質問の意図を理解できませんでした。別の方法で質問していただけますか？");
            break;
    }
    
    return 1;  // 成功
}

// 概念タイプを文字列に変換
const char* concept_type_to_string(ConceptType type) {
    switch (type) {
        case ENTITY:
            return "ENTITY";
        case CONCEPT:
            return "CONCEPT";
        case RELATION:
            return "RELATION";
        case ATTRIBUTE:
            return "ATTRIBUTE";
        case TIME:
            return "TIME";
        case LOCATION:
            return "LOCATION";
        case QUANTITY:
            return "QUANTITY";
        default:
            return "UNKNOWN";
    }
}

// 文字列を概念タイプに変換
ConceptType string_to_concept_type(const char* str) {
    if (strcmp(str, "ENTITY") == 0) {
        return ENTITY;
    } else if (strcmp(str, "CONCEPT") == 0) {
        return CONCEPT;
    } else if (strcmp(str, "RELATION") == 0) {
        return RELATION;
    } else if (strcmp(str, "ATTRIBUTE") == 0) {
        return ATTRIBUTE;
    } else if (strcmp(str, "TIME") == 0) {
        return TIME;
    } else if (strcmp(str, "LOCATION") == 0) {
        return LOCATION;
    } else if (strcmp(str, "QUANTITY") == 0) {
        return QUANTITY;
    } else {
        return UNKNOWN;
    }
}

// トピック関連性を初期化
void init_topic_relations() {
    relation_count = 0;
    
    // 幸せトピックの関連性
    add_topic_relation("happiness", "health", 0.7f);
    add_topic_relation("happiness", "love", 0.8f);
    add_topic_relation("happiness", "work", 0.6f);
    
    // 愛トピックの関連性
    add_topic_relation("love", "happiness", 0.8f);
    add_topic_relation("love", "health", 0.5f);
    
    // 健康トピックの関連性
    add_topic_relation("health", "happiness", 0.7f);
    add_topic_relation("health", "survival", 0.6f);
    
    // 仕事トピックの関連性
    add_topic_relation("work", "happiness", 0.6f);
    add_topic_relation("work", "learning", 0.7f);
    add_topic_relation("work", "programming", 0.5f);
    
    // 学習トピックの関連性
    add_topic_relation("learning", "work", 0.7f);
    add_topic_relation("learning", "math", 0.6f);
    add_topic_relation("learning", "programming", 0.7f);
    
    // 数学トピックの関連性
    add_topic_relation("math", "learning", 0.6f);
    add_topic_relation("math", "physics", 0.8f);
    add_topic_relation("math", "programming", 0.7f);
    
    // 物理トピックの関連性
    add_topic_relation("physics", "math", 0.8f);
    add_topic_relation("physics", "programming", 0.5f);
    
    // プログラミングトピックの関連性
    add_topic_relation("programming", "learning", 0.7f);
    add_topic_relation("programming", "math", 0.7f);
    add_topic_relation("programming", "work", 0.5f);
    
    // ギタートピックの関連性
    add_topic_relation("guitar", "happiness", 0.5f);
    
    // サバイバルトピックの関連性
    add_topic_relation("survival", "health", 0.6f);
    
    printf("%d個のトピック関連性を初期化しました\n", relation_count);
}

// 推論ルールを初期化
void init_inference_rules() {
    rule_count = 0;
    
    // 幸せに関する推論ルール
    add_inference_rule("幸せ AND 方法", "幸せになるためには、小さな喜びを見つけることが大切です。", 0.8f);
    add_inference_rule("幸せ AND コツ", "幸せを感じるコツは、感謝の気持ちを持つことです。", 0.8f);
    
    // 健康に関する推論ルール
    add_inference_rule("健康 AND 維持", "健康を維持するには、バランスの良い食事と適度な運動が重要です。", 0.9f);
    add_inference_rule("健康 AND 食事", "健康的な食事には、多くの野菜と果物を含めることが重要です。", 0.9f);
    
    // 学習に関する推論ルール
    add_inference_rule("学習 AND 効率", "効率的な学習のためには、集中できる環境と定期的な復習が重要です。", 0.8f);
    add_inference_rule("学習 AND 記憶", "記憶を定着させるには、アクティブラーニングと間隔を空けた復習が効果的です。", 0.8f);
    
    // プログラミングに関する推論ルール
    add_inference_rule("プログラミング AND 始め", "プログラミングを始めるには、基本的な概念を学び、簡単なプロジェクトから取り組むことが良いでしょう。", 0.9f);
    add_inference_rule("プログラミング AND 始め方", "プログラミングを始めるには、基本的な概念を学び、簡単なプロジェクトから取り組むことが良いでしょう。", 0.9f);
    add_inference_rule("プログラミング AND 言語", "プログラミング言語を選ぶ際は、目的に合わせて選ぶことが重要です。初心者にはPythonがおすすめです。", 0.8f);
    add_inference_rule("プログラミング AND やり方", "プログラミングを学ぶ最良の方法は、実際にコードを書いて試してみることです。オンラインチュートリアルや書籍から基本を学び、小さなプロジェクトに取り組みましょう。", 0.9f);
    
    // ギターに関する推論ルール
    add_inference_rule("ギター AND 弾き方", "ギターの基本的な弾き方は、正しい姿勢でコードを押さえ、ピッキングの練習から始めることです。", 0.9f);
    add_inference_rule("ギター AND 初心者", "ギター初心者は、基本的なコードと簡単な曲から練習を始めると良いでしょう。", 0.8f);
    
    // サバイバルに関する推論ルール
    add_inference_rule("サバイバル AND 基本", "サバイバルの基本は、水、食料、シェルター、火の確保です。", 0.9f);
    add_inference_rule("サバイバル AND 技術", "重要なサバイバル技術には、火起こし、水の浄化、シェルター構築、ナビゲーションがあります。", 0.9f);
    
    // 量子コンピュータに関する推論ルール
    add_inference_rule("量子コンピュータ AND 仕組み", "量子コンピュータは量子力学の原理を利用し、従来のビットではなく量子ビット（キュービット）を使用します。キュービットは重ね合わせ状態で0と1を同時に表現できます。", 0.95f);
    add_inference_rule("量子 AND コンピュータ", "量子コンピュータは量子力学の原理を利用し、従来のビットではなく量子ビット（キュービット）を使用します。特定の計算で従来のコンピュータより高速です。", 0.95f);
    add_inference_rule("量子 AND 計算", "量子計算は量子力学の原理を利用した計算方法で、暗号解読やデータベース検索などで従来のコンピュータより効率的です。", 0.9f);
    add_inference_rule("量子 AND 利点", "量子計算の主な利点は、特定の問題（因数分解や検索など）を従来のコンピュータより指数関数的に高速に解けることです。これは量子重ね合わせと量子もつれの性質を活用しています。", 0.95f);
    add_inference_rule("量子コンピュータ AND 利点", "量子コンピュータの利点は、暗号解読、データベース検索、物質シミュレーションなどの特定の問題を従来のコンピュータより圧倒的に高速に解けることです。", 0.95f);
    
    // 料理に関する推論ルール
    add_inference_rule("料理 AND 基本", "料理の基本は、食材の特性を理解し、適切な調理法と調味料を選ぶことです。包丁の扱いや火加減の調整も重要なスキルです。", 0.9f);
    add_inference_rule("料理 AND コツ", "料理のコツは、新鮮な食材を選び、下ごしらえをしっかりと行い、味の調整を少しずつ行うことです。また、レシピに頼りすぎず、自分の感覚も大切にしましょう。", 0.9f);
    add_inference_rule("和食 AND 特徴", "和食の特徴は、素材の持ち味を活かし、うま味を重視する点です。四季折々の食材を用い、見た目の美しさも大切にします。", 0.9f);
    add_inference_rule("料理 AND 初心者", "料理初心者は、シンプルなレシピから始め、基本的な調理技術（切る、炒める、煮るなど）を習得することが大切です。失敗を恐れず、楽しみながら学びましょう。", 0.9f);
    
    // フィットネスに関する推論ルール
    add_inference_rule("フィットネス AND 始め方", "フィットネスを始めるには、まず自分の目標を設定し、現在の体力レベルに合った運動から始めることが大切です。無理なく継続できるプランを立てましょう。", 0.9f);
    add_inference_rule("筋トレ AND 方法", "効果的な筋トレ方法は、正しいフォームで適切な重量を使い、筋肉グループごとにトレーニングすることです。十分な休息と栄養摂取も重要です。", 0.9f);
    add_inference_rule("ダイエット AND 効果的", "効果的なダイエットは、適度な運動と栄養バランスの良い食事の組み合わせです。急激な減量よりも、持続可能な生活習慣の改善を目指しましょう。", 0.9f);
    add_inference_rule("有酸素運動 AND 効果", "有酸素運動の効果には、心肺機能の向上、脂肪燃焼の促進、ストレス軽減などがあります。ウォーキング、ジョギング、水泳などが代表的です。", 0.9f);
    
    // 旅行に関する推論ルール
    add_inference_rule("旅行 AND 計画", "効果的な旅行計画には、目的地の情報収集、予算設定、交通手段と宿泊先の予約、必要書類の確認が含まれます。余裕を持ったスケジュールを立てることも大切です。", 0.9f);
    add_inference_rule("海外旅行 AND 準備", "海外旅行の準備には、パスポートとビザの確認、現地通貨の準備、旅行保険への加入、現地の文化や習慣の理解が重要です。", 0.9f);
    add_inference_rule("旅行 AND コツ", "旅行を楽しむコツは、過密なスケジュールを避け、現地の文化や食事を体験する時間を作ることです。また、予期せぬ事態に備えて柔軟な計画を立てましょう。", 0.9f);
    add_inference_rule("バックパッキング AND 方法", "バックパッキングのコツは、荷物を最小限に抑え、現地での交通手段や宿泊施設を事前に調査することです。現地の人との交流も旅の魅力を高めます。", 0.9f);
    
    // 金融・投資に関する推論ルール
    add_inference_rule("投資 AND 始め方", "投資を始めるには、まず緊急資金を確保し、自分のリスク許容度を理解した上で、分散投資の原則に従って少額から始めることが賢明です。", 0.9f);
    add_inference_rule("株式投資 AND 方法", "株式投資の基本は、企業の財務状況や業界動向を分析し、長期的な視点で投資することです。分散投資とドルコスト平均法も有効な戦略です。", 0.9f);
    add_inference_rule("資産運用 AND 戦略", "効果的な資産運用戦略には、リスク分散、長期投資、複利の活用、定期的な見直しが含まれます。自分の年齢やライフステージに合わせた調整も重要です。", 0.9f);
    add_inference_rule("貯金 AND コツ", "効果的な貯金のコツは、収入の一部を自動的に貯蓄に回し、無駄な支出を減らすことです。明確な目標を設定し、進捗を定期的に確認しましょう。", 0.9f);
    
    // エンターテイメントに関する推論ルール
    add_inference_rule("映画 AND おすすめ", "映画選びのコツは、自分の好みのジャンルや監督、俳優の作品から探すことです。また、評論家のレビューや友人の推薦も参考になります。", 0.9f);
    add_inference_rule("音楽 AND ジャンル", "音楽ジャンルは非常に多様で、ロック、ポップ、クラシック、ジャズ、ヒップホップなど様々です。各ジャンルには独自の特徴や歴史があります。", 0.9f);
    add_inference_rule("ストリーミング AND サービス", "音楽や映画のストリーミングサービスは、豊富なコンテンツを月額制で楽しめるサービスです。自分の視聴習慣に合ったサービスを選ぶことが重要です。", 0.9f);
    add_inference_rule("コンサート AND 楽しみ方", "コンサートを楽しむコツは、事前にアーティストの曲を聴いて予習し、会場の情報を確認することです。当日は時間に余裕を持って行動しましょう。", 0.9f);
    
    // 未知のトピックに対する一般的な推論ルール
    add_inference_rule("* AND 始め方", "新しいことを始めるには、基本から学び、少しずつ実践することが大切です。", 0.7f);
    add_inference_rule("* AND 方法", "効果的な方法を見つけるには、専門家のアドバイスを参考にし、自分に合ったやり方を見つけることが重要です。", 0.7f);
    add_inference_rule("* AND コツ", "上達するコツは、定期的な練習と継続的な学習です。", 0.7f);
    
    printf("%d個の推論ルールを初期化しました\n", rule_count);
}

// トピック関連性を追加
void add_topic_relation(const char* topic, const char* related_topic, float strength) {
    if (relation_count >= MAX_TOPICS_LOGIC) {
        fprintf(stderr, "警告: トピック関連性の上限に達しました\n");
        return;
    }
    
    // 既存のトピックを探す
    int topic_index = -1;
    for (int i = 0; i < relation_count; i++) {
        if (strcmp(topic_relations[i].topic_name, topic) == 0) {
            topic_index = i;
            break;
        }
    }
    
    // 新しいトピックを追加
    if (topic_index == -1) {
        topic_index = relation_count++;
        strncpy(topic_relations[topic_index].topic_name, topic, MAX_TOPIC_NAME_LOGIC - 1);
        topic_relations[topic_index].topic_name[MAX_TOPIC_NAME_LOGIC - 1] = '\0';
        topic_relations[topic_index].related_count = 0;
    }
    
    // 関連トピックを追加
    if (topic_relations[topic_index].related_count < MAX_RELATED_TOPICS) {
        int related_index = topic_relations[topic_index].related_count;
        strncpy(topic_relations[topic_index].related_topics[related_index], related_topic, MAX_TOPIC_NAME_LOGIC - 1);
        topic_relations[topic_index].related_topics[related_index][MAX_TOPIC_NAME_LOGIC - 1] = '\0';
        topic_relations[topic_index].relation_strength[related_index] = strength;
        topic_relations[topic_index].related_count++;
    } else {
        fprintf(stderr, "警告: トピック '%s' の関連トピック上限に達しました\n", topic);
    }
}

// 推論ルールを追加
void add_inference_rule(const char* condition, const char* conclusion, float confidence) {
    if (rule_count >= MAX_INFERENCE_RULES) {
        fprintf(stderr, "警告: 推論ルールの上限に達しました\n");
        return;
    }
    
    strncpy(inference_rules[rule_count].condition, condition, MAX_RULE_TEXT - 1);
    inference_rules[rule_count].condition[MAX_RULE_TEXT - 1] = '\0';
    
    strncpy(inference_rules[rule_count].conclusion, conclusion, MAX_RULE_TEXT - 1);
    inference_rules[rule_count].conclusion[MAX_RULE_TEXT - 1] = '\0';
    
    inference_rules[rule_count].confidence = confidence;
    
    rule_count++;
}

// 関連トピックを検索
bool find_related_topics(const char* topic, char related_topics[MAX_RELATED_TOPICS][MAX_TOPIC_NAME_LOGIC], float strengths[MAX_RELATED_TOPICS], int* count) {
    *count = 0;
    
    // トピックを探す
    for (int i = 0; i < relation_count; i++) {
        if (strcmp(topic_relations[i].topic_name, topic) == 0) {
            // 関連トピックをコピー
            for (int j = 0; j < topic_relations[i].related_count; j++) {
                strncpy(related_topics[j], topic_relations[i].related_topics[j], MAX_TOPIC_NAME_LOGIC - 1);
                related_topics[j][MAX_TOPIC_NAME_LOGIC - 1] = '\0';
                strengths[j] = topic_relations[i].relation_strength[j];
            }
            *count = topic_relations[i].related_count;
            return true;
        }
    }
    
    return false;
}

// 推論ルールを適用
bool apply_inference_rules(const char* input, char* response) {
    bool rule_applied = false;
    float max_confidence = 0.0f;
    
    printf("入力テキスト: '%s'\n", input);
    printf("推論ルール数: %d\n", rule_count);
    
    // 各ルールをチェック
    for (int i = 0; i < rule_count; i++) {
        char condition[MAX_RULE_TEXT];
        strncpy(condition, inference_rules[i].condition, MAX_RULE_TEXT - 1);
        condition[MAX_RULE_TEXT - 1] = '\0';
        
        printf("ルール %d: 条件='%s', 結論='%s', 確信度=%.2f\n", 
               i, inference_rules[i].condition, 
               inference_rules[i].conclusion, inference_rules[i].confidence);
        
        char condition_copy[MAX_RULE_TEXT];
        strncpy(condition_copy, condition, MAX_RULE_TEXT - 1);
        condition_copy[MAX_RULE_TEXT - 1] = '\0';
        
        char* token = strtok(condition_copy, " AND ");
        bool match = true;
        
        // ANDで区切られた条件をすべてチェック
        while (token != NULL && match) {
            // ワイルドカードの場合は常にマッチ
            if (strcmp(token, "*") == 0) {
                printf("  ワイルドカードマッチ: '*'\n");
                token = strtok(NULL, " AND ");
                continue;
            }
            
            // 入力テキストに条件が含まれているかチェック
            if (strstr(input, token) == NULL) {
                printf("  不一致: '%s' が入力テキストに含まれていません\n", token);
                match = false;
                break;
            } else {
                printf("  一致: '%s' が入力テキストに含まれています\n", token);
            }
            
            token = strtok(NULL, " AND ");
        }
        
        // すべての条件がマッチし、より高い確信度を持つ場合
        if (match && inference_rules[i].confidence > max_confidence) {
            strncpy(response, inference_rules[i].conclusion, MAX_RESPONSE_LEN - 1);
            response[MAX_RESPONSE_LEN - 1] = '\0';
            max_confidence = inference_rules[i].confidence;
            rule_applied = true;
            printf("  ルール適用: 確信度=%.2f, 結論='%s'\n", 
                   inference_rules[i].confidence, inference_rules[i].conclusion);
        } else if (match) {
            printf("  ルールはマッチしましたが、より高い確信度のルールが既に適用されています\n");
        }
    }
    
    if (rule_applied) {
        printf("最終的に適用されたルールの確信度: %.2f\n", max_confidence);
    } else {
        printf("適用可能なルールが見つかりませんでした\n");
    }
    
    return rule_applied;
}

// トピック間の類似度を計算
float calculate_topic_similarity(const char* topic1, const char* topic2) {
    // 同じトピックの場合は最大類似度
    if (strcmp(topic1, topic2) == 0) {
        return 1.0f;
    }
    
    // トピック1の関連トピックを取得
    char related_topics[MAX_RELATED_TOPICS][MAX_TOPIC_NAME_LOGIC];
    float strengths[MAX_RELATED_TOPICS];
    int count = 0;
    
    if (find_related_topics(topic1, related_topics, strengths, &count)) {
        // トピック2が関連トピックに含まれているか確認
        for (int i = 0; i < count; i++) {
            if (strcmp(related_topics[i], topic2) == 0) {
                return strengths[i];
            }
        }
    }
    
    // トピック2の関連トピックを取得
    if (find_related_topics(topic2, related_topics, strengths, &count)) {
        // トピック1が関連トピックに含まれているか確認
        for (int i = 0; i < count; i++) {
            if (strcmp(related_topics[i], topic1) == 0) {
                return strengths[i];
            }
        }
    }
    
    // 間接的な関連性を確認（2ホップ）
    float max_similarity = 0.0f;
    
    if (find_related_topics(topic1, related_topics, strengths, &count)) {
        for (int i = 0; i < count; i++) {
            char related_topics2[MAX_RELATED_TOPICS][MAX_TOPIC_NAME_LOGIC];
            float strengths2[MAX_RELATED_TOPICS];
            int count2 = 0;
            
            if (find_related_topics(related_topics[i], related_topics2, strengths2, &count2)) {
                for (int j = 0; j < count2; j++) {
                    if (strcmp(related_topics2[j], topic2) == 0) {
                        float similarity = strengths[i] * strengths2[j];
                        if (similarity > max_similarity) {
                            max_similarity = similarity;
                        }
                    }
                }
            }
        }
    }
    
    return max_similarity;
}

// メイン関数
int main(int argc, char* argv[]) {
    // ロケールを設定（日本語対応）
    setlocale(LC_ALL, "");
    
    // 乱数の初期化
    srand((unsigned int)time(NULL));
    
    // エージェントとトピックを初期化
    init_agents();
    init_topics();
    init_topic_relations();
    init_inference_rules();
    
    // コマンドライン引数の解析
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--api-key") == 0) {
            if (i + 1 < argc) {
                strncpy(api_key, argv[i + 1], MAX_API_KEY_LEN - 1);
                api_key[MAX_API_KEY_LEN - 1] = '\0';
                printf("APIキーが設定されました\n");
                i++;  // APIキーの値をスキップ
            } else {
                printf("--api-key オプションには値が必要です\n");
                return 1;
            }
        } else {
            // 最初の非オプション引数を見つけたらループを抜ける
            break;
        }
    }
    
    // APIキーが設定されていない場合は環境変数から取得
    if (strlen(api_key) == 0) {
        char* env_api_key = getenv("OPENAI_API_KEY");
        if (env_api_key != NULL) {
            strncpy(api_key, env_api_key, MAX_API_KEY_LEN - 1);
            api_key[MAX_API_KEY_LEN - 1] = '\0';
            printf("環境変数からAPIキーが設定されました\n");
        }
    }
    
    // コマンドライン引数のチェック
    if (i >= argc) {
        printf("使用法:\n");
        printf("  %s [--api-key <APIキー>] <入力テキスト>\n", argv[0]);
        printf("  %s [--api-key <APIキー>] -f <入力ファイル>\n", argv[0]);
        printf("  %s [--api-key <APIキー>] -i (対話モード)\n", argv[0]);
        
        // APIキーが設定されていない場合は入力を促す
        if (strlen(api_key) == 0) {
            printf("\nOpenAI APIキーを入力してください（入力しない場合はシミュレーションモードで動作します）: ");
            if (fgets(api_key, MAX_API_KEY_LEN, stdin) != NULL) {
                // 改行文字を削除
                size_t len = strlen(api_key);
                if (len > 0 && api_key[len - 1] == '\n') {
                    api_key[len - 1] = '\0';
                }
                
                if (strlen(api_key) > 0) {
                    printf("APIキーが設定されました\n");
                }
            }
        }
        
        // 対話モードを開始
        printf("対話モードを開始します。終了するには 'exit' と入力してください。\n");
        printf("APIキー関連コマンド: /api-help で確認できます。\n");
        char text_buffer[MAX_TEXT_LEN];
        
        while (1) {
            // 入力を受け取る
            printf("> ");
            if (!fgets(text_buffer, MAX_TEXT_LEN, stdin)) {
                break;
            }
            
            // 改行を削除
            size_t len = strlen(text_buffer);
            if (len > 0 && text_buffer[len-1] == '\n') {
                text_buffer[len-1] = '\0';
            }
            
            // 終了コマンドをチェック
            if (strcmp(text_buffer, "exit") == 0) {
                break;
            }
            
            // APIキー設定コマンドをチェック
            if (strncmp(text_buffer, "/api-key ", 9) == 0) {
                strncpy(api_key, text_buffer + 9, MAX_API_KEY_LEN - 1);
                api_key[MAX_API_KEY_LEN - 1] = '\0';
                printf("APIキーが設定されました\n");
                continue;
            }
            
            // APIキー状態確認コマンド
            if (strcmp(text_buffer, "/api-status") == 0) {
                if (strlen(api_key) > 0) {
                    printf("APIキーが設定されています\n");
                } else {
                    printf("APIキーが設定されていません（シミュレーションモードで動作します）\n");
                }
                continue;
            }
            
            // APIキーヘルプコマンド
            if (strcmp(text_buffer, "/api-help") == 0) {
                printf("APIキー関連コマンド:\n");
                printf("  /api-key <APIキー> - APIキーを設定します\n");
                printf("  /api-status - APIキーの設定状態を確認します\n");
                printf("  /api-help - このヘルプを表示します\n");
                continue;
            }
            
            // テキストをルーティング
            char response[MAX_RESPONSE_LEN];
            route_text(text_buffer, response);
            
            // 応答を表示
            printf("応答: %s\n", response);
        }
        
        return 0;
    }
    
    // モードを判定
    if (strcmp(argv[i], "-f") == 0) {
        if (i + 1 >= argc) {
            printf("ファイルモードでは入力ファイルが必要です。\n");
            return 1;
        }
        
        // テキストバッファを準備
        char text_buffer[MAX_TEXT_LEN];
        
        // ファイルを開く
        FILE* file = fopen(argv[i + 1], "r");
        if (!file) {
            printf("ファイル %s を開けませんでした。\n", argv[i + 1]);
            return 1;
        }
        
        // ファイルを読み込む
        size_t read_size = fread(text_buffer, 1, MAX_TEXT_LEN - 1, file);
        text_buffer[read_size] = '\0';
        fclose(file);
        
        // テキストをルーティング
        char response[MAX_RESPONSE_LEN];
        route_text(text_buffer, response);
        
        // 応答を表示
        printf("応答: %s\n", response);
    } else if (strcmp(argv[i], "-i") == 0) {
        // 対話モード
        printf("対話モードを開始します。終了するには 'exit' と入力してください。\n");
        printf("APIキー関連コマンド: /api-help で確認できます。\n");
        
        while (1) {
            // 入力を受け取る
            printf("> ");
            if (!fgets(text_buffer, MAX_TEXT_LEN, stdin)) {
                break;
            }
            
            // 改行を削除
            size_t len = strlen(text_buffer);
            if (len > 0 && text_buffer[len-1] == '\n') {
                text_buffer[len-1] = '\0';
            }
            
            // 終了コマンドをチェック
            if (strcmp(text_buffer, "exit") == 0) {
                break;
            }
            
            // APIキー設定コマンドをチェック
            if (strncmp(text_buffer, "/api-key ", 9) == 0) {
                strncpy(api_key, text_buffer + 9, MAX_API_KEY_LEN - 1);
                api_key[MAX_API_KEY_LEN - 1] = '\0';
                printf("APIキーが設定されました\n");
                continue;
            }
            
            // APIキー状態確認コマンド
            if (strcmp(text_buffer, "/api-status") == 0) {
                if (strlen(api_key) > 0) {
                    printf("APIキーが設定されています\n");
                } else {
                    printf("APIキーが設定されていません（シミュレーションモードで動作します）\n");
                }
                continue;
            }
            
            // APIキーヘルプコマンド
            if (strcmp(text_buffer, "/api-help") == 0) {
                printf("APIキー関連コマンド:\n");
                printf("  /api-key <APIキー> - APIキーを設定します\n");
                printf("  /api-status - APIキーの設定状態を確認します\n");
                printf("  /api-help - このヘルプを表示します\n");
                continue;
            }
            
            // テキストをルーティング
            char response[MAX_RESPONSE_LEN];
            route_text(text_buffer, response);
            
            // 応答を表示
            printf("応答: %s\n", response);
        }
    } else {
        // 単一テキストモード
        // コマンドライン引数を連結
        char text_buffer[MAX_TEXT_LEN];
        text_buffer[0] = '\0';
        for (int j = i; j < argc; j++) {
            if (j > i) {
                strcat(text_buffer, " ");
            }
            strcat(text_buffer, argv[j]);
        }
        
        // テキストをルーティング
        char response[MAX_RESPONSE_LEN];
        route_text(text_buffer, response);
        
        // 応答を表示
        printf("応答: %s\n", response);
    }
    
    return 0;
}