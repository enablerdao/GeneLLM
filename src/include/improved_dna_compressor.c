#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utf8_verb_conjugator.h"

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
DnaDictionary* dna_dictionary_init(int initial_capacity) {
    DnaDictionary* dict = (DnaDictionary*)malloc(sizeof(DnaDictionary));
    if (!dict) {
        fprintf(stderr, "メモリ割り当てエラー: 辞書の初期化に失敗しました\n");
        return NULL;
    }
    
    dict->entries = (DnaEntry*)malloc(sizeof(DnaEntry) * initial_capacity);
    if (!dict->entries) {
        fprintf(stderr, "メモリ割り当てエラー: 辞書エントリの初期化に失敗しました\n");
        free(dict);
        return NULL;
    }
    
    dict->capacity = initial_capacity;
    dict->count = 0;
    dict->next_entity_id = 0;
    dict->next_concept_id = 0;
    dict->next_result_id = 0;
    
    // エントリを初期化
    for (int i = 0; i < initial_capacity; i++) {
        dict->entries[i].used = 0;
        dict->entries[i].code[0] = '\0';
        dict->entries[i].word[0] = '\0';
    }
    
    return dict;
}

// 辞書の解放
void dna_dictionary_free(DnaDictionary* dict) {
    if (dict) {
        if (dict->entries) {
            free(dict->entries);
        }
        free(dict);
    }
}

// 辞書の拡張
int dna_dictionary_expand(DnaDictionary* dict) {
    int new_capacity = dict->capacity * 2;
    DnaEntry* new_entries = (DnaEntry*)realloc(dict->entries, sizeof(DnaEntry) * new_capacity);
    if (!new_entries) {
        fprintf(stderr, "メモリ割り当てエラー: 辞書の拡張に失敗しました\n");
        return 0;
    }
    
    dict->entries = new_entries;
    
    // 新しいエントリを初期化
    for (int i = dict->capacity; i < new_capacity; i++) {
        dict->entries[i].used = 0;
        dict->entries[i].code[0] = '\0';
        dict->entries[i].word[0] = '\0';
    }
    
    dict->capacity = new_capacity;
    return 1;
}

// 単語からDNAコードを取得（なければ新規作成）
const char* dna_dictionary_get_code(DnaDictionary* dict, const char* word, char type) {
    if (!dict || !word || !*word) {
        return NULL;
    }
    
    // 既存のエントリを検索
    for (int i = 0; i < dict->count; i++) {
        if (dict->entries[i].used && strcmp(dict->entries[i].word, word) == 0) {
            return dict->entries[i].code;
        }
    }
    
    // 新しいエントリを作成
    if (dict->count >= dict->capacity) {
        if (!dna_dictionary_expand(dict)) {
            return NULL;
        }
    }
    
    int id;
    switch (type) {
        case 'E': // Entity（主語）
            id = dict->next_entity_id++;
            break;
        case 'C': // Concept（動詞）
            id = dict->next_concept_id++;
            break;
        case 'R': // Result（結果）
            id = dict->next_result_id++;
            break;
        default:
            fprintf(stderr, "無効なDNAタイプ: %c\n", type);
            return NULL;
    }
    
    // コードを生成
    snprintf(dict->entries[dict->count].code, sizeof(dict->entries[dict->count].code), 
             "%c%02d", type, id);
    
    // 単語をコピー
    strncpy(dict->entries[dict->count].word, word, sizeof(dict->entries[dict->count].word) - 1);
    dict->entries[dict->count].word[sizeof(dict->entries[dict->count].word) - 1] = '\0';
    
    dict->entries[dict->count].used = 1;
    
    return dict->entries[dict->count++].code;
}

// DNAコードから単語を取得
const char* dna_dictionary_get_word(DnaDictionary* dict, const char* code) {
    if (!dict || !code || !*code) {
        return NULL;
    }
    
    for (int i = 0; i < dict->count; i++) {
        if (dict->entries[i].used && strcmp(dict->entries[i].code, code) == 0) {
            return dict->entries[i].word;
        }
    }
    
    return NULL;
}

// 文をDNA形式に圧縮
char* dna_compress(DnaDictionary* dict, const char* subject, const char* verb, const char* result) {
    static char dna_code[256];
    dna_code[0] = '\0';
    
    if (subject && *subject) {
        const char* subject_code = dna_dictionary_get_code(dict, subject, 'E');
        if (subject_code) {
            strcat(dna_code, subject_code);
        }
    }
    
    if (verb && *verb) {
        const char* verb_code = dna_dictionary_get_code(dict, verb, 'C');
        if (verb_code) {
            strcat(dna_code, verb_code);
        }
    }
    
    if (result && *result) {
        const char* result_code = dna_dictionary_get_code(dict, result, 'R');
        if (result_code) {
            strcat(dna_code, result_code);
        }
    }
    
    return dna_code;
}

// DNA形式から文を再構築（改良版）
char* dna_decompress_improved(DnaDictionary* dict, const char* dna_code) {
    static char text[1024];
    text[0] = '\0';
    
    char code[8];
    const char* subject = NULL;
    const char* verb = NULL;
    const char* result = NULL;
    
    // DNAコードを解析して単語を取得
    size_t code_len = strlen(dna_code);
    for (size_t i = 0; i < code_len; i += 3) {
        if (i + 3 <= code_len) {
            strncpy(code, dna_code + i, 3);
            code[3] = '\0';
            
            const char* word = dna_dictionary_get_word(dict, code);
            if (word) {
                if (code[0] == 'E') { // 主語
                    subject = word;
                } else if (code[0] == 'C') { // 動詞
                    verb = word;
                } else if (code[0] == 'R') { // 結果
                    result = word;
                }
            }
        }
    }
    
    // 文を構築
    if (subject) {
        strcat(text, subject);
        strcat(text, "は");
    }
    
    if (verb) {
        // 動詞の活用形を決定
        if (result) {
            // 結果がある場合はて形
            char* conjugated = conjugate_verb(verb, VERB_FORM_TE);
            strcat(text, conjugated);
        } else {
            // 結果がない場合はた形
            char* conjugated = conjugate_verb(verb, VERB_FORM_TA);
            strcat(text, conjugated);
        }
    }
    
    if (result) {
        strcat(text, result);
        strcat(text, "になりました");
    }
    
    return text;
}

// 辞書の内容を表示
void dna_dictionary_print(DnaDictionary* dict) {
    printf("DNA辞書の内容（%d/%d エントリ）:\n", dict->count, dict->capacity);
    for (int i = 0; i < dict->count; i++) {
        if (dict->entries[i].used) {
            printf("  %s: %s\n", dict->entries[i].code, dict->entries[i].word);
        }
    }
}

// 辞書をファイルに保存
int dna_dictionary_save(DnaDictionary* dict, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "ファイルを開けませんでした: %s\n", filename);
        return 0;
    }
    
    // ヘッダー情報を書き込み
    fprintf(file, "%d %d %d %d\n", dict->count, dict->next_entity_id, 
            dict->next_concept_id, dict->next_result_id);
    
    // エントリを書き込み
    for (int i = 0; i < dict->count; i++) {
        if (dict->entries[i].used) {
            fprintf(file, "%s %s\n", dict->entries[i].code, dict->entries[i].word);
        }
    }
    
    fclose(file);
    return 1;
}

// ファイルから辞書を読み込み
DnaDictionary* dna_dictionary_load(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "ファイルを開けませんでした: %s\n", filename);
        return NULL;
    }
    
    int count, next_entity_id, next_concept_id, next_result_id;
    if (fscanf(file, "%d %d %d %d\n", &count, &next_entity_id, 
               &next_concept_id, &next_result_id) != 4) {
        fprintf(stderr, "ファイルフォーマットが無効です: %s\n", filename);
        fclose(file);
        return NULL;
    }
    
    DnaDictionary* dict = dna_dictionary_init(count > 0 ? count : 10);
    if (!dict) {
        fclose(file);
        return NULL;
    }
    
    dict->next_entity_id = next_entity_id;
    dict->next_concept_id = next_concept_id;
    dict->next_result_id = next_result_id;
    
    char code[8];
    char word[256];
    
    for (int i = 0; i < count; i++) {
        if (fscanf(file, "%7s %255s\n", code, word) != 2) {
            break;
        }
        
        if (dict->count >= dict->capacity) {
            if (!dna_dictionary_expand(dict)) {
                break;
            }
        }
        
        strncpy(dict->entries[dict->count].code, code, sizeof(dict->entries[dict->count].code) - 1);
        dict->entries[dict->count].code[sizeof(dict->entries[dict->count].code) - 1] = '\0';
        
        strncpy(dict->entries[dict->count].word, word, sizeof(dict->entries[dict->count].word) - 1);
        dict->entries[dict->count].word[sizeof(dict->entries[dict->count].word) - 1] = '\0';
        
        dict->entries[dict->count].used = 1;
        dict->count++;
    }
    
    fclose(file);
    return dict;
}

// テスト用のメイン関数
#ifdef TEST_IMPROVED_DNA_COMPRESSOR
int main(int argc, char* argv[]) {
    // 辞書を初期化
    DnaDictionary* dict = dna_dictionary_init(10);
    if (!dict) {
        fprintf(stderr, "辞書の初期化に失敗しました\n");
        return 1;
    }
    
    // コマンドライン引数から単語を取得
    if (argc >= 4) {
        const char* subject = argv[1];
        const char* verb = argv[2];
        const char* result = argv[3];
        
        printf("入力:\n");
        printf("  主語 = %s\n", subject);
        printf("  動詞 = %s\n", verb);
        printf("  結果 = %s\n", result);
        printf("\n");
        
        // DNA圧縮を実行
        char* dna_code = dna_compress(dict, subject, verb, result);
        printf("DNA風圧縮: %s\n", dna_code);
        
        // 辞書の内容を表示
        printf("\n");
        dna_dictionary_print(dict);
        
        // 再構築（改良版）
        char* reconstructed = dna_decompress_improved(dict, dna_code);
        printf("\n");
        printf("再構築された文（改良版）: %s\n", reconstructed);
        
        // 辞書をファイルに保存
        if (dna_dictionary_save(dict, "improved_dna_dictionary.txt")) {
            printf("\n");
            printf("辞書をファイルに保存しました: improved_dna_dictionary.txt\n");
        }
    } else {
        printf("使用法: %s <主語> <動詞> <結果>\n", argv[0]);
        printf("例: %s 猫 食べる 嬉しい\n", argv[0]);
    }
    
    // 辞書を解放
    dna_dictionary_free(dict);
    
    return 0;
}
#endif