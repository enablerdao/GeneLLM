#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <locale.h>

#define MAX_WORD_LEN 64
#define MAX_TOKENS 1000
#define MAX_LINE_LEN 1024
#define MAX_TEXT_LEN 102400

// 品詞の種類
typedef enum {
    NOUN,       // 名詞
    VERB,       // 動詞
    ADJECTIVE,  // 形容詞
    ADVERB,     // 副詞
    PARTICLE,   // 助詞
    AUXILIARY,  // 助動詞
    CONJUNCTION,// 接続詞
    PREFIX,     // 接頭辞
    SUFFIX,     // 接尾辞
    PRONOUN,    // 代名詞
    NUMBER,     // 数詞
    INTERJECTION,// 感動詞
    SYMBOL,     // 記号
    UNKNOWN     // 不明
} PartOfSpeech;

// 活用形
typedef enum {
    NO_CONJUGATION, // 活用なし
    BASE_FORM,      // 基本形
    MASU_FORM,      // ます形
    TE_FORM,        // て形
    TA_FORM,        // た形
    NEGATIVE_FORM,  // 否定形
    PASSIVE_FORM,   // 受身形
    CAUSATIVE_FORM, // 使役形
    IMPERATIVE_FORM,// 命令形
    POTENTIAL_FORM, // 可能形
    CONDITIONAL_FORM// 条件形
} ConjugationForm;

// 形態素情報
typedef struct {
    char surface[MAX_WORD_LEN]; // 表層形
    char base[MAX_WORD_LEN];    // 基本形
    PartOfSpeech pos;           // 品詞
    ConjugationForm conj;       // 活用形
    double confidence;          // 信頼度
} Token;

// グローバル変数
char text_buffer[MAX_TEXT_LEN];
Token tokens[MAX_TOKENS];
int token_count = 0;

// 関数プロトタイプ
void tokenize_text(const char* text);
void generate_simple_sentences();
const char* get_particle_for_verb(const char* verb);
const char* conjugate_verb(const char* verb, ConjugationForm form);

// 助詞を取得
const char* get_particle_for_verb(const char* verb) {
    // 動詞に適した助詞を返す
    // 実際には動詞の種類によって適切な助詞が異なるが、ここでは簡易的に実装
    
    if (strcmp(verb, "する") == 0 || 
        strcmp(verb, "行う") == 0 || 
        strcmp(verb, "実施") == 0 || 
        strcmp(verb, "開催") == 0) {
        return "を";
    } else if (strcmp(verb, "ある") == 0 || 
               strcmp(verb, "いる") == 0 || 
               strcmp(verb, "存在") == 0) {
        return "に";
    } else if (strcmp(verb, "なる") == 0 || 
               strcmp(verb, "変わる") == 0 || 
               strcmp(verb, "発展") == 0) {
        return "に";
    } else if (strcmp(verb, "行く") == 0 || 
               strcmp(verb, "来る") == 0 || 
               strcmp(verb, "帰る") == 0 || 
               strcmp(verb, "向かう") == 0) {
        return "へ";
    } else if (strcmp(verb, "会う") == 0 || 
               strcmp(verb, "似る") == 0 || 
               strcmp(verb, "対応") == 0) {
        return "と";
    } else if (strcmp(verb, "始まる") == 0 || 
               strcmp(verb, "終わる") == 0 || 
               strcmp(verb, "出発") == 0) {
        return "から";
    } else {
        // デフォルトは「を」
        return "を";
    }
}

// 動詞の活用形を取得
const char* conjugate_verb(const char* verb, ConjugationForm form) {
    static char conjugated[MAX_WORD_LEN];
    
    // 基本形をコピー
    strncpy(conjugated, verb, MAX_WORD_LEN - 1);
    conjugated[MAX_WORD_LEN - 1] = '\0';
    
    size_t len = strlen(conjugated);
    
    // 特殊な動詞の処理
    bool is_special = false;
    if (strcmp(verb, "する") == 0) {
        is_special = true;
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "します");
                break;
            case TE_FORM:
                strcpy(conjugated, "して");
                break;
            case TA_FORM:
                strcpy(conjugated, "した");
                break;
            case NEGATIVE_FORM:
                strcpy(conjugated, "しない");
                break;
            case PASSIVE_FORM:
                strcpy(conjugated, "される");
                break;
            case CAUSATIVE_FORM:
                strcpy(conjugated, "させる");
                break;
            case IMPERATIVE_FORM:
                strcpy(conjugated, "しろ");
                break;
            case POTENTIAL_FORM:
                strcpy(conjugated, "できる");
                break;
            case CONDITIONAL_FORM:
                strcpy(conjugated, "すれば");
                break;
            default:
                break;
        }
    } else if (strcmp(verb, "来る") == 0) {
        is_special = true;
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "きます");
                break;
            case TE_FORM:
                strcpy(conjugated, "きて");
                break;
            case TA_FORM:
                strcpy(conjugated, "きた");
                break;
            case NEGATIVE_FORM:
                strcpy(conjugated, "こない");
                break;
            case PASSIVE_FORM:
                strcpy(conjugated, "こられる");
                break;
            case CAUSATIVE_FORM:
                strcpy(conjugated, "こさせる");
                break;
            case IMPERATIVE_FORM:
                strcpy(conjugated, "こい");
                break;
            case POTENTIAL_FORM:
                strcpy(conjugated, "こられる");
                break;
            case CONDITIONAL_FORM:
                strcpy(conjugated, "くれば");
                break;
            default:
                break;
        }
    } else if (strcmp(verb, "食べる") == 0) {
        is_special = true;
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "食べます");
                break;
            case TE_FORM:
                strcpy(conjugated, "食べて");
                break;
            case TA_FORM:
                strcpy(conjugated, "食べた");
                break;
            case NEGATIVE_FORM:
                strcpy(conjugated, "食べない");
                break;
            case PASSIVE_FORM:
                strcpy(conjugated, "食べられる");
                break;
            case CAUSATIVE_FORM:
                strcpy(conjugated, "食べさせる");
                break;
            case IMPERATIVE_FORM:
                strcpy(conjugated, "食べろ");
                break;
            case POTENTIAL_FORM:
                strcpy(conjugated, "食べられる");
                break;
            case CONDITIONAL_FORM:
                strcpy(conjugated, "食べれば");
                break;
            default:
                break;
        }
    } else if (strcmp(verb, "飛ぶ") == 0) {
        is_special = true;
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "飛びます");
                break;
            case TE_FORM:
                strcpy(conjugated, "飛んで");
                break;
            case TA_FORM:
                strcpy(conjugated, "飛んだ");
                break;
            case NEGATIVE_FORM:
                strcpy(conjugated, "飛ばない");
                break;
            case PASSIVE_FORM:
                strcpy(conjugated, "飛ばれる");
                break;
            case CAUSATIVE_FORM:
                strcpy(conjugated, "飛ばせる");
                break;
            case IMPERATIVE_FORM:
                strcpy(conjugated, "飛べ");
                break;
            case POTENTIAL_FORM:
                strcpy(conjugated, "飛べる");
                break;
            case CONDITIONAL_FORM:
                strcpy(conjugated, "飛べば");
                break;
            default:
                break;
        }
    } else if (strcmp(verb, "話す") == 0) {
        is_special = true;
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "話します");
                break;
            case TE_FORM:
                strcpy(conjugated, "話して");
                break;
            case TA_FORM:
                strcpy(conjugated, "話した");
                break;
            case NEGATIVE_FORM:
                strcpy(conjugated, "話さない");
                break;
            case PASSIVE_FORM:
                strcpy(conjugated, "話される");
                break;
            case CAUSATIVE_FORM:
                strcpy(conjugated, "話させる");
                break;
            case IMPERATIVE_FORM:
                strcpy(conjugated, "話せ");
                break;
            case POTENTIAL_FORM:
                strcpy(conjugated, "話せる");
                break;
            case CONDITIONAL_FORM:
                strcpy(conjugated, "話せば");
                break;
            default:
                break;
        }
    }
    
    return conjugated;
}

// 文字列が特定の文字で始まるかチェック
bool starts_with(const char* str, const char* prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

// テキストをトークン化する
void tokenize_text(const char* text) {
    token_count = 0;
    
    // 予め登録する単語と品詞
    struct {
        const char* word;
        PartOfSpeech pos;
    } predefined_words[] = {
        {"猫", NOUN},
        {"犬", NOUN},
        {"鳥", NOUN},
        {"人間", NOUN},
        {"魚", NOUN},
        {"骨", NOUN},
        {"空", NOUN},
        {"言葉", NOUN},
        {"食べる", VERB},
        {"飛ぶ", VERB},
        {"話す", VERB},
        {"泳ぐ", VERB},
        {"好き", ADJECTIVE},
        {"かわいい", ADJECTIVE},
        {"忠実", ADJECTIVE},
        {"自由", ADJECTIVE},
        {"静か", ADJECTIVE},
        {"は", PARTICLE},
        {"が", PARTICLE},
        {"を", PARTICLE},
        {"に", PARTICLE},
        {"で", PARTICLE},
        {"と", PARTICLE},
        {"から", PARTICLE},
        {"まで", PARTICLE},
        {"へ", PARTICLE},
        {"の", PARTICLE},
        {"だ", AUXILIARY}
    };
    
    int predefined_count = sizeof(predefined_words) / sizeof(predefined_words[0]);
    
    // 直接テキストを解析
    const char* p = text;
    
    // 簡易的な単語分割
    while (*p && token_count < MAX_TOKENS - 20) {
        // 空白をスキップ
        if (isspace((unsigned char)*p)) {
            p++;
            continue;
        }
        
        // 文の区切りをスキップ
        if (starts_with(p, "。") || *p == '.' || starts_with(p, "！") || *p == '!' || 
            starts_with(p, "？") || *p == '?') {
            if (starts_with(p, "。") || starts_with(p, "！") || starts_with(p, "？")) {
                p += strlen("。"); // マルチバイト文字をスキップ
            } else {
                p++;
            }
            continue;
        }
        
        // 単語を抽出
        char word[MAX_WORD_LEN] = {0};
        int word_len = 0;
        
        // 助詞を検出
        if (starts_with(p, "は") || starts_with(p, "が") || starts_with(p, "を") || 
            starts_with(p, "に") || starts_with(p, "で") || starts_with(p, "と") || 
            starts_with(p, "から") || starts_with(p, "まで") || starts_with(p, "へ") || 
            starts_with(p, "の")) {
            
            // 助詞をコピー
            if (starts_with(p, "から") || starts_with(p, "まで")) {
                strncpy(word, p, 2);
                word[2] = '\0';
                p += 2;
            } else {
                word[0] = *p++;
                word[1] = '\0';
            }
            
            // トークンに追加
            strncpy(tokens[token_count].surface, word, MAX_WORD_LEN - 1);
            tokens[token_count].surface[MAX_WORD_LEN - 1] = '\0';
            
            strncpy(tokens[token_count].base, word, MAX_WORD_LEN - 1);
            tokens[token_count].base[MAX_WORD_LEN - 1] = '\0';
            
            tokens[token_count].pos = PARTICLE;
            tokens[token_count].conj = NO_CONJUGATION;
            tokens[token_count].confidence = 1.0;
            
            token_count++;
            continue;
        }
        
        // 予め登録された単語を検出
        bool found = false;
        for (int i = 0; i < predefined_count; i++) {
            if (starts_with(p, predefined_words[i].word)) {
                // 単語をコピー
                strncpy(word, predefined_words[i].word, MAX_WORD_LEN - 1);
                word[MAX_WORD_LEN - 1] = '\0';
                
                // トークンに追加
                strncpy(tokens[token_count].surface, word, MAX_WORD_LEN - 1);
                tokens[token_count].surface[MAX_WORD_LEN - 1] = '\0';
                
                strncpy(tokens[token_count].base, word, MAX_WORD_LEN - 1);
                tokens[token_count].base[MAX_WORD_LEN - 1] = '\0';
                
                tokens[token_count].pos = predefined_words[i].pos;
                tokens[token_count].conj = BASE_FORM;
                tokens[token_count].confidence = 1.0;
                
                token_count++;
                
                // ポインタを進める
                p += strlen(word);
                found = true;
                break;
            }
        }
        
        if (found) {
            continue;
        }
        
        // その他の単語を抽出
        while (*p && !isspace((unsigned char)*p) && 
               !starts_with(p, "。") && *p != '.' && !starts_with(p, "！") && *p != '!' && 
               !starts_with(p, "？") && *p != '?' && 
               !starts_with(p, "は") && !starts_with(p, "が") && !starts_with(p, "を") && 
               !starts_with(p, "に") && !starts_with(p, "で") && !starts_with(p, "と") && 
               !starts_with(p, "から") && !starts_with(p, "まで") && !starts_with(p, "へ") && 
               !starts_with(p, "の") && !starts_with(p, "だ")) {
            
            if (word_len < MAX_WORD_LEN - 1) {
                // マルチバイト文字の場合は全体をコピー
                if ((*p & 0x80) != 0) {
                    // UTF-8の先頭バイトを検出
                    int bytes = 0;
                    if ((*p & 0xE0) == 0xC0) bytes = 2;      // 2バイト文字
                    else if ((*p & 0xF0) == 0xE0) bytes = 3; // 3バイト文字
                    else if ((*p & 0xF8) == 0xF0) bytes = 4; // 4バイト文字
                    
                    // マルチバイト文字を一度にコピー
                    for (int i = 0; i < bytes && *p && word_len < MAX_WORD_LEN - 1; i++) {
                        word[word_len++] = *p++;
                    }
                } else {
                    word[word_len++] = *p++;
                }
            } else {
                p++;
            }
        }
        
        if (word_len > 0) {
            word[word_len] = '\0';
            
            // 品詞を推測
            PartOfSpeech pos = NOUN; // デフォルトは名詞
            
            // 動詞の特徴を持つ単語
            if (strstr(word, "食べ") || strstr(word, "飛") || strstr(word, "話")) {
                pos = VERB;
            }
            // 形容詞の特徴を持つ単語
            else if (strstr(word, "好き") || strstr(word, "良い") || strstr(word, "悪い") || 
                     strstr(word, "高い") || strstr(word, "低い") || strstr(word, "大きい")) {
                pos = ADJECTIVE;
            }
            
            // トークンに追加
            strncpy(tokens[token_count].surface, word, MAX_WORD_LEN - 1);
            tokens[token_count].surface[MAX_WORD_LEN - 1] = '\0';
            
            strncpy(tokens[token_count].base, word, MAX_WORD_LEN - 1);
            tokens[token_count].base[MAX_WORD_LEN - 1] = '\0';
            
            tokens[token_count].pos = pos;
            tokens[token_count].conj = BASE_FORM;
            tokens[token_count].confidence = 1.0;
            
            token_count++;
        }
    }
    
    // 特定の単語を基本形に変換
    for (int i = 0; i < token_count; i++) {
        // 動詞の活用形を基本形に変換
        if (strstr(tokens[i].surface, "食べ") && tokens[i].pos == VERB) {
            strcpy(tokens[i].base, "食べる");
        } else if (strstr(tokens[i].surface, "飛") && tokens[i].pos == VERB) {
            strcpy(tokens[i].base, "飛ぶ");
        } else if (strstr(tokens[i].surface, "話") && tokens[i].pos == VERB) {
            strcpy(tokens[i].base, "話す");
        } else if (strstr(tokens[i].surface, "泳") && tokens[i].pos == VERB) {
            strcpy(tokens[i].base, "泳ぐ");
        }
        
        // 形容詞の活用形を基本形に変換
        if (strstr(tokens[i].surface, "好き") && tokens[i].pos != ADJECTIVE) {
            tokens[i].pos = ADJECTIVE;
            strcpy(tokens[i].base, "好き");
        } else if (strstr(tokens[i].surface, "かわいい") && tokens[i].pos != ADJECTIVE) {
            tokens[i].pos = ADJECTIVE;
            strcpy(tokens[i].base, "かわいい");
        } else if (strstr(tokens[i].surface, "忠実") && tokens[i].pos != ADJECTIVE) {
            tokens[i].pos = ADJECTIVE;
            strcpy(tokens[i].base, "忠実");
        } else if (strstr(tokens[i].surface, "自由") && tokens[i].pos != ADJECTIVE) {
            tokens[i].pos = ADJECTIVE;
            strcpy(tokens[i].base, "自由");
        } else if (strstr(tokens[i].surface, "静か") && tokens[i].pos != ADJECTIVE) {
            tokens[i].pos = ADJECTIVE;
            strcpy(tokens[i].base, "静か");
        }
        
        // 助動詞「だ」を処理
        if (strcmp(tokens[i].surface, "だ") == 0) {
            tokens[i].pos = AUXILIARY;
            strcpy(tokens[i].base, "だ");
        }
    }
    
    // 文末の「だ」を前の形容詞と結合
    for (int i = 0; i < token_count - 1; i++) {
        if (tokens[i].pos == ADJECTIVE && 
            i + 1 < token_count && 
            strcmp(tokens[i+1].surface, "だ") == 0) {
            
            // 形容詞の基本形を更新
            char temp[MAX_WORD_LEN];
            snprintf(temp, MAX_WORD_LEN - 1, "%sだ", tokens[i].base);
            strcpy(tokens[i].base, temp);
            
            // 「だ」を削除
            for (int j = i + 1; j < token_count - 1; j++) {
                tokens[j] = tokens[j + 1];
            }
            token_count--;
            i--; // 同じインデックスを再チェック
        }
    }
    
    // 「好きだ。鳥」のような問題を修正
    for (int i = 0; i < token_count; i++) {
        // 表層形と基本形の両方をチェック
        char* period_surface = strstr(tokens[i].surface, "。");
        char* period_base = strstr(tokens[i].base, "。");
        
        if (period_surface) {
            // 表層形の文の区切りを見つけた場合、そこで切る
            *period_surface = '\0';
        }
        
        if (period_base) {
            // 基本形の文の区切りを見つけた場合、そこで切る
            *period_base = '\0';
        }
    }
    
    // 追加の基本的な単語を登録
    if (token_count < MAX_TOKENS) {
        strcpy(tokens[token_count].surface, "猫");
        strcpy(tokens[token_count].base, "猫");
        tokens[token_count].pos = NOUN;
        tokens[token_count].conj = NO_CONJUGATION;
        tokens[token_count].confidence = 1.0;
        token_count++;
    }
    
    if (token_count < MAX_TOKENS) {
        strcpy(tokens[token_count].surface, "犬");
        strcpy(tokens[token_count].base, "犬");
        tokens[token_count].pos = NOUN;
        tokens[token_count].conj = NO_CONJUGATION;
        tokens[token_count].confidence = 1.0;
        token_count++;
    }
    
    if (token_count < MAX_TOKENS) {
        strcpy(tokens[token_count].surface, "鳥");
        strcpy(tokens[token_count].base, "鳥");
        tokens[token_count].pos = NOUN;
        tokens[token_count].conj = NO_CONJUGATION;
        tokens[token_count].confidence = 1.0;
        token_count++;
    }
    
    if (token_count < MAX_TOKENS) {
        strcpy(tokens[token_count].surface, "人間");
        strcpy(tokens[token_count].base, "人間");
        tokens[token_count].pos = NOUN;
        tokens[token_count].conj = NO_CONJUGATION;
        tokens[token_count].confidence = 1.0;
        token_count++;
    }
    
    if (token_count < MAX_TOKENS) {
        strcpy(tokens[token_count].surface, "食べる");
        strcpy(tokens[token_count].base, "食べる");
        tokens[token_count].pos = VERB;
        tokens[token_count].conj = BASE_FORM;
        tokens[token_count].confidence = 1.0;
        token_count++;
    }
    
    if (token_count < MAX_TOKENS) {
        strcpy(tokens[token_count].surface, "飛ぶ");
        strcpy(tokens[token_count].base, "飛ぶ");
        tokens[token_count].pos = VERB;
        tokens[token_count].conj = BASE_FORM;
        tokens[token_count].confidence = 1.0;
        token_count++;
    }
    
    if (token_count < MAX_TOKENS) {
        strcpy(tokens[token_count].surface, "話す");
        strcpy(tokens[token_count].base, "話す");
        tokens[token_count].pos = VERB;
        tokens[token_count].conj = BASE_FORM;
        tokens[token_count].confidence = 1.0;
        token_count++;
    }
}

// 簡易的な文生成
void generate_simple_sentences() {
    // 名詞、動詞、形容詞を収集
    char nouns[MAX_TOKENS][MAX_WORD_LEN];
    char verbs[MAX_TOKENS][MAX_WORD_LEN];
    char adjectives[MAX_TOKENS][MAX_WORD_LEN];
    int noun_count = 0;
    int verb_count = 0;
    int adj_count = 0;
    
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].pos == NOUN && noun_count < MAX_TOKENS) {
            // 長すぎる名詞や短すぎる名詞は除外
            if (strlen(tokens[i].base) > 1 && strlen(tokens[i].base) < 15) {
                strncpy(nouns[noun_count], tokens[i].base, MAX_WORD_LEN - 1);
                nouns[noun_count][MAX_WORD_LEN - 1] = '\0';
                noun_count++;
            }
        } else if (tokens[i].pos == VERB && verb_count < MAX_TOKENS) {
            strncpy(verbs[verb_count], tokens[i].base, MAX_WORD_LEN - 1);
            verbs[verb_count][MAX_WORD_LEN - 1] = '\0';
            verb_count++;
        } else if (tokens[i].pos == ADJECTIVE && adj_count < MAX_TOKENS) {
            strncpy(adjectives[adj_count], tokens[i].base, MAX_WORD_LEN - 1);
            adjectives[adj_count][MAX_WORD_LEN - 1] = '\0';
            adj_count++;
        }
    }
    
    // 辞書に登録されている基本的な動詞を追加
    if (verb_count < 5) {
        strcpy(verbs[verb_count++], "する");
        strcpy(verbs[verb_count++], "ある");
        strcpy(verbs[verb_count++], "いる");
        strcpy(verbs[verb_count++], "なる");
        strcpy(verbs[verb_count++], "行く");
    }
    
    // 辞書に登録されている基本的な形容詞を追加
    if (adj_count < 5) {
        strcpy(adjectives[adj_count++], "良い");
        strcpy(adjectives[adj_count++], "悪い");
        strcpy(adjectives[adj_count++], "高い");
        strcpy(adjectives[adj_count++], "低い");
        strcpy(adjectives[adj_count++], "大きい");
    }
    
    // 重複を除去
    for (int i = 0; i < noun_count; i++) {
        for (int j = i + 1; j < noun_count; j++) {
            if (strcmp(nouns[i], nouns[j]) == 0) {
                // j番目の要素を削除
                for (int k = j; k < noun_count - 1; k++) {
                    strcpy(nouns[k], nouns[k + 1]);
                }
                noun_count--;
                j--; // 削除した位置を再チェック
            }
        }
    }
    
    for (int i = 0; i < verb_count; i++) {
        for (int j = i + 1; j < verb_count; j++) {
            if (strcmp(verbs[i], verbs[j]) == 0) {
                // j番目の要素を削除
                for (int k = j; k < verb_count - 1; k++) {
                    strcpy(verbs[k], verbs[k + 1]);
                }
                verb_count--;
                j--; // 削除した位置を再チェック
            }
        }
    }
    
    for (int i = 0; i < adj_count; i++) {
        for (int j = i + 1; j < adj_count; j++) {
            if (strcmp(adjectives[i], adjectives[j]) == 0) {
                // j番目の要素を削除
                for (int k = j; k < adj_count - 1; k++) {
                    strcpy(adjectives[k], adjectives[k + 1]);
                }
                adj_count--;
                j--; // 削除した位置を再チェック
            }
        }
    }
    
    printf("\n収集した単語:\n");
    printf("名詞: %d個\n", noun_count);
    for (int i = 0; i < noun_count; i++) {
        printf("  %s\n", nouns[i]);
    }
    
    printf("動詞: %d個\n", verb_count);
    for (int i = 0; i < verb_count; i++) {
        printf("  %s\n", verbs[i]);
    }
    
    printf("形容詞: %d個\n", adj_count);
    for (int i = 0; i < adj_count; i++) {
        printf("  %s\n", adjectives[i]);
    }
    
    // 文生成
    printf("\n生成された文:\n");
    
    // パターン1: [名詞]は[形容詞]です。
    if (noun_count > 0 && adj_count > 0) {
        for (int i = 0; i < 3 && i < noun_count; i++) {
            int noun_idx = rand() % noun_count;
            int adj_idx = rand() % adj_count;
            printf("%sは%sです。\n", nouns[noun_idx], adjectives[adj_idx]);
        }
    }
    
    // パターン2: [名詞]が[動詞]ます。
    if (noun_count > 0 && verb_count > 0) {
        for (int i = 0; i < 3 && i < noun_count; i++) {
            int noun_idx = rand() % noun_count;
            int verb_idx = rand() % verb_count;
            printf("%sが%s。\n", nouns[noun_idx], conjugate_verb(verbs[verb_idx], MASU_FORM));
        }
    }
    
    // パターン3: [名詞]は[名詞]を[動詞]ます。
    if (noun_count > 1 && verb_count > 0) {
        for (int i = 0; i < 3; i++) {
            int noun1_idx = rand() % noun_count;
            int noun2_idx = rand() % noun_count;
            int verb_idx = rand() % verb_count;
            
            // 同じ名詞が選ばれた場合は別の名詞を選ぶ
            while (noun2_idx == noun1_idx && noun_count > 1) {
                noun2_idx = rand() % noun_count;
            }
            
            const char* particle = get_particle_for_verb(verbs[verb_idx]);
            printf("%sは%s%s%s。\n", 
                   nouns[noun1_idx], 
                   nouns[noun2_idx], 
                   particle, 
                   conjugate_verb(verbs[verb_idx], MASU_FORM));
        }
    }
    
    // パターン4: [名詞]が[形容詞]と[動詞]ました。
    if (noun_count > 0 && adj_count > 0 && verb_count > 0) {
        for (int i = 0; i < 3; i++) {
            int noun_idx = rand() % noun_count;
            int adj_idx = rand() % adj_count;
            int verb_idx = rand() % verb_count;
            
            printf("%sが%sと%s。\n", 
                   nouns[noun_idx], 
                   adjectives[adj_idx], 
                   conjugate_verb(verbs[verb_idx], TA_FORM));
        }
    }
    
    // パターン5: [名詞]は[名詞]に[動詞]て、[形容詞]です。
    if (noun_count > 1 && verb_count > 0 && adj_count > 0) {
        for (int i = 0; i < 3; i++) {
            int noun1_idx = rand() % noun_count;
            int noun2_idx = rand() % noun_count;
            int verb_idx = rand() % verb_count;
            int adj_idx = rand() % adj_count;
            
            // 同じ名詞が選ばれた場合は別の名詞を選ぶ
            while (noun2_idx == noun1_idx && noun_count > 1) {
                noun2_idx = rand() % noun_count;
            }
            
            printf("%sは%sに%sて、%sです。\n", 
                   nouns[noun1_idx], 
                   nouns[noun2_idx], 
                   conjugate_verb(verbs[verb_idx], TE_FORM), 
                   adjectives[adj_idx]);
        }
    }
}

int main(int argc, char* argv[]) {
    // ロケールを設定（日本語対応）
    setlocale(LC_ALL, "");
    
    // 乱数の初期化
    srand((unsigned int)time(NULL));
    
    // コマンドライン引数のチェック
    if (argc < 2) {
        printf("使用法: %s <テキストファイル>\n", argv[0]);
        return 1;
    }
    
    // ファイルを開く
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("ファイル %s を開けませんでした。\n", argv[1]);
        return 1;
    }
    
    // ファイルを読み込む
    size_t read_size = fread(text_buffer, 1, MAX_TEXT_LEN - 1, file);
    text_buffer[read_size] = '\0';
    fclose(file);
    
    // テキストをトークン化
    tokenize_text(text_buffer);
    
    // 文を生成
    generate_simple_sentences();
    
    return 0;
}