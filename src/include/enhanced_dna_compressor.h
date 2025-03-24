#ifndef ENHANCED_DNA_COMPRESSOR_H
#define ENHANCED_DNA_COMPRESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LEN 128
#define MAX_DNA_CODE_LEN 16
#define MAX_DICT_ENTRIES 1000

// 拡張DNAタイプの定義
typedef enum {
    ENTITY,     // E: 主語（名詞）
    CONCEPT,    // C: 動詞
    RESULT,     // R: 目的語（名詞）
    ATTRIBUTE,  // A: 属性（形容詞）
    TIME,       // T: 時間表現
    LOCATION,   // L: 場所表現
    MANNER,     // M: 様態（副詞）
    QUANTITY    // Q: 数量表現
} EnhancedDNAType;

// 拡張DNA辞書エントリの構造体
typedef struct {
    char code[MAX_DNA_CODE_LEN];
    char word[MAX_WORD_LEN];
    EnhancedDNAType type;
    int id;
} EnhancedDNAEntry;

// 拡張DNA辞書の構造体
typedef struct {
    EnhancedDNAEntry entries[MAX_DICT_ENTRIES];
    int count;
    int capacity;
} EnhancedDNADictionary;

// 拡張DNA辞書の初期化
void init_enhanced_dna_dictionary(EnhancedDNADictionary *dict);

// 単語からDNAコードを取得（なければ新規作成）
const char* get_enhanced_dna_code(EnhancedDNADictionary *dict, const char *word, EnhancedDNAType type);

// DNAコードから単語を取得
const char* get_word_from_enhanced_dna(EnhancedDNADictionary *dict, const char *code);

// 文を拡張DNA形式に圧縮
char* compress_to_enhanced_dna(EnhancedDNADictionary *dict, const char *text);

// 拡張DNA形式から文を再構築
char* decompress_from_enhanced_dna(EnhancedDNADictionary *dict, const char *dna_code);

// 拡張DNA辞書をファイルに保存
int save_enhanced_dna_dictionary(EnhancedDNADictionary *dict, const char *filename);

// ファイルから拡張DNA辞書を読み込む
int load_enhanced_dna_dictionary(EnhancedDNADictionary *dict, const char *filename);

// 拡張DNA辞書の内容を表示
void print_enhanced_dna_dictionary(EnhancedDNADictionary *dict);

#endif // ENHANCED_DNA_COMPRESSOR_H