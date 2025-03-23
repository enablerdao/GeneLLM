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

// 日本語の文から主語、動詞、結果を抽出する
void extract_japanese_elements(const char* sentence, char* subject, char* verb, char* result) {
    // 作業用のバッファにコピー
    char buffer[MAX_STR_LEN];
    strncpy(buffer, sentence, MAX_STR_LEN - 1);
    buffer[MAX_STR_LEN - 1] = '\0';
    
    // 句読点を削除
    char* p = buffer;
    while (*p) {
        if (strncmp(p, "。", 3) == 0 || 
            strncmp(p, "、", 3) == 0 || 
            strncmp(p, "！", 3) == 0 || 
            strncmp(p, "？", 3) == 0) {
            *p = '\0';
        }
        p++;
    }
    
    // 主語の抽出（「が」「は」の前まで）
    char* ga = strstr(buffer, "が");
    char* wa = strstr(buffer, "は");
    char* subject_marker = NULL;
    
    if (ga && (!wa || ga < wa)) {
        subject_marker = ga;
    } else if (wa) {
        subject_marker = wa;
    }
    
    if (subject_marker) {
        int len = subject_marker - buffer;
        strncpy(subject, buffer, len);
        subject[len] = '\0';
    } else {
        // 主語マーカーがない場合は、最初の単語を主語と仮定
        strcpy(subject, "");
        char* first_space = strchr(buffer, ' ');
        if (first_space) {
            int len = first_space - buffer;
            strncpy(subject, buffer, len);
            subject[len] = '\0';
        } else {
            // スペースがない場合は、最初の数文字を主語と仮定
            int len = strlen(buffer) > 3 ? 3 : strlen(buffer);
            strncpy(subject, buffer, len);
            subject[len] = '\0';
        }
    }
    
    // 動詞の抽出（「を」の後ろから「て」の前まで）
    char* wo = strstr(buffer, "を");
    char* te = strstr(buffer, "て");
    
    if (wo && te && wo < te) {
        int start = wo - buffer + 3; // 「を」の後ろ
        int len = te - buffer - start;
        strncpy(verb, buffer + start, len);
        verb[len] = '\0';
        
        // 「食べ」のような形になっているので、「る」を追加
        strcat(verb, "る");
    } else {
        // 特定の動詞を検出
        if (strstr(buffer, "食べ")) {
            strcpy(verb, "食べる");
        } else if (strstr(buffer, "飲み")) {
            strcpy(verb, "飲む");
        } else if (strstr(buffer, "見")) {
            strcpy(verb, "見る");
        } else if (strstr(buffer, "行き")) {
            strcpy(verb, "行く");
        } else if (strstr(buffer, "来")) {
            strcpy(verb, "来る");
        } else {
            strcpy(verb, "する"); // デフォルト
        }
    }
    
    // 結果の抽出（「て」の後ろ）
    if (te) {
        int start = te - buffer + 3; // 「て」の後ろ
        strcpy(result, buffer + start);
        
        // 「嬉しかっ」のような形になっているので、「た」を追加
        if (strstr(result, "かっ") && !strstr(result, "かった")) {
            strcat(result, "た");
        }
    } else {
        // 特定の結果を検出
        if (strstr(buffer, "嬉し")) {
            strcpy(result, "嬉しい");
        } else if (strstr(buffer, "悲し")) {
            strcpy(result, "悲しい");
        } else if (strstr(buffer, "楽し")) {
            strcpy(result, "楽しい");
        } else {
            strcpy(result, ""); // 結果不明
        }
    }
    
    // 空白を削除
    p = subject;
    while (*p) {
        if (*p == ' ') {
            memmove(p, p + 1, strlen(p));
        } else {
            p++;
        }
    }
    
    p = verb;
    while (*p) {
        if (*p == ' ') {
            memmove(p, p + 1, strlen(p));
        } else {
            p++;
        }
    }
    
    p = result;
    while (*p) {
        if (*p == ' ') {
            memmove(p, p + 1, strlen(p));
        } else {
            p++;
        }
    }
}

// 英語の文から主語、動詞、結果を抽出する
void extract_english_elements(const char* sentence, char* subject, char* verb, char* result) {
    // 作業用のバッファにコピー
    char buffer[MAX_STR_LEN];
    strncpy(buffer, sentence, MAX_STR_LEN - 1);
    buffer[MAX_STR_LEN - 1] = '\0';
    
    // 句読点を削除
    char* p = buffer;
    while (*p) {
        if (*p == '.' || *p == ',' || *p == '!' || *p == '?') {
            *p = ' ';
        }
        p++;
    }
    
    // 簡易的なトークン化（スペースで分割）
    char* tokens[100];
    int token_count = 0;
    
    char* token = strtok(buffer, " ");
    while (token != NULL && token_count < 100) {
        tokens[token_count++] = token;
        token = strtok(NULL, " ");
    }
    
    // 主語、動詞、結果を抽出
    if (token_count > 0) {
        strcpy(subject, tokens[0]); // 最初のトークンを主語とする
    } else {
        strcpy(subject, "");
    }
    
    if (token_count > 1) {
        strcpy(verb, tokens[1]); // 2番目のトークンを動詞とする
    } else {
        strcpy(verb, "");
    }
    
    if (token_count > 2) {
        strcpy(result, tokens[2]); // 3番目のトークンを結果とする
        
        // 残りのトークンも結果に含める
        for (int i = 3; i < token_count; i++) {
            strcat(result, " ");
            strcat(result, tokens[i]);
        }
    } else {
        strcpy(result, "");
    }
}

// 言語を判定する（簡易的な実装）
bool is_japanese(const char* text) {
    // 日本語の文字コード範囲をチェック（UTF-8を想定）
    unsigned char* p = (unsigned char*)text;
    while (*p) {
        if (*p >= 0xE0) { // 日本語のUTF-8は3バイト以上
            return true;
        }
        p++;
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
    if (argc < 2) {
        printf("使用方法: %s \"分析する文\"\n", argv[0]);
        return 1;
    }
    
    const char* sentence = argv[1];
    char subject[MAX_STR_LEN] = "";
    char verb[MAX_STR_LEN] = "";
    char result[MAX_STR_LEN] = "";
    
    // 文から主語、動詞、結果を抽出
    if (is_japanese(sentence)) {
        extract_japanese_elements(sentence, subject, verb, result);
    } else {
        extract_english_elements(sentence, subject, verb, result);
    }
    
    printf("入力文: %s\n", sentence);
    printf("抽出結果: [主語: %s][動詞: %s][結果: %s]\n", subject, verb, result);
    
    // DNA風に圧縮
    Dictionary dict;
    init_dictionary(&dict);
    
    char dna_code[MAX_STR_LEN] = "";
    compress_to_dna(&dict, subject, verb, result, dna_code);
    
    printf("\nDNA風圧縮: %s\n", dna_code);
    printf("\n");
    
    print_dictionary(&dict);
    printf("\n");
    
    // DNA風コードから再構築
    char reconstructed[MAX_STR_LEN] = "";
    decompress_from_dna(&dict, dna_code, reconstructed);
    
    printf("再構築された文: %s\n", reconstructed);
    
    return 0;
}