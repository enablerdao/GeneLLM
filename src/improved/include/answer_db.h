#ifndef ANSWER_DB_H
#define ANSWER_DB_H

// 回答データベースを初期化する
void init_answer_db();

// 質問に対する回答を検索する
const char* find_answer(const char* question);

// 回答データベースのサイズを取得する
int get_answer_db_size();

#endif // ANSWER_DB_H