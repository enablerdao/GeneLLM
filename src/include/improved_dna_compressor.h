#ifndef IMPROVED_DNA_COMPRESSOR_H
#define IMPROVED_DNA_COMPRESSOR_H

#include <stdbool.h>

// DNA圧縮用の辞書エントリ
typedef struct {
    char code[8];     // DNAコード（例：E00, C01, R02）
    char word[256];   // 対応する単語
    int used;         // 使用中フラグ
} DnaEntry;

// DNA圧縮用の辞書
typedef struct {
    DnaEntry* entries;    // エントリ配列
    int capacity;         // 辞書の容量
    int count;            // 現在のエントリ数
    int next_entity_id;   // 次の主語ID
    int next_concept_id;  // 次の動詞ID
    int next_result_id;   // 次の結果ID
} DnaDictionary;

// 辞書の初期化
DnaDictionary* dna_dictionary_init(int initial_capacity);

// 辞書の解放
void dna_dictionary_free(DnaDictionary* dict);

// 辞書の拡張
int dna_dictionary_expand(DnaDictionary* dict);

// 単語からDNAコードを取得（なければ新規作成）
const char* dna_dictionary_get_code(DnaDictionary* dict, const char* word, char type);

// DNAコードから単語を取得
const char* dna_dictionary_get_word(DnaDictionary* dict, const char* code);

// 文をDNA形式に圧縮
char* dna_compress(DnaDictionary* dict, const char* subject, const char* verb, const char* result);

// DNA形式から文を再構築（改良版）
char* dna_decompress_improved(DnaDictionary* dict, const char* dna_code);

// 辞書の内容を表示
void dna_dictionary_print(DnaDictionary* dict);

// 辞書をファイルに保存
int dna_dictionary_save(DnaDictionary* dict, const char* filename);

// ファイルから辞書を読み込み
DnaDictionary* dna_dictionary_load(const char* filename);

#endif // IMPROVED_DNA_COMPRESSOR_H