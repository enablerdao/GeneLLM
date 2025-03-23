#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STR_LEN 1024
#define MAX_DICT_SIZE 1000

// 辞書エントリ
typedef struct {
    char code[10];     // E00, C01, R02 などのコード
    char value[MAX_STR_LEN]; // 実際の値（猫、食べる、嬉しいなど）
} DictEntry;

// 辞書
typedef struct {
    DictEntry entries[MAX_DICT_SIZE];
    int size;
} Dictionary;

// 辞書の初期化
void init_dictionary(Dictionary* dict) {
    dict->size = 0;
}

// 辞書に追加
void add_to_dictionary(Dictionary* dict, const char* code, const char* value) {
    if (dict->size < MAX_DICT_SIZE) {
        strcpy(dict->entries[dict->size].code, code);
        strcpy(dict->entries[dict->size].value, value);
        dict->size++;
    }
}

// 辞書から検索
const char* lookup_in_dictionary(Dictionary* dict, const char* code) {
    for (int i = 0; i < dict->size; i++) {
        if (strcmp(dict->entries[i].code, code) == 0) {
            return dict->entries[i].value;
        }
    }
    return NULL;
}

// 辞書にコードが存在するかチェック
bool code_exists(Dictionary* dict, const char* code) {
    return lookup_in_dictionary(dict, code) != NULL;
}

// 辞書に値が存在するかチェック
bool value_exists(Dictionary* dict, const char* value, char* existing_code) {
    for (int i = 0; i < dict->size; i++) {
        if (strcmp(dict->entries[i].value, value) == 0) {
            if (existing_code) {
                strcpy(existing_code, dict->entries[i].code);
            }
            return true;
        }
    }
    return false;
}

// 主語、動詞、結果をDNA風に圧縮する
void compress_to_dna(Dictionary* dict, const char* subject, const char* verb, const char* result, char* dna_code) {
    char subject_code[10] = "";
    char verb_code[10] = "";
    char result_code[10] = "";
    
    // 主語のコード生成
    if (strlen(subject) > 0) {
        if (value_exists(dict, subject, subject_code)) {
            // 既存のコードを使用
        } else {
            // 新しいコードを生成
            sprintf(subject_code, "E%02d", dict->size);
            add_to_dictionary(dict, subject_code, subject);
        }
    }
    
    // 動詞のコード生成
    if (strlen(verb) > 0) {
        if (value_exists(dict, verb, verb_code)) {
            // 既存のコードを使用
        } else {
            // 新しいコードを生成
            sprintf(verb_code, "C%02d", dict->size);
            add_to_dictionary(dict, verb_code, verb);
        }
    }
    
    // 結果のコード生成
    if (strlen(result) > 0) {
        if (value_exists(dict, result, result_code)) {
            // 既存のコードを使用
        } else {
            // 新しいコードを生成
            sprintf(result_code, "R%02d", dict->size);
            add_to_dictionary(dict, result_code, result);
        }
    }
    
    // DNA風コードの生成
    strcpy(dna_code, "");
    if (strlen(subject_code) > 0) {
        strcat(dna_code, subject_code);
    }
    if (strlen(verb_code) > 0) {
        strcat(dna_code, verb_code);
    }
    if (strlen(result_code) > 0) {
        strcat(dna_code, result_code);
    }
}

// DNA風コードから元の文を再構築する
void decompress_from_dna(Dictionary* dict, const char* dna_code, char* reconstructed) {
    char code[10] = "";
    int i = 0;
    size_t dna_len = strlen(dna_code);
    
    strcpy(reconstructed, "");
    
    while ((size_t)i < dna_len) {
        // コードを抽出（E00, C01, R02 など）
        code[0] = dna_code[i++];
        code[1] = dna_code[i++];
        code[2] = dna_code[i++];
        code[3] = '\0';
        
        // 辞書から値を取得
        const char* value = lookup_in_dictionary(dict, code);
        if (value) {
            if (code[0] == 'E') { // 主語
                strcat(reconstructed, value);
                strcat(reconstructed, "は");
            } else if (code[0] == 'C') { // 動詞
                strcat(reconstructed, value);
                // 動詞の語尾を調整
                if (strstr(value, "る")) {
                    reconstructed[strlen(reconstructed) - 2] = '\0'; // 「る」を削除
                    strcat(reconstructed, "て");
                } else {
                    strcat(reconstructed, "して");
                }
            } else if (code[0] == 'R') { // 結果
                strcat(reconstructed, value);
                strcat(reconstructed, "になった");
            }
        }
    }
    
    // 最後に句点を追加
    strcat(reconstructed, "。");
}

// 辞書の内容を表示
void print_dictionary(Dictionary* dict) {
    printf("辞書内容:\n");
    for (int i = 0; i < dict->size; i++) {
        printf("%s: %s\n", dict->entries[i].code, dict->entries[i].value);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("使用方法: %s <主語> <動詞> <結果>\n", argv[0]);
        return 1;
    }
    
    const char* subject = argv[1];
    const char* verb = argv[2];
    const char* result = argv[3];
    
    Dictionary dict;
    init_dictionary(&dict);
    
    char dna_code[MAX_STR_LEN] = "";
    compress_to_dna(&dict, subject, verb, result, dna_code);
    
    char reconstructed[MAX_STR_LEN] = "";
    decompress_from_dna(&dict, dna_code, reconstructed);
    
    printf("入力:\n");
    printf("主語 = %s\n", subject);
    printf("動詞 = %s\n", verb);
    printf("結果 = %s\n", result);
    printf("\n");
    
    printf("DNA風圧縮: %s\n", dna_code);
    printf("\n");
    
    print_dictionary(&dict);
    printf("\n");
    
    printf("再構築された文: %s\n", reconstructed);
    
    return 0;
}