#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STR_LEN 1024

// 品詞の種類
typedef enum {
    SUBJECT,  // 主語
    VERB,     // 動詞
    RESULT    // 結果
} TokenType;

// 抽出された要素
typedef struct {
    char text[MAX_STR_LEN];
    TokenType type;
} Token;

// 日本語の文から主語、動詞、結果を抽出する
void extract_japanese_elements(const char* sentence, char* subject, char* verb, char* result) {
    // 作業用のバッファにコピー
    char buffer[MAX_STR_LEN];
    strncpy(buffer, sentence, MAX_STR_LEN - 1);
    buffer[MAX_STR_LEN - 1] = '\0';
    
    // 句読点を削除
    char* p = buffer;
    while (*p) {
        if (strncmp(p, "。", 3) == 0 || 
            strncmp(p, "、", 3) == 0 || 
            strncmp(p, "！", 3) == 0 || 
            strncmp(p, "？", 3) == 0) {
            *p = '\0';
        }
        p++;
    }
    
    // 主語の抽出（「が」「は」の前まで）
    char* ga = strstr(buffer, "が");
    char* wa = strstr(buffer, "は");
    char* subject_marker = NULL;
    
    if (ga && (!wa || ga < wa)) {
        subject_marker = ga;
    } else if (wa) {
        subject_marker = wa;
    }
    
    if (subject_marker) {
        int len = subject_marker - buffer;
        strncpy(subject, buffer, len);
        subject[len] = '\0';
    } else {
        // 主語マーカーがない場合は、最初の単語を主語と仮定
        strcpy(subject, "");
        char* first_space = strchr(buffer, ' ');
        if (first_space) {
            int len = first_space - buffer;
            strncpy(subject, buffer, len);
            subject[len] = '\0';
        } else {
            // スペースがない場合は、最初の数文字を主語と仮定
            int len = strlen(buffer) > 3 ? 3 : strlen(buffer);
            strncpy(subject, buffer, len);
            subject[len] = '\0';
        }
    }
    
    // 動詞の抽出（「を」の後ろから「て」の前まで）
    char* wo = strstr(buffer, "を");
    char* te = strstr(buffer, "て");
    
    if (wo && te && wo < te) {
        int start = wo - buffer + 3; // 「を」の後ろ
        int len = te - buffer - start;
        strncpy(verb, buffer + start, len);
        verb[len] = '\0';
        
        // 「食べ」のような形になっているので、「る」を追加
        strcat(verb, "る");
    } else {
        // 特定の動詞を検出
        if (strstr(buffer, "食べ")) {
            strcpy(verb, "食べる");
        } else if (strstr(buffer, "飲み")) {
            strcpy(verb, "飲む");
        } else if (strstr(buffer, "見")) {
            strcpy(verb, "見る");
        } else if (strstr(buffer, "行き")) {
            strcpy(verb, "行く");
        } else if (strstr(buffer, "来")) {
            strcpy(verb, "来る");
        } else {
            strcpy(verb, "する"); // デフォルト
        }
    }
    
    // 結果の抽出（「て」の後ろ）
    if (te) {
        int start = te - buffer + 3; // 「て」の後ろ
        strcpy(result, buffer + start);
        
        // 「嬉しかっ」のような形になっているので、「た」を追加
        if (strstr(result, "かっ") && !strstr(result, "かった")) {
            strcat(result, "た");
        }
    } else {
        // 特定の結果を検出
        if (strstr(buffer, "嬉し")) {
            strcpy(result, "嬉しい");
        } else if (strstr(buffer, "悲し")) {
            strcpy(result, "悲しい");
        } else if (strstr(buffer, "楽し")) {
            strcpy(result, "楽しい");
        } else {
            strcpy(result, ""); // 結果不明
        }
    }
    
    // 空白を削除
    p = subject;
    while (*p) {
        if (*p == ' ') {
            memmove(p, p + 1, strlen(p));
        } else {
            p++;
        }
    }
    
    p = verb;
    while (*p) {
        if (*p == ' ') {
            memmove(p, p + 1, strlen(p));
        } else {
            p++;
        }
    }
    
    p = result;
    while (*p) {
        if (*p == ' ') {
            memmove(p, p + 1, strlen(p));
        } else {
            p++;
        }
    }
}

// 英語の文から主語、動詞、結果を抽出する
void extract_english_elements(const char* sentence, char* subject, char* verb, char* result) {
    // 作業用のバッファにコピー
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
    char* tokens[100];
    int token_count = 0;
    
    char* token = strtok(buffer, " ");
    while (token != NULL && token_count < 100) {
        tokens[token_count++] = token;
        token = strtok(NULL, " ");
    }
    
    // 主語、動詞、結果を抽出
    if (token_count > 0) {
        strcpy(subject, tokens[0]); // 最初のトークンを主語とする
    } else {
        strcpy(subject, "");
    }
    
    if (token_count > 1) {
        strcpy(verb, tokens[1]); // 2番目のトークンを動詞とする
    } else {
        strcpy(verb, "");
    }
    
    if (token_count > 2) {
        strcpy(result, tokens[2]); // 3番目のトークンを結果とする
        
        // 残りのトークンも結果に含める
        for (int i = 3; i < token_count; i++) {
            strcat(result, " ");
            strcat(result, tokens[i]);
        }
    } else {
        strcpy(result, "");
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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("使用方法: %s \"分析する文\"\n", argv[0]);
        return 1;
    }
    
    const char* sentence = argv[1];
    char subject[MAX_STR_LEN] = "";
    char verb[MAX_STR_LEN] = "";
    char result[MAX_STR_LEN] = "";
    
    if (is_japanese(sentence)) {
        extract_japanese_elements(sentence, subject, verb, result);
    } else {
        extract_english_elements(sentence, subject, verb, result);
    }
    
    printf("入力文: %s\n", sentence);
    printf("抽出結果: [主語: %s][動詞: %s][結果: %s]\n", subject, verb, result);
    
    return 0;
}