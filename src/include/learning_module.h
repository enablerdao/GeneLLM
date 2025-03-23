#ifndef LEARNING_MODULE_H
#define LEARNING_MODULE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 学習データの構造体
typedef struct {
    char question[1024];
    char answer[2048];
    float confidence;
} LearningEntry;

// 学習データベース
typedef struct {
    LearningEntry* entries;
    int count;
    int capacity;
    char filename[256];
} LearningDB;

// 学習データベースの初期化
LearningDB* learning_db_init(const char* filename);

// 学習データベースの解放
void learning_db_free(LearningDB* db);

// 学習データの追加
bool learning_db_add(LearningDB* db, const char* question, const char* answer, float confidence);

// 学習データの検索
bool learning_db_find(LearningDB* db, const char* question, char* answer, float* confidence);

// 学習データの保存
bool learning_db_save(LearningDB* db);

// 学習データの読み込み
bool learning_db_load(LearningDB* db);

// 質問の類似度計算
float calculate_question_similarity(const char* q1, const char* q2);

#endif // LEARNING_MODULE_H