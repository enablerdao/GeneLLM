/**
 * 簡易版DNAベース概念圧縮プログラム
 * 
 * 主語(E)、動詞(C)、結果(R)をそれぞれ1文字コードにし、
 * 番号を付けて圧縮するシンプルな仕組み
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LEN 256
#define MAX_DICT_SIZE 100
#define MAX_DNA_LEN 1024

// 概念タイプの定義
typedef enum {
    ENTITY,   // 主語
    CONCEPT,  // 動詞
    RESULT    // 結果
} ConceptType;

// 辞書エントリの構造体
typedef struct {
    char word[MAX_WORD_LEN];
    ConceptType type;
    int id;
} DictionaryEntry;

// グローバル変数
DictionaryEntry dictionary[MAX_DICT_SIZE];
int dict_size = 0;

/**
 * 辞書に単語を追加する
 * 
 * @param word 追加する単語
 * @param type 概念タイプ
 * @return 単語のID
 */
int add_to_dictionary(const char* word, ConceptType type) {
    // 既に辞書にある場合はそのIDを返す
    for (int i = 0; i < dict_size; i++) {
        if (strcmp(dictionary[i].word, word) == 0 && dictionary[i].type == type) {
            return dictionary[i].id;
        }
    }
    
    // 辞書が満杯の場合はエラー
    if (dict_size >= MAX_DICT_SIZE) {
        fprintf(stderr, "辞書が満杯です\n");
        return -1;
    }
    
    // 新しいエントリを追加
    strcpy(dictionary[dict_size].word, word);
    dictionary[dict_size].type = type;
    dictionary[dict_size].id = dict_size;
    
    return dict_size++;
}

/**
 * 概念タイプを文字に変換する
 * 
 * @param type 概念タイプ
 * @return 対応する文字
 */
char type_to_char(ConceptType type) {
    switch (type) {
        case ENTITY:  return 'E';
        case CONCEPT: return 'C';
        case RESULT:  return 'R';
        default:      return 'X';
    }
}

/**
 * 文字を概念タイプに変換する
 * 
 * @param c 文字
 * @return 対応する概念タイプ
 */
ConceptType char_to_type(char c) {
    switch (c) {
        case 'E': return ENTITY;
        case 'C': return CONCEPT;
        case 'R': return RESULT;
        default:  return ENTITY;  // デフォルト値
    }
}

/**
 * 文を圧縮する
 * 
 * @param entity 主語
 * @param concept 動詞
 * @param result 結果
 * @param dna_code 出力されるDNAコード
 * @return 成功した場合は1、失敗した場合は0
 */
int compress_sentence(const char* entity, const char* concept, const char* result, char* dna_code) {
    int entity_id = -1, concept_id = -1, result_id = -1;
    
    // 各要素を辞書に追加し、IDを取得
    if (entity != NULL && strlen(entity) > 0) {
        entity_id = add_to_dictionary(entity, ENTITY);
        if (entity_id < 0) return 0;
    }
    
    if (concept != NULL && strlen(concept) > 0) {
        concept_id = add_to_dictionary(concept, CONCEPT);
        if (concept_id < 0) return 0;
    }
    
    if (result != NULL && strlen(result) > 0) {
        result_id = add_to_dictionary(result, RESULT);
        if (result_id < 0) return 0;
    }
    
    // DNAコードを生成
    dna_code[0] = '\0';
    
    if (entity_id >= 0) {
        char entity_code[16];
        sprintf(entity_code, "E%02d", entity_id);
        strcat(dna_code, entity_code);
    }
    
    if (concept_id >= 0) {
        char concept_code[16];
        sprintf(concept_code, "C%02d", concept_id);
        strcat(dna_code, concept_code);
    }
    
    if (result_id >= 0) {
        char result_code[16];
        sprintf(result_code, "R%02d", result_id);
        strcat(dna_code, result_code);
    }
    
    return 1;
}

/**
 * DNAコードを展開する
 * 
 * @param dna_code DNAコード
 * @param sentence 出力される文
 * @return 成功した場合は1、失敗した場合は0
 */
int decompress_dna(const char* dna_code, char* sentence) {
    char entity[MAX_WORD_LEN] = "";
    char concept[MAX_WORD_LEN] = "";
    char result[MAX_WORD_LEN] = "";
    
    // DNAコードを解析
    const char* p = dna_code;
    while (*p) {
        char type_char = *p++;
        ConceptType type = char_to_type(type_char);
        
        // IDを取得
        if (*p < '0' || *p > '9') return 0;
        int id = (*p++ - '0') * 10;
        if (*p < '0' || *p > '9') return 0;
        id += (*p++ - '0');
        
        // IDが辞書の範囲内かチェック
        if (id < 0 || id >= dict_size) return 0;
        
        // 辞書から単語を取得
        if (dictionary[id].type != type) return 0;
        
        // 対応する変数に格納
        switch (type) {
            case ENTITY:
                strcpy(entity, dictionary[id].word);
                break;
            case CONCEPT:
                strcpy(concept, dictionary[id].word);
                break;
            case RESULT:
                strcpy(result, dictionary[id].word);
                break;
        }
    }
    
    // 文を生成
    sentence[0] = '\0';
    
    if (strlen(entity) > 0) {
        strcat(sentence, entity);
        strcat(sentence, "は");
    }
    
    if (strlen(concept) > 0) {
        strcat(sentence, concept);
        
        if (strlen(result) > 0) {
            strcat(sentence, "して");
        } else {
            strcat(sentence, "します");
        }
    }
    
    if (strlen(result) > 0) {
        strcat(sentence, result);
        strcat(sentence, "になりました");
    }
    
    return 1;
}

/**
 * 辞書の内容を表示する
 */
void print_dictionary() {
    printf("===== 辞書の内容 =====\n");
    for (int i = 0; i < dict_size; i++) {
        printf("%c%02d: %s\n", 
               type_to_char(dictionary[i].type),
               dictionary[i].id,
               dictionary[i].word);
    }
    printf("=====================\n");
}

int main() {
    char dna_code[MAX_DNA_LEN];
    char sentence[MAX_WORD_LEN * 3];
    
    // テスト用のデータ
    const char* test_entities[] = {"猫", "犬", "私"};
    const char* test_concepts[] = {"食べる", "走る", "考える"};
    const char* test_results[] = {"嬉しい", "疲れた", "賢くなった"};
    
    // テストケースを実行
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                // 文を圧縮
                if (!compress_sentence(test_entities[i], test_concepts[j], test_results[k], dna_code)) {
                    fprintf(stderr, "圧縮に失敗しました\n");
                    continue;
                }
                
                printf("入力: 主語=%s, 動詞=%s, 結果=%s\n", 
                       test_entities[i], test_concepts[j], test_results[k]);
                printf("圧縮: %s\n", dna_code);
                
                // DNAコードを展開
                if (!decompress_dna(dna_code, sentence)) {
                    fprintf(stderr, "展開に失敗しました\n");
                    continue;
                }
                
                printf("展開: %s\n\n", sentence);
            }
        }
    }
    
    // 辞書の内容を表示
    print_dictionary();
    
    return 0;
}