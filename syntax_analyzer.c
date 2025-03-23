#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STR_LEN 1024
#define MAX_TOKENS 100

// 品詞の種類
typedef enum {
    SUBJECT,  // 主語
    VERB,     // 動詞
    RESULT,   // 結果
    OTHER     // その他
} TokenType;

// トークン構造体
typedef struct {
    char text[MAX_STR_LEN];
    TokenType type;
} Token;

// 日本語の助詞リスト
const char* JP_SUBJECT_MARKERS[] = {"が", "は", "も", "の"};
const int JP_SUBJECT_MARKERS_SIZE = 4;

const char* JP_OBJECT_MARKERS[] = {"を", "に", "へ"};
const int JP_OBJECT_MARKERS_SIZE = 3;

const char* JP_VERB_MARKERS[] = {"する", "した", "食べる", "食べた", "飲む", "飲んだ", "見る", "見た", "行く", "行った", "来る", "来た"};
const int JP_VERB_MARKERS_SIZE = 12;

const char* JP_RESULT_MARKERS[] = {"て", "で", "なって", "なった", "くて", "かった", "嬉しい", "嬉しかった", "悲しい", "悲しかった"};
const int JP_RESULT_MARKERS_SIZE = 10;

// 英語の主語候補（代名詞など）
const char* EN_SUBJECT_CANDIDATES[] = {"I", "you", "he", "she", "it", "they", "we", "the", "a", "an"};
const int EN_SUBJECT_CANDIDATES_SIZE = 10;

const char* EN_VERB_CANDIDATES[] = {"is", "are", "was", "were", "eat", "eats", "ate", "drink", "drinks", "drank", "see", "sees", "saw", "go", "goes", "went", "come", "comes", "came"};
const int EN_VERB_CANDIDATES_SIZE = 19;

const char* EN_RESULT_CANDIDATES[] = {"happy", "sad", "angry", "excited", "tired", "hungry", "thirsty", "good", "bad", "well", "sick"};
const int EN_RESULT_CANDIDATES_SIZE = 11;

// 文字列が配列内の要素で始まるかチェック
bool starts_with_any(const char* str, const char* array[], int size) {
    for (int i = 0; i < size; i++) {
        if (strncmp(str, array[i], strlen(array[i])) == 0) {
            return true;
        }
    }
    return false;
}

// 文字列が配列内の要素と一致するかチェック
bool equals_any(const char* str, const char* array[], int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(str, array[i]) == 0) {
            return true;
        }
    }
    return false;
}

// 日本語の文を分析する
void analyze_japanese(const char* sentence, Token* tokens, int* token_count) {
    char buffer[MAX_STR_LEN];
    strncpy(buffer, sentence, MAX_STR_LEN - 1);
    buffer[MAX_STR_LEN - 1] = '\0';
    
    // 句読点を削除
    char* p = buffer;
    while (*p) {
        // 日本語の句読点をスペースに置き換え
        if (strncmp(p, "。", 3) == 0 || 
            strncmp(p, "、", 3) == 0 || 
            strncmp(p, "！", 3) == 0 || 
            strncmp(p, "？", 3) == 0) {
            *p = ' ';
        }
        p++;
    }
    
    // 簡易的なトークン化（スペースで分割）
    char* token_str = strtok(buffer, " ");
    *token_count = 0;
    
    // 主語を探す（最初の「が」「は」などの前の単語）
    bool found_subject = false;
    bool found_verb = false;
    bool found_result = false;
    
    while (token_str != NULL && *token_count < MAX_TOKENS) {
        // 主語の検出
        if (!found_subject) {
            char* marker = NULL;
            for (int i = 0; i < JP_SUBJECT_MARKERS_SIZE; i++) {
                marker = strstr(token_str, JP_SUBJECT_MARKERS[i]);
                if (marker) {
                    // 主語を抽出（マーカーの前まで）
                    int len = marker - token_str;
                    strncpy(tokens[*token_count].text, token_str, len);
                    tokens[*token_count].text[len] = '\0';
                    tokens[*token_count].type = SUBJECT;
                    (*token_count)++;
                    found_subject = true;
                    break;
                }
            }
            
            // マーカーがない場合は、最初のトークンを主語と仮定
            if (!found_subject && *token_count == 0) {
                strcpy(tokens[*token_count].text, token_str);
                tokens[*token_count].type = SUBJECT;
                (*token_count)++;
                found_subject = true;
            }
        }
        // 動詞の検出
        else if (!found_verb) {
            if (starts_with_any(token_str, JP_VERB_MARKERS, JP_VERB_MARKERS_SIZE)) {
                strcpy(tokens[*token_count].text, token_str);
                tokens[*token_count].type = VERB;
                (*token_count)++;
                found_verb = true;
            }
        }
        // 結果の検出
        else if (!found_result) {
            // 「て」「で」などで終わる場合や、「なって」「なった」などを含む場合
            for (int i = 0; i < JP_RESULT_MARKERS_SIZE; i++) {
                if (strstr(token_str, JP_RESULT_MARKERS[i])) {
                    // 次のトークンも結果の一部として扱う
                    char* next_token = strtok(NULL, " ");
                    if (next_token) {
                        sprintf(tokens[*token_count].text, "%s %s", token_str, next_token);
                    } else {
                        strcpy(tokens[*token_count].text, token_str);
                    }
                    tokens[*token_count].type = RESULT;
                    (*token_count)++;
                    found_result = true;
                    break;
                }
            }
            
            // 結果が見つからない場合は、残りのトークンを結果とする
            if (!found_result) {
                strcpy(tokens[*token_count].text, token_str);
                tokens[*token_count].type = RESULT;
                (*token_count)++;
                found_result = true;
            }
        }
        
        token_str = strtok(NULL, " ");
    }
}

// 英語の文を分析する
void analyze_english(const char* sentence, Token* tokens, int* token_count) {
    char buffer[MAX_STR_LEN];
    strncpy(buffer, sentence, MAX_STR_LEN - 1);
    buffer[MAX_STR_LEN - 1] = '\0';
    
    // 句読点を削除
    char* p = buffer;
    while (*p) {
        if (*p == '.' || *p == ',' || *p == '!' || *p == '?') {
            *p = ' ';
        }
        p++;
    }
    
    // 簡易的なトークン化（スペースで分割）
    char* token_str = strtok(buffer, " ");
    *token_count = 0;
    
    // 主語、動詞、結果を順番に探す
    int state = 0; // 0: 主語を探す, 1: 動詞を探す, 2: 結果を探す
    
    while (token_str != NULL && *token_count < MAX_TOKENS) {
        if (state == 0) { // 主語を探す
            if (equals_any(token_str, EN_SUBJECT_CANDIDATES, EN_SUBJECT_CANDIDATES_SIZE) ||
                (*token_count == 0)) { // 最初のトークンは主語と仮定
                strcpy(tokens[*token_count].text, token_str);
                tokens[*token_count].type = SUBJECT;
                (*token_count)++;
                state = 1;
            }
        }
        else if (state == 1) { // 動詞を探す
            if (equals_any(token_str, EN_VERB_CANDIDATES, EN_VERB_CANDIDATES_SIZE)) {
                strcpy(tokens[*token_count].text, token_str);
                tokens[*token_count].type = VERB;
                (*token_count)++;
                state = 2;
            }
        }
        else if (state == 2) { // 結果を探す
            if (equals_any(token_str, EN_RESULT_CANDIDATES, EN_RESULT_CANDIDATES_SIZE) ||
                (*token_count == 2)) { // 動詞の後は結果と仮定
                strcpy(tokens[*token_count].text, token_str);
                tokens[*token_count].type = RESULT;
                (*token_count)++;
                // 結果が見つかったら、残りのトークンも結果として扱う
                char* next_token = strtok(NULL, " ");
                while (next_token != NULL) {
                    strcat(tokens[*token_count - 1].text, " ");
                    strcat(tokens[*token_count - 1].text, next_token);
                    next_token = strtok(NULL, " ");
                }
            }
        }
        
        if (state < 2) {
            token_str = strtok(NULL, " ");
        } else {
            break;
        }
    }
}

// 言語を判定する（簡易的な実装）
bool is_japanese(const char* text) {
    // 日本語の文字コード範囲をチェック（UTF-8を想定）
    unsigned char* p = (unsigned char*)text;
    while (*p) {
        if (*p >= 0xE0) { // 日本語のUTF-8は3バイト以上
            return true;
        }
        p++;
    }
    return false;
}

// 文を分析して主語、動詞、結果を抽出する
void analyze_sentence(const char* sentence, Token* tokens, int* token_count) {
    if (is_japanese(sentence)) {
        analyze_japanese(sentence, tokens, token_count);
    } else {
        analyze_english(sentence, tokens, token_count);
    }
}

// トークンタイプを文字列に変換
const char* token_type_to_string(TokenType type) {
    switch (type) {
        case SUBJECT: return "主語";
        case VERB: return "動詞";
        case RESULT: return "結果";
        default: return "その他";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("使用方法: %s \"分析する文\"\n", argv[0]);
        return 1;
    }
    
    const char* sentence = argv[1];
    Token tokens[MAX_TOKENS];
    int token_count = 0;
    
    analyze_sentence(sentence, tokens, &token_count);
    
    printf("入力文: %s\n", sentence);
    printf("抽出結果: ");
    
    for (int i = 0; i < token_count; i++) {
        printf("[%s: %s]", token_type_to_string(tokens[i].type), tokens[i].text);
    }
    printf("\n");
    
    return 0;
}