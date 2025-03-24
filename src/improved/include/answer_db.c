#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_ANSWERS 100
#define MAX_QUESTION_LENGTH 256
#define MAX_ANSWER_LENGTH 4096

typedef struct {
    char question[MAX_QUESTION_LENGTH];
    char answer[MAX_ANSWER_LENGTH];
} QAPair;

static QAPair answer_db[MAX_ANSWERS];
static int answer_db_size = 0;

// 文字列内のエスケープシーケンスを実際の文字に変換する
void unescape_string(char *str) {
    char *src = str;
    char *dst = str;
    char c;
    
    while ((c = *src++) != '\0') {
        if (c == '\\' && *src != '\0') {
            switch (*src) {
                case 'n': c = '\n'; src++; break;
                case 't': c = '\t'; src++; break;
                case 'r': c = '\r'; src++; break;
                case '\\': c = '\\'; src++; break;
                // 他のエスケープシーケンスも必要に応じて追加
            }
        }
        *dst++ = c;
    }
    *dst = '\0';
}

// 回答データベースを初期化する
void init_answer_db() {
    FILE *fp;
    char line[MAX_QUESTION_LENGTH + MAX_ANSWER_LENGTH + 2]; // +2 for delimiter and null terminator
    char *question, *answer;
    
    // ファイルを開く
    fp = fopen("knowledge/base/answers.txt", "r");
    if (fp == NULL) {
        printf("回答データベースファイルを開けませんでした\n");
        return;
    }
    
    // ファイルから質問と回答のペアを読み込む
    while (fgets(line, sizeof(line), fp) != NULL && answer_db_size < MAX_ANSWERS) {
        // 改行文字を削除
        line[strcspn(line, "\n")] = 0;
        
        // 質問と回答を分割（区切り文字は'|'）
        question = strtok(line, "|");
        answer = strtok(NULL, "");
        
        if (question != NULL && answer != NULL) {
            // 質問と回答をデータベースに追加
            strncpy(answer_db[answer_db_size].question, question, MAX_QUESTION_LENGTH - 1);
            answer_db[answer_db_size].question[MAX_QUESTION_LENGTH - 1] = '\0';
            
            strncpy(answer_db[answer_db_size].answer, answer, MAX_ANSWER_LENGTH - 1);
            answer_db[answer_db_size].answer[MAX_ANSWER_LENGTH - 1] = '\0';
            
            // エスケープシーケンスを変換
            unescape_string(answer_db[answer_db_size].answer);
            
            answer_db_size++;
        }
    }
    
    // ファイルを閉じる
    fclose(fp);
    
    printf("回答データベースを初期化しました（%d件の回答）\n", answer_db_size);
}

// 質問に対する回答を検索する
const char* find_answer(const char* question) {
    int i;
    double best_score = 0.0;
    int best_match = -1;
    
    // 完全一致を検索
    for (i = 0; i < answer_db_size; i++) {
        if (strcmp(answer_db[i].question, question) == 0) {
            return answer_db[i].answer;
        }
    }
    
    // 部分一致を検索（単純な文字列含有チェック）
    for (i = 0; i < answer_db_size; i++) {
        if (strstr(question, answer_db[i].question) != NULL || 
            strstr(answer_db[i].question, question) != NULL) {
            // 文字列長の比率でスコアを計算
            double len_ratio = (double)strlen(question) / strlen(answer_db[i].question);
            if (len_ratio > 1.0) len_ratio = 1.0 / len_ratio;
            
            double score = len_ratio * 0.8; // 最大スコアは0.8（完全一致より低い）
            
            if (score > best_score) {
                best_score = score;
                best_match = i;
            }
        }
    }
    
    // 部分一致が見つかった場合
    if (best_match >= 0) {
        return answer_db[best_match].answer;
    }
    
    // 回答が見つからない場合
    return NULL;
}

// 回答データベースのサイズを取得する
int get_answer_db_size() {
    return answer_db_size;
}