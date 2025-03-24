#ifndef ANSWER_DB_H
#define ANSWER_DB_H

// 回答データベースを初期化する
void init_answer_db();

// 質問に対する回答を検索する
const char* find_answer(const char* question);

// 質問に対する回答を検索し、類似度スコアも返す
const char* find_answer_with_score(const char* question, double* score);

// マッチした質問を取得する
const char* get_matched_question();

// 回答データベースのサイズを取得する
int get_answer_db_size();

#endif // ANSWER_DB_H