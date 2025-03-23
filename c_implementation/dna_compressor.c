#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mecab.h>

#define MAX_WORD_LEN 64
#define MAX_TOKENS 1000
#define MAX_LINE_LEN 1024
#define MAX_TEXT_LEN 102400
#define MAX_DNA_LEN 256
#define MAX_DICT_SIZE 1000

// 品詞の種類
typedef enum {
    ENTITY,     // 主語・実体
    CONCEPT,    // 概念・動詞
    RELATION,   // 関係・結果
    UNKNOWN     // 不明
} ConceptType;

// 形態素情報
typedef struct {
    char surface[MAX_WORD_LEN]; // 表層形
    char base[MAX_WORD_LEN];    // 基本形
    char pos[MAX_WORD_LEN];     // 品詞
    char pos_detail[MAX_WORD_LEN]; // 品詞細分類
    ConceptType type;           // 概念タイプ
} Token;

// 辞書エントリ
typedef struct {
    char code[8];               // DNAコード（例: E00, C01, R02）
    char word[MAX_WORD_LEN];    // 単語
    ConceptType type;           // 概念タイプ
} DictEntry;

// グローバル変数
char text_buffer[MAX_TEXT_LEN];
Token tokens[MAX_TOKENS];
int token_count = 0;
DictEntry dictionary[MAX_DICT_SIZE];
int dict_size = 0;

// 関数プロトタイプ
void tokenize_with_mecab(const char* text);
void classify_concepts();
void compress_to_dna(const char* output_file);
void decompress_from_dna(const char* dna_file, const char* output_file);
void add_to_dictionary(const char* word, ConceptType type);
const char* get_dna_code(const char* word, ConceptType type);
const char* get_word_from_code(const char* code);

// MeCabを使用してテキストをトークン化する
void tokenize_with_mecab(const char* text) {
    token_count = 0;
    
    // MeCabの初期化
    mecab_t* mecab = mecab_new2("");
    if (!mecab) {
        fprintf(stderr, "MeCabの初期化に失敗しました: %s\n", mecab_strerror(NULL));
        return;
    }
    
    // 形態素解析を実行
    const mecab_node_t* node = mecab_sparse_tonode(mecab, text);
    if (!node) {
        fprintf(stderr, "形態素解析に失敗しました: %s\n", mecab_strerror(mecab));
        mecab_destroy(mecab);
        return;
    }
    
    // 結果を処理
    for (; node; node = node->next) {
        // EOSノードをスキップ
        if (node->stat == MECAB_EOS_NODE) {
            continue;
        }
        
        // 表層形をコピー
        char surface[MAX_WORD_LEN];
        strncpy(surface, node->surface, node->length);
        surface[node->length] = '\0';
        
        // 空白をスキップ
        if (surface[0] == ' ' || surface[0] == '\t' || surface[0] == '\n' || surface[0] == '\r') {
            continue;
        }
        
        // 素性情報を解析
        char feature[MAX_LINE_LEN];
        strncpy(feature, node->feature, MAX_LINE_LEN - 1);
        feature[MAX_LINE_LEN - 1] = '\0';
        
        // 素性情報をカンマで分割
        char* pos = strtok(feature, ",");
        char* pos_detail1 = pos ? strtok(NULL, ",") : NULL;
        char* pos_detail2 = pos_detail1 ? strtok(NULL, ",") : NULL;
        char* pos_detail3 = pos_detail2 ? strtok(NULL, ",") : NULL;
        char* conjugation1 = pos_detail3 ? strtok(NULL, ",") : NULL;
        char* conjugation2 = conjugation1 ? strtok(NULL, ",") : NULL;
        char* base_form = conjugation2 ? strtok(NULL, ",") : NULL;
        
        // トークンに追加
        if (token_count < MAX_TOKENS) {
            strncpy(tokens[token_count].surface, surface, MAX_WORD_LEN - 1);
            tokens[token_count].surface[MAX_WORD_LEN - 1] = '\0';
            
            if (base_form && strcmp(base_form, "*") != 0) {
                strncpy(tokens[token_count].base, base_form, MAX_WORD_LEN - 1);
            } else {
                strncpy(tokens[token_count].base, surface, MAX_WORD_LEN - 1);
            }
            tokens[token_count].base[MAX_WORD_LEN - 1] = '\0';
            
            if (pos) {
                strncpy(tokens[token_count].pos, pos, MAX_WORD_LEN - 1);
            } else {
                strcpy(tokens[token_count].pos, "未知語");
            }
            tokens[token_count].pos[MAX_WORD_LEN - 1] = '\0';
            
            if (pos_detail1) {
                strncpy(tokens[token_count].pos_detail, pos_detail1, MAX_WORD_LEN - 1);
            } else {
                strcpy(tokens[token_count].pos_detail, "*");
            }
            tokens[token_count].pos_detail[MAX_WORD_LEN - 1] = '\0';
            
            // 初期値は不明
            tokens[token_count].type = UNKNOWN;
            
            token_count++;
        }
    }
    
    // MeCabを解放
    mecab_destroy(mecab);
}

// トークンを概念タイプに分類
void classify_concepts() {
    // 文の区切りを検出するための変数
    int sentence_start = 0;
    
    for (int i = 0; i < token_count; i++) {
        // 文の区切り（句点）を検出
        if (strcmp(tokens[i].pos, "記号") == 0 && 
            (strcmp(tokens[i].surface, "。") == 0 || 
             strcmp(tokens[i].surface, ".") == 0 || 
             strcmp(tokens[i].surface, "！") == 0 || 
             strcmp(tokens[i].surface, "!") == 0 || 
             strcmp(tokens[i].surface, "？") == 0 || 
             strcmp(tokens[i].surface, "?") == 0)) {
            
            // 文の中で主語、動詞、結果を特定
            int subject_idx = -1;
            int verb_idx = -1;
            int result_idx = -1;
            
            // 主語を探す（「は」「が」の前の名詞）
            for (int j = sentence_start; j < i; j++) {
                if (strcmp(tokens[j].pos, "名詞") == 0 && j + 1 < i) {
                    if ((strcmp(tokens[j+1].surface, "は") == 0 || 
                         strcmp(tokens[j+1].surface, "が") == 0) && 
                        subject_idx == -1) {
                        subject_idx = j;
                        tokens[j].type = ENTITY;
                        add_to_dictionary(tokens[j].base, ENTITY);
                    }
                }
            }
            
            // 動詞を探す
            for (int j = sentence_start; j < i; j++) {
                if (strcmp(tokens[j].pos, "動詞") == 0 && verb_idx == -1) {
                    verb_idx = j;
                    tokens[j].type = CONCEPT;
                    add_to_dictionary(tokens[j].base, CONCEPT);
                }
            }
            
            // 結果を探す（形容詞、または「を」の後の名詞）
            for (int j = sentence_start; j < i; j++) {
                if (strcmp(tokens[j].pos, "形容詞") == 0 && result_idx == -1) {
                    result_idx = j;
                    tokens[j].type = RELATION;
                    add_to_dictionary(tokens[j].base, RELATION);
                } else if (strcmp(tokens[j].surface, "を") == 0 && j + 1 < i && 
                           strcmp(tokens[j+1].pos, "名詞") == 0 && result_idx == -1) {
                    result_idx = j + 1;
                    tokens[j+1].type = RELATION;
                    add_to_dictionary(tokens[j+1].base, RELATION);
                }
            }
            
            // 「好き」は特別扱い（形容動詞語幹）
            for (int j = sentence_start; j < i; j++) {
                if (strcmp(tokens[j].base, "好き") == 0 && result_idx == -1) {
                    result_idx = j;
                    tokens[j].type = RELATION;
                    add_to_dictionary(tokens[j].base, RELATION);
                }
            }
            
            // 次の文の開始位置を更新
            sentence_start = i + 1;
        }
    }
}

// 辞書に単語を追加
void add_to_dictionary(const char* word, ConceptType type) {
    // 既に辞書に存在するかチェック
    for (int i = 0; i < dict_size; i++) {
        if (dictionary[i].type == type && strcmp(dictionary[i].word, word) == 0) {
            return; // 既に存在する場合は何もしない
        }
    }
    
    // 辞書に追加
    if (dict_size < MAX_DICT_SIZE) {
        // タイプに応じたプレフィックスを設定
        char prefix;
        switch (type) {
            case ENTITY:
                prefix = 'E';
                break;
            case CONCEPT:
                prefix = 'C';
                break;
            case RELATION:
                prefix = 'R';
                break;
            default:
                prefix = 'U'; // Unknown
                break;
        }
        
        // 同じタイプの単語数をカウント
        int count = 0;
        for (int i = 0; i < dict_size; i++) {
            if (dictionary[i].type == type) {
                count++;
            }
        }
        
        // コードを生成（例: E00, C01, R02）
        snprintf(dictionary[dict_size].code, sizeof(dictionary[dict_size].code), 
                 "%c%02d", prefix, count);
        
        // 単語とタイプを設定
        strncpy(dictionary[dict_size].word, word, MAX_WORD_LEN - 1);
        dictionary[dict_size].word[MAX_WORD_LEN - 1] = '\0';
        dictionary[dict_size].type = type;
        
        dict_size++;
    }
}

// 単語からDNAコードを取得
const char* get_dna_code(const char* word, ConceptType type) {
    for (int i = 0; i < dict_size; i++) {
        if (dictionary[i].type == type && strcmp(dictionary[i].word, word) == 0) {
            return dictionary[i].code;
        }
    }
    return NULL; // 見つからない場合
}

// DNAコードから単語を取得
const char* get_word_from_code(const char* code) {
    for (int i = 0; i < dict_size; i++) {
        if (strcmp(dictionary[i].code, code) == 0) {
            return dictionary[i].word;
        }
    }
    return NULL; // 見つからない場合
}

// テキストをDNA形式に圧縮
void compress_to_dna(const char* output_file) {
    FILE* file = fopen(output_file, "w");
    if (!file) {
        fprintf(stderr, "出力ファイルを開けませんでした: %s\n", output_file);
        return;
    }
    
    // 辞書を出力
    fprintf(file, "# DNA Dictionary\n");
    for (int i = 0; i < dict_size; i++) {
        fprintf(file, "%s:%s\n", dictionary[i].code, dictionary[i].word);
    }
    fprintf(file, "# End Dictionary\n\n");
    
    // 文の区切りを検出するための変数
    int sentence_start = 0;
    
    // 各文をDNA形式に変換
    for (int i = 0; i < token_count; i++) {
        // 文の区切り（句点）を検出
        if (strcmp(tokens[i].pos, "記号") == 0 && 
            (strcmp(tokens[i].surface, "。") == 0 || 
             strcmp(tokens[i].surface, ".") == 0 || 
             strcmp(tokens[i].surface, "！") == 0 || 
             strcmp(tokens[i].surface, "!") == 0 || 
             strcmp(tokens[i].surface, "？") == 0 || 
             strcmp(tokens[i].surface, "?") == 0)) {
            
            // 文の中で主語、動詞、結果を特定
            const char* subject_code = NULL;
            const char* verb_code = NULL;
            const char* result_code = NULL;
            
            // 各トークンのDNAコードを取得
            for (int j = sentence_start; j < i; j++) {
                if (tokens[j].type == ENTITY && !subject_code) {
                    subject_code = get_dna_code(tokens[j].base, ENTITY);
                } else if (tokens[j].type == CONCEPT && !verb_code) {
                    verb_code = get_dna_code(tokens[j].base, CONCEPT);
                } else if (tokens[j].type == RELATION && !result_code) {
                    result_code = get_dna_code(tokens[j].base, RELATION);
                }
            }
            
            // DNA形式で出力
            fprintf(file, "DNA: ");
            if (subject_code) fprintf(file, "%s", subject_code);
            if (verb_code) fprintf(file, "%s", verb_code);
            if (result_code) fprintf(file, "%s", result_code);
            fprintf(file, "\n");
            
            // 元の文も出力
            fprintf(file, "ORG: ");
            for (int j = sentence_start; j < i + 1; j++) {
                fprintf(file, "%s", tokens[j].surface);
            }
            fprintf(file, "\n\n");
            
            // 次の文の開始位置を更新
            sentence_start = i + 1;
        }
    }
    
    fclose(file);
    printf("DNAファイルを生成しました: %s\n", output_file);
}

// DNA形式から文を再構築
void decompress_from_dna(const char* dna_file, const char* output_file) {
    FILE* in_file = fopen(dna_file, "r");
    if (!in_file) {
        fprintf(stderr, "DNAファイルを開けませんでした: %s\n", dna_file);
        return;
    }
    
    FILE* out_file = fopen(output_file, "w");
    if (!out_file) {
        fprintf(stderr, "出力ファイルを開けませんでした: %s\n", output_file);
        fclose(in_file);
        return;
    }
    
    // 辞書をクリア
    dict_size = 0;
    
    // 辞書を読み込む
    char line[MAX_LINE_LEN];
    bool reading_dict = false;
    
    while (fgets(line, sizeof(line), in_file)) {
        // 改行を削除
        size_t len = strlen(line);
        if (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
        }
        
        // 辞書の開始と終了をチェック
        if (strcmp(line, "# DNA Dictionary") == 0) {
            reading_dict = true;
            continue;
        } else if (strcmp(line, "# End Dictionary") == 0) {
            reading_dict = false;
            continue;
        }
        
        // 辞書エントリを解析
        if (reading_dict) {
            char* code = strtok(line, ":");
            char* word = strtok(NULL, ":");
            
            if (code && word && dict_size < MAX_DICT_SIZE) {
                strncpy(dictionary[dict_size].code, code, sizeof(dictionary[dict_size].code) - 1);
                dictionary[dict_size].code[sizeof(dictionary[dict_size].code) - 1] = '\0';
                
                strncpy(dictionary[dict_size].word, word, MAX_WORD_LEN - 1);
                dictionary[dict_size].word[MAX_WORD_LEN - 1] = '\0';
                
                // タイプを設定
                switch (code[0]) {
                    case 'E':
                        dictionary[dict_size].type = ENTITY;
                        break;
                    case 'C':
                        dictionary[dict_size].type = CONCEPT;
                        break;
                    case 'R':
                        dictionary[dict_size].type = RELATION;
                        break;
                    default:
                        dictionary[dict_size].type = UNKNOWN;
                        break;
                }
                
                dict_size++;
            }
        }
        // DNAコードを解析
        else if (strncmp(line, "DNA: ", 5) == 0) {
            char dna[MAX_DNA_LEN];
            strncpy(dna, line + 5, sizeof(dna) - 1);
            dna[sizeof(dna) - 1] = '\0';
            
            // DNAコードを3文字ずつ解析
            char subject[MAX_WORD_LEN] = "";
            char verb[MAX_WORD_LEN] = "";
            char relation[MAX_WORD_LEN] = "";
            
            int pos = 0;
            while (pos < strlen(dna)) {
                char code[4] = {0};
                strncpy(code, dna + pos, 3);
                code[3] = '\0';
                
                const char* word = get_word_from_code(code);
                if (word) {
                    if (code[0] == 'E' && subject[0] == '\0') {
                        strncpy(subject, word, sizeof(subject) - 1);
                        subject[sizeof(subject) - 1] = '\0';
                    } else if (code[0] == 'C' && verb[0] == '\0') {
                        strncpy(verb, word, sizeof(verb) - 1);
                        verb[sizeof(verb) - 1] = '\0';
                    } else if (code[0] == 'R' && relation[0] == '\0') {
                        strncpy(relation, word, sizeof(relation) - 1);
                        relation[sizeof(relation) - 1] = '\0';
                    }
                }
                
                pos += 3;
            }
            
            // 文を再構築
            if (subject[0] != '\0') {
                if (verb[0] != '\0') {
                    if (relation[0] != '\0') {
                        // 主語・動詞・結果がある場合
                        // 動詞の活用形を生成
                        if (strcmp(verb, "食べる") == 0) {
                            fprintf(out_file, "%sは%sを食べます。\n", subject, relation);
                        } else if (strcmp(verb, "飛ぶ") == 0) {
                            fprintf(out_file, "%sは%sを飛びます。\n", subject, relation);
                        } else if (strcmp(verb, "話す") == 0) {
                            fprintf(out_file, "%sは%sを話します。\n", subject, relation);
                        } else {
                            fprintf(out_file, "%sは%sを%sします。\n", subject, relation, verb);
                        }
                    } else {
                        // 主語・動詞のみの場合
                        if (strcmp(verb, "食べる") == 0) {
                            fprintf(out_file, "%sが食べます。\n", subject);
                        } else if (strcmp(verb, "飛ぶ") == 0) {
                            fprintf(out_file, "%sが飛びます。\n", subject);
                        } else if (strcmp(verb, "話す") == 0) {
                            fprintf(out_file, "%sが話します。\n", subject);
                        } else {
                            fprintf(out_file, "%sが%sします。\n", subject, verb);
                        }
                    }
                } else if (relation[0] != '\0') {
                    // 主語・結果のみの場合
                    fprintf(out_file, "%sは%sです。\n", subject, relation);
                } else {
                    // 主語のみの場合
                    fprintf(out_file, "%sがあります。\n", subject);
                }
            } else if (verb[0] != '\0') {
                // 動詞のみの場合
                if (strcmp(verb, "食べる") == 0) {
                    fprintf(out_file, "食べます。\n");
                } else if (strcmp(verb, "飛ぶ") == 0) {
                    fprintf(out_file, "飛びます。\n");
                } else if (strcmp(verb, "話す") == 0) {
                    fprintf(out_file, "話します。\n");
                } else {
                    fprintf(out_file, "%sします。\n", verb);
                }
            }
        }
    }
    
    fclose(in_file);
    fclose(out_file);
    printf("文を再構築しました: %s\n", output_file);
}

int main(int argc, char* argv[]) {
    // コマンドライン引数のチェック
    if (argc < 3) {
        printf("使用法:\n");
        printf("  圧縮: %s compress <入力テキストファイル> <出力DNAファイル>\n", argv[0]);
        printf("  展開: %s decompress <入力DNAファイル> <出力テキストファイル>\n", argv[0]);
        return 1;
    }
    
    // モードを判定
    if (strcmp(argv[1], "compress") == 0) {
        if (argc < 4) {
            printf("圧縮モードでは入力ファイルと出力ファイルが必要です。\n");
            return 1;
        }
        
        // ファイルを開く
        FILE* file = fopen(argv[2], "r");
        if (!file) {
            printf("ファイル %s を開けませんでした。\n", argv[2]);
            return 1;
        }
        
        // ファイルを読み込む
        size_t read_size = fread(text_buffer, 1, MAX_TEXT_LEN - 1, file);
        text_buffer[read_size] = '\0';
        fclose(file);
        
        // テキストをトークン化
        tokenize_with_mecab(text_buffer);
        
        // 概念を分類
        classify_concepts();
        
        // DNA形式に圧縮
        compress_to_dna(argv[3]);
    } else if (strcmp(argv[1], "decompress") == 0) {
        if (argc < 4) {
            printf("展開モードでは入力DNAファイルと出力テキストファイルが必要です。\n");
            return 1;
        }
        
        // DNA形式から文を再構築
        decompress_from_dna(argv[2], argv[3]);
    } else {
        printf("不明なモード: %s\n", argv[1]);
        printf("使用法:\n");
        printf("  圧縮: %s compress <入力テキストファイル> <出力DNAファイル>\n", argv[0]);
        printf("  展開: %s decompress <入力DNAファイル> <出力テキストファイル>\n", argv[0]);
        return 1;
    }
    
    return 0;
}