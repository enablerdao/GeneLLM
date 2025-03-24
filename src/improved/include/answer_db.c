#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <strings.h>

#define MAX_ANSWERS 100
#define MAX_QUESTION_LENGTH 256
#define MAX_ANSWER_LENGTH 4096

typedef struct {
    char question[MAX_QUESTION_LENGTH];
    char answer[MAX_ANSWER_LENGTH];
} QAPair;

static QAPair answer_db[MAX_ANSWERS];
static int answer_db_size = 0;

// マッチした質問を保存する変数
static char matched_question[MAX_QUESTION_LENGTH];

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

// 単語の類似度を計算する簡易関数（改良版）
double word_similarity(const char* word1, const char* word2) {
    int len1 = strlen(word1);
    int len2 = strlen(word2);
    
    // 同じ単語なら最高スコア
    if (strcasecmp(word1, word2) == 0) {
        return 1.0;
    }
    
    // 長さが大きく異なる場合は類似度を下げる
    double len_ratio = (double)len1 / len2;
    if (len_ratio > 1.0) len_ratio = 1.0 / len_ratio;
    
    // 共通の文字数をカウント（位置も考慮）
    int common_chars = 0;
    int position_matches = 0;
    
    for (int i = 0; i < len1; i++) {
        for (int j = 0; j < len2; j++) {
            if (tolower(word1[i]) == tolower(word2[j])) {
                common_chars++;
                // 同じ相対位置にある文字は高いスコア
                if (fabs((double)i / len1 - (double)j / len2) < 0.3) {
                    position_matches++;
                }
                break;
            }
        }
    }
    
    // 先頭文字が一致する場合はボーナス
    double prefix_bonus = (tolower(word1[0]) == tolower(word2[0])) ? 0.2 : 0.0;
    
    // 類似度を計算（共通文字の割合、位置の一致、長さの比率を考慮）
    double char_ratio = (double)common_chars / len1;
    double position_ratio = (len1 > 0) ? (double)position_matches / len1 : 0;
    
    return char_ratio * 0.4 + position_ratio * 0.3 + len_ratio * 0.1 + prefix_bonus;
}

// 文章の類似度を計算する（改良版）
double sentence_similarity(const char* sentence1, const char* sentence2) {
    // 完全一致の場合
    if (strcasecmp(sentence1, sentence2) == 0) {
        return 1.0;
    }
    
    // 部分文字列の場合（大文字小文字を区別しない）
    char s1_lower[MAX_QUESTION_LENGTH];
    char s2_lower[MAX_QUESTION_LENGTH];
    
    strncpy(s1_lower, sentence1, MAX_QUESTION_LENGTH - 1);
    strncpy(s2_lower, sentence2, MAX_QUESTION_LENGTH - 1);
    s1_lower[MAX_QUESTION_LENGTH - 1] = '\0';
    s2_lower[MAX_QUESTION_LENGTH - 1] = '\0';
    
    // 小文字に変換
    for (int i = 0; s1_lower[i]; i++) {
        s1_lower[i] = tolower(s1_lower[i]);
    }
    
    for (int i = 0; s2_lower[i]; i++) {
        s2_lower[i] = tolower(s2_lower[i]);
    }
    
    // 数字で始まる質問と人名を含む質問は特別扱い
    // 例: "1. 新しい質問タイプの追加" と "濱田優貴とは誰ですか？" は類似していないと判断
    bool s1_starts_with_number = isdigit(s1_lower[0]);
    bool s2_starts_with_number = isdigit(s2_lower[0]);
    
    // 人名を含むかどうかをチェック
    bool s1_has_name = strstr(s1_lower, "濱田") != NULL || strstr(s1_lower, "はまだ") != NULL;
    bool s2_has_name = strstr(s2_lower, "濱田") != NULL || strstr(s2_lower, "はまだ") != NULL;
    
    // 数字で始まる質問と人名を含む質問は類似度を下げる
    if ((s1_starts_with_number && s2_has_name) || (s2_starts_with_number && s1_has_name)) {
        return 0.1; // 非常に低い類似度を返す
    }
    
    if (strstr(s1_lower, s2_lower) != NULL) {
        double len_ratio = (double)strlen(sentence2) / strlen(sentence1);
        return 0.7 + len_ratio * 0.3; // 部分一致の場合は高いスコア
    }
    
    if (strstr(s2_lower, s1_lower) != NULL) {
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
        // 短すぎる単語（助詞など）は重要度を下げる
        if (strlen(word) > 1) {
            words1[word_count1++] = word;
        }
        word = strtok(NULL, " \t\n,.;:!?");
    }
    
    word = strtok(s2_copy, " \t\n,.;:!?");
    while (word != NULL && word_count2 < 100) {
        if (strlen(word) > 1) {
            words2[word_count2++] = word;
        }
        word = strtok(NULL, " \t\n,.;:!?");
    }
    
    // 単語間の類似度を計算
    double total_similarity = 0.0;
    int matches = 0;
    
    // 各単語の最良マッチを見つける
    for (int i = 0; i < word_count1; i++) {
        double best_word_sim = 0.0;
        int best_match_idx = -1;
        
        for (int j = 0; j < word_count2; j++) {
            double sim = word_similarity(words1[i], words2[j]);
            if (sim > best_word_sim) {
                best_word_sim = sim;
                best_match_idx = j;
            }
        }
        
        // 一定以上の類似度がある場合のみカウント
        if (best_word_sim > 0.6) {
            // 単語の長さに応じて重み付け（長い単語ほど重要）
            double word_weight = 0.5 + 0.5 * fmin(1.0, strlen(words1[i]) / 10.0);
            total_similarity += best_word_sim * word_weight;
            matches++;
            
            // 単語の位置も考慮（近い位置にある単語ほど重要）
            if (best_match_idx >= 0) {
                double position_ratio = 1.0 - fabs((double)i / word_count1 - (double)best_match_idx / word_count2);
                total_similarity += position_ratio * 0.2;
            }
        }
    }
    
    // 類似度のスコアを計算
    if (matches > 0) {
        // マッチした単語の割合と平均類似度を考慮
        double match_ratio = (double)matches / word_count1;
        double avg_similarity = total_similarity / matches;
        
        // 短い質問に対するボーナス
        double length_bonus = (word_count1 < 5) ? 0.1 : 0.0;
        
        return avg_similarity * 0.6 + match_ratio * 0.4 + length_bonus;
    }
    
    return 0.0;
}

// 特定のキーワードに基づいて質問をマッチングする
int find_keyword_match(const char* question, const char* keyword) {
    char question_lower[MAX_QUESTION_LENGTH];
    strncpy(question_lower, question, MAX_QUESTION_LENGTH - 1);
    question_lower[MAX_QUESTION_LENGTH - 1] = '\0';
    
    // 小文字に変換
    for (int i = 0; question_lower[i]; i++) {
        question_lower[i] = tolower(question_lower[i]);
    }
    
    // キーワードを小文字に変換
    char keyword_lower[MAX_QUESTION_LENGTH];
    strncpy(keyword_lower, keyword, MAX_QUESTION_LENGTH - 1);
    keyword_lower[MAX_QUESTION_LENGTH - 1] = '\0';
    
    for (int i = 0; keyword_lower[i]; i++) {
        keyword_lower[i] = tolower(keyword_lower[i]);
    }
    
    // キーワードを含む質問を検索
    if (strstr(question_lower, keyword_lower) != NULL) {
        for (int i = 0; i < answer_db_size; i++) {
            char db_question_lower[MAX_QUESTION_LENGTH];
            strncpy(db_question_lower, answer_db[i].question, MAX_QUESTION_LENGTH - 1);
            db_question_lower[MAX_QUESTION_LENGTH - 1] = '\0';
            
            for (int j = 0; db_question_lower[j]; j++) {
                db_question_lower[j] = tolower(db_question_lower[j]);
            }
            
            if (strstr(db_question_lower, keyword_lower) != NULL) {
                return i;
            }
        }
    }
    
    return -1;
}

// 質問に対する回答を検索し、類似度スコアと選択された質問も返す
const char* find_answer_with_score(const char* question, double* score) {
    int i;
    double best_score = 0.0;
    int best_match = -1;
    
    // 特定のキーワードに基づく優先マッチング
    struct {
        const char* keyword;
        double match_score;
    } keywords[] = {
        {"ポインタ", 0.95},
        {"構造体", 0.95},
        {"配列", 0.95},
        {"メモリ", 0.95},
        {"関数", 0.95},
        {"ファイル", 0.95},
        {"エラー", 0.95},
        {"デバッグ", 0.95},
        {NULL, 0.0}
    };
    
    for (i = 0; keywords[i].keyword != NULL; i++) {
        int match_idx = find_keyword_match(question, keywords[i].keyword);
        if (match_idx >= 0) {
            if (score) *score = keywords[i].match_score;
            strncpy(matched_question, answer_db[match_idx].question, MAX_QUESTION_LENGTH - 1);
            matched_question[MAX_QUESTION_LENGTH - 1] = '\0';
            return answer_db[match_idx].answer;
        }
    }

    // 完全一致を検索
    for (i = 0; i < answer_db_size; i++) {
        if (strcmp(answer_db[i].question, question) == 0) {
            if (score) *score = 1.0; // 完全一致は最高スコア
            strncpy(matched_question, answer_db[i].question, MAX_QUESTION_LENGTH - 1);
            matched_question[MAX_QUESTION_LENGTH - 1] = '\0';
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
        strncpy(matched_question, answer_db[best_match].question, MAX_QUESTION_LENGTH - 1);
        matched_question[MAX_QUESTION_LENGTH - 1] = '\0';
        return answer_db[best_match].answer;
    }

    // 回答が見つからない場合
    matched_question[0] = '\0';
    return NULL;
}

// マッチした質問を取得する
const char* get_matched_question() {
    return matched_question;
}

// 質問に対する回答を検索する
const char* find_answer(const char* question) {
    return find_answer_with_score(question, NULL);
}

// 回答データベースのサイズを取得する
int get_answer_db_size() {
    return answer_db_size;
}
