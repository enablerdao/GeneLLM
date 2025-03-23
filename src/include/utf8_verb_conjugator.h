#ifndef UTF8_VERB_CONJUGATOR_H
#define UTF8_VERB_CONJUGATOR_H

#include <stdbool.h>

// 動詞の活用形
typedef enum {
    VERB_FORM_BASIC,      // 基本形（辞書形）: 食べる
    VERB_FORM_MASU,       // ます形: 食べます
    VERB_FORM_TE,         // て形: 食べて
    VERB_FORM_TA,         // た形: 食べた
    VERB_FORM_NAI,        // ない形: 食べない
    VERB_FORM_PASSIVE,    // 受身形: 食べられる
    VERB_FORM_CAUSATIVE,  // 使役形: 食べさせる
    VERB_FORM_POTENTIAL,  // 可能形: 食べられる
    VERB_FORM_IMPERATIVE, // 命令形: 食べろ/食べよ
    VERB_FORM_VOLITIONAL  // 意志形: 食べよう
} VerbForm;

// 動詞の種類
typedef enum {
    VERB_TYPE_GODAN,      // 五段動詞: 書く、読む
    VERB_TYPE_ICHIDAN,    // 一段動詞: 食べる、見る
    VERB_TYPE_IRREGULAR,  // 不規則動詞: する、来る
    VERB_TYPE_UNKNOWN     // 不明
} VerbType;

// 動詞の活用情報
typedef struct {
    char basic[256];      // 基本形
    VerbType type;        // 動詞の種類
    char stem[256];       // 語幹
    char ending[8];       // 語尾（UTF-8対応のため文字列として扱う）
} VerbInfo;

// UTF-8文字列の最後の文字を取得
bool get_last_utf8_char(const char* str, char* last_char);

// UTF-8文字列の最後の文字を除いた部分を取得
void get_utf8_except_last(const char* str, char* result, size_t result_size);

// 動詞の種類を判定
VerbType determine_verb_type(const char* verb);

// 動詞の情報を解析
void analyze_verb(const char* verb, VerbInfo* info);

// 五段動詞の活用語尾を取得
const char* get_godan_ending(const char* base, VerbForm form);

// 動詞を活用させる
char* conjugate_verb(const char* verb, VerbForm form);

// 動詞の種類を文字列で取得
const char* get_verb_type_name(VerbType type);

// 活用形を文字列で取得
const char* get_verb_form_name(VerbForm form);

#endif // UTF8_VERB_CONJUGATOR_H