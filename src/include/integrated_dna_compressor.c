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
    int type;         // 単語の種類（0: 主語, 1: 動詞, 2: 結果）
} DnaEntry;

// DNA圧縮用の辞書
typedef struct {
    DnaEntry* entries;    // エントリ配列
    int capacity;         // 辞書の容量
    int count;            // 現在のエントリ数
    int next_entity_id;   // 次の主語ID
    int next_concept_id;  // 次の動詞ID
    int next_result_id;   // 次の結果ID
    char filename[256];   // 辞書ファイル名
} DnaDictionary;

// 辞書の初期化
DnaDictionary* dna_dictionary_init(int initial_capacity, const char* filename) {
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
    
    // ファイル名を保存
    if (filename) {
        strncpy(dict->filename, filename, sizeof(dict->filename) - 1);
        dict->filename[sizeof(dict->filename) - 1] = '\0';
    } else {
        strcpy(dict->filename, "dna_dictionary.txt");
    }
    
    // エントリを初期化
    for (int i = 0; i < initial_capacity; i++) {
        dict->entries[i].used = 0;
        dict->entries[i].code[0] = '\0';
        dict->entries[i].word[0] = '\0';
        dict->entries[i].type = -1;
    }
    
    // 既存の辞書ファイルがあれば読み込む
    FILE* file = fopen(dict->filename, "r");
    if (file) {
        int count, next_entity_id, next_concept_id, next_result_id;
        if (fscanf(file, "%d %d %d %d\n", &count, &next_entity_id, 
                   &next_concept_id, &next_result_id) == 4) {
            
            dict->next_entity_id = next_entity_id;
            dict->next_concept_id = next_concept_id;
            dict->next_result_id = next_result_id;
            
            char code[8];
            char word[256];
            int type;
            
            for (int i = 0; i < count; i++) {
                if (fscanf(file, "%7s %255s %d\n", code, word, &type) != 3) {
                    break;
                }
                
                if (dict->count >= dict->capacity) {
                    int new_capacity = dict->capacity * 2;
                    DnaEntry* new_entries = (DnaEntry*)realloc(dict->entries, sizeof(DnaEntry) * new_capacity);
                    if (!new_entries) {
                        break;
                    }
                    dict->entries = new_entries;
                    dict->capacity = new_capacity;
                    
                    // 新しいエントリを初期化
                    for (int j = dict->count; j < new_capacity; j++) {
                        dict->entries[j].used = 0;
                        dict->entries[j].code[0] = '\0';
                        dict->entries[j].word[0] = '\0';
                        dict->entries[j].type = -1;
                    }
                }
                
                strncpy(dict->entries[dict->count].code, code, sizeof(dict->entries[dict->count].code) - 1);
                dict->entries[dict->count].code[sizeof(dict->entries[dict->count].code) - 1] = '\0';
                
                strncpy(dict->entries[dict->count].word, word, sizeof(dict->entries[dict->count].word) - 1);
                dict->entries[dict->count].word[sizeof(dict->entries[dict->count].word) - 1] = '\0';
                
                dict->entries[dict->count].type = type;
                dict->entries[dict->count].used = 1;
                dict->count++;
            }
        }
        fclose(file);
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
        dict->entries[i].type = -1;
    }
    
    dict->capacity = new_capacity;
    return 1;
}

// 辞書をファイルに保存
int dna_dictionary_save(DnaDictionary* dict);

// 単語からDNAコードを取得（なければ新規作成）
const char* dna_dictionary_get_code(DnaDictionary* dict, const char* word, char type) {
    if (!dict || !word || !*word) {
        return NULL;
    }
    
    int word_type;
    switch (type) {
        case 'E': word_type = 0; break; // 主語
        case 'C': word_type = 1; break; // 動詞
        case 'R': word_type = 2; break; // 結果
        default: return NULL;
    }
    
    // 既存のエントリを検索
    for (int i = 0; i < dict->count; i++) {
        if (dict->entries[i].used && 
            dict->entries[i].type == word_type && 
            strcmp(dict->entries[i].word, word) == 0) {
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
    
    dict->entries[dict->count].type = word_type;
    dict->entries[dict->count].used = 1;
    
    // 辞書をファイルに自動保存
    dna_dictionary_save(dict);
    
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

// DNA形式から文を再構築（自然な日本語文に）
char* dna_decompress_natural(DnaDictionary* dict, const char* dna_code) {
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
            strcat(text, "。");
            return text;
        }
    }
    
    if (result) {
        strcat(text, result);
        strcat(text, "になりました。");
    }
    
    return text;
}

// 辞書の内容を表示
void dna_dictionary_print(DnaDictionary* dict) {
    printf("DNA辞書の内容（%d/%d エントリ）:\n", dict->count, dict->capacity);
    
    printf("主語:\n");
    for (int i = 0; i < dict->count; i++) {
        if (dict->entries[i].used && dict->entries[i].type == 0) {
            printf("  %s: %s\n", dict->entries[i].code, dict->entries[i].word);
        }
    }
    
    printf("動詞:\n");
    for (int i = 0; i < dict->count; i++) {
        if (dict->entries[i].used && dict->entries[i].type == 1) {
            printf("  %s: %s\n", dict->entries[i].code, dict->entries[i].word);
        }
    }
    
    printf("結果:\n");
    for (int i = 0; i < dict->count; i++) {
        if (dict->entries[i].used && dict->entries[i].type == 2) {
            printf("  %s: %s\n", dict->entries[i].code, dict->entries[i].word);
        }
    }
}

// 辞書をファイルに保存
int dna_dictionary_save(DnaDictionary* dict) {
    if (!dict) {
        return 0;
    }
    
    FILE* file = fopen(dict->filename, "w");
    if (!file) {
        fprintf(stderr, "ファイルを開けませんでした: %s\n", dict->filename);
        return 0;
    }
    
    // ヘッダー情報を書き込み
    fprintf(file, "%d %d %d %d\n", dict->count, dict->next_entity_id, 
            dict->next_concept_id, dict->next_result_id);
    
    // エントリを書き込み
    for (int i = 0; i < dict->count; i++) {
        if (dict->entries[i].used) {
            fprintf(file, "%s %s %d\n", dict->entries[i].code, dict->entries[i].word, dict->entries[i].type);
        }
    }
    
    fclose(file);
    return 1;
}

// 自然言語文からDNA圧縮を行う（簡易構文解析）
char* natural_to_dna(DnaDictionary* dict, const char* text) {
    // 簡易的な構文解析（実際にはMeCabなどの形態素解析器を使うべき）
    char subject[256] = "";
    char verb[256] = "";
    char result[256] = "";
    
    // 非常に簡易的な解析（「〜は〜て〜になった」という形式を想定）
    char* ha = strstr(text, "は");
    if (ha) {
        // 主語を抽出
        size_t subject_len = ha - text;
        if (subject_len < sizeof(subject)) {
            strncpy(subject, text, subject_len);
            subject[subject_len] = '\0';
        }
        
        // 動詞を抽出（「て」の前まで）
        char* te = strstr(ha, "て");
        if (te) {
            size_t verb_len = te - (ha + 2); // 「は」の後から「て」の前まで
            if (verb_len < sizeof(verb)) {
                strncpy(verb, ha + 2, verb_len);
                verb[verb_len] = '\0';
                
                // 動詞の基本形に変換（て形から基本形へ）
                if (strlen(verb) > 0) {
                    strcat(verb, "る"); // 簡易的な変換（実際には活用形から基本形への変換が必要）
                }
            }
            
            // 結果を抽出（「て」の後から「になった」の前まで）
            char* ni = strstr(te, "になった");
            if (ni) {
                size_t result_len = ni - (te + 1); // 「て」の後から「になった」の前まで
                if (result_len < sizeof(result)) {
                    strncpy(result, te + 1, result_len);
                    result[result_len] = '\0';
                }
            }
        }
    }
    
    // DNA圧縮を実行
    return dna_compress(dict, subject, verb, result);
}

// テスト用のメイン関数
#ifdef TEST_INTEGRATED_DNA_COMPRESSOR
int main(int argc, char* argv[]) {
    // 辞書を初期化
    DnaDictionary* dict = dna_dictionary_init(10, "integrated_dna_dictionary.txt");
    if (!dict) {
        fprintf(stderr, "辞書の初期化に失敗しました\n");
        return 1;
    }
    
    if (argc >= 2) {
        // 最初の引数が「-n」の場合は自然言語文からDNA圧縮
        if (strcmp(argv[1], "-n") == 0 && argc >= 3) {
            const char* natural_text = argv[2];
            printf("入力文: %s\n\n", natural_text);
            
            char* dna_code = natural_to_dna(dict, natural_text);
            printf("DNA風圧縮: %s\n\n", dna_code);
            
            char* reconstructed = dna_decompress_natural(dict, dna_code);
            printf("再構築された文: %s\n\n", reconstructed);
            
            dna_dictionary_print(dict);
        }
        // 通常のDNA圧縮（主語、動詞、結果を指定）
        else if (argc >= 4) {
            const char* subject = argv[1];
            const char* verb = argv[2];
            const char* result = (argc >= 4) ? argv[3] : "";
            
            printf("入力:\n");
            printf("  主語 = %s\n", subject);
            printf("  動詞 = %s\n", verb);
            printf("  結果 = %s\n", result);
            printf("\n");
            
            // DNA圧縮を実行
            char* dna_code = dna_compress(dict, subject, verb, result);
            printf("DNA風圧縮: %s\n\n", dna_code);
            
            // 辞書の内容を表示
            dna_dictionary_print(dict);
            printf("\n");
            
            // 再構築（自然な日本語文に）
            char* reconstructed = dna_decompress_natural(dict, dna_code);
            printf("再構築された文: %s\n", reconstructed);
        } else {
            printf("使用法:\n");
            printf("  %s <主語> <動詞> [<結果>]  - 主語、動詞、結果からDNA圧縮\n", argv[0]);
            printf("  %s -n \"<自然言語文>\"      - 自然言語文からDNA圧縮\n", argv[0]);
        }
    } else {
        printf("使用法:\n");
        printf("  %s <主語> <動詞> [<結果>]  - 主語、動詞、結果からDNA圧縮\n", argv[0]);
        printf("  %s -n \"<自然言語文>\"      - 自然言語文からDNA圧縮\n", argv[0]);
    }
    
    // 辞書を解放
    dna_dictionary_free(dict);
    
    return 0;
}
#endif