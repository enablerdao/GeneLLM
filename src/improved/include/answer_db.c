#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

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

// 単語の類似度を計算する簡易関数
double word_similarity(const char* word1, const char* word2) {
    int len1 = strlen(word1);
    int len2 = strlen(word2);
    
    // 長さが大きく異なる場合は類似度を下げる
    double len_ratio = (double)len1 / len2;
    if (len_ratio > 1.0) len_ratio = 1.0 / len_ratio;
    
    // 共通の文字数をカウント
    int common_chars = 0;
    for (int i = 0; i < len1; i++) {
        for (int j = 0; j < len2; j++) {
            if (tolower(word1[i]) == tolower(word2[j])) {
                common_chars++;
                break;
            }
        }
    }
    
    // 類似度を計算（共通文字の割合と長さの比率を考慮）
    double char_ratio = (double)common_chars / len1;
    return char_ratio * 0.7 + len_ratio * 0.3;
}

// 文章の類似度を計算する
double sentence_similarity(const char* sentence1, const char* sentence2) {
    // 簡易的な実装: 文字列の部分一致と長さの比率で類似度を計算
    if (strstr(sentence1, sentence2) != NULL) {
        double len_ratio = (double)strlen(sentence2) / strlen(sentence1);
        return 0.7 + len_ratio * 0.3; // 部分一致の場合は高いスコア
    }
    
    if (strstr(sentence2, sentence1) != NULL) {
        double len_ratio = (double)strlen(sentence1) / strlen(sentence2);
        return 0.7 + len_ratio * 0.3; // 部分一致の場合は高いスコア
    }
    
    // 単語レベルでの類似度を計算
    char s1_copy[MAX_QUESTION_LENGTH];
    char s2_copy[MAX_QUESTION_LENGTH];
    strncpy(s1_copy, sentence1, MAX_QUESTION_LENGTH - 1);
    strncpy(s2_copy, sentence2, MAX_QUESTION_LENGTH - 1);
    s1_copy[MAX_QUESTION_LENGTH - 1] = '\0';
    s2_copy[MAX_QUESTION_LENGTH - 1] = '\0';
    
    // 単語に分割
    char* words1[100];
    char* words2[100];
    int word_count1 = 0;
    int word_count2 = 0;
    
    char* word = strtok(s1_copy, " \t\n,.;:!?");
    while (word != NULL && word_count1 < 100) {
        words1[word_count1++] = word;
        word = strtok(NULL, " \t\n,.;:!?");
    }
    
    word = strtok(s2_copy, " \t\n,.;:!?");
    while (word != NULL && word_count2 < 100) {
        words2[word_count2++] = word;
        word = strtok(NULL, " \t\n,.;:!?");
    }
    
    // 単語間の類似度を計算
    double total_similarity = 0.0;
    int matches = 0;
    
    for (int i = 0; i < word_count1; i++) {
        double best_word_sim = 0.0;
        for (int j = 0; j < word_count2; j++) {
            double sim = word_similarity(words1[i], words2[j]);
            if (sim > best_word_sim) {
                best_word_sim = sim;
            }
        }
        if (best_word_sim > 0.5) { // 一定以上の類似度がある場合のみカウント
            total_similarity += best_word_sim;
            matches++;
        }
    }
    
    // 類似度のスコアを計算
    if (matches > 0) {
        return (total_similarity / matches) * ((double)matches / word_count1);
    }
    
    return 0.0;
}

// 質問に対する回答を検索し、類似度スコアも返す
const char* find_answer_with_score(const char* question, double* score) {
    int i;
    double best_score = 0.0;
    int best_match = -1;
    
    // 完全一致を検索
    for (i = 0; i < answer_db_size; i++) {
        if (strcmp(answer_db[i].question, question) == 0) {
            if (score) *score = 1.0; // 完全一致は最高スコア
            return answer_db[i].answer;
        }
    }
    
    // 類似度に基づく検索
    for (i = 0; i < answer_db_size; i++) {
        double current_score = sentence_similarity(question, answer_db[i].question);
        
        if (current_score > best_score) {
            best_score = current_score;
            best_match = i;
        }
    }
    
    // スコアを設定
    if (score) *score = best_score;
    
    // 一定以上の類似度がある場合のみ回答を返す
    if (best_score > 0.5 && best_match >= 0) {
        return answer_db[best_match].answer;
    }
    
    // 回答が見つからない場合
    return NULL;
}

// 質問に対する回答を検索する
const char* find_answer(const char* question) {
    return find_answer_with_score(question, NULL);
}

// 回答データベースのサイズを取得する
int get_answer_db_size() {
    return answer_db_size;
}