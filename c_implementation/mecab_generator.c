#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <locale.h>
#include <mecab.h>

#define MAX_WORD_LEN 64
#define MAX_TOKENS 1000
#define MAX_LINE_LEN 1024
#define MAX_TEXT_LEN 102400

// 品詞の種類
typedef enum {
    NOUN,       // 名詞
    VERB,       // 動詞
    ADJECTIVE,  // 形容詞
    ADVERB,     // 副詞
    PARTICLE,   // 助詞
    AUXILIARY,  // 助動詞
    CONJUNCTION,// 接続詞
    PREFIX,     // 接頭辞
    SUFFIX,     // 接尾辞
    PRONOUN,    // 代名詞
    NUMBER,     // 数詞
    INTERJECTION,// 感動詞
    SYMBOL,     // 記号
    UNKNOWN     // 不明
} PartOfSpeech;

// 活用形
typedef enum {
    NO_CONJUGATION, // 活用なし
    BASE_FORM,      // 基本形
    MASU_FORM,      // ます形
    TE_FORM,        // て形
    TA_FORM,        // た形
    NEGATIVE_FORM,  // 否定形
    PASSIVE_FORM,   // 受身形
    CAUSATIVE_FORM, // 使役形
    IMPERATIVE_FORM,// 命令形
    POTENTIAL_FORM, // 可能形
    CONDITIONAL_FORM// 条件形
} ConjugationForm;

// 形態素情報
typedef struct {
    char surface[MAX_WORD_LEN]; // 表層形
    char base[MAX_WORD_LEN];    // 基本形
    char pos[MAX_WORD_LEN];     // 品詞
    char pos_detail[MAX_WORD_LEN]; // 品詞細分類
    char conjugation[MAX_WORD_LEN]; // 活用形
    double confidence;          // 信頼度
} Token;

// グローバル変数
char text_buffer[MAX_TEXT_LEN];
Token tokens[MAX_TOKENS];
int token_count = 0;

// 関数プロトタイプ
void tokenize_with_mecab(const char* text);
void generate_simple_sentences();
const char* conjugate_verb(const char* verb, ConjugationForm form);

// 動詞の活用形を取得
const char* conjugate_verb(const char* verb, ConjugationForm form) {
    static char conjugated[MAX_WORD_LEN];
    
    // 基本形をコピー
    strncpy(conjugated, verb, MAX_WORD_LEN - 1);
    conjugated[MAX_WORD_LEN - 1] = '\0';
    
    // 特殊な動詞の処理
    if (strcmp(verb, "する") == 0) {
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "します");
                break;
            case TE_FORM:
                strcpy(conjugated, "して");
                break;
            case TA_FORM:
                strcpy(conjugated, "した");
                break;
            default:
                break;
        }
    } else if (strcmp(verb, "来る") == 0 || strcmp(verb, "くる") == 0) {
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "きます");
                break;
            case TE_FORM:
                strcpy(conjugated, "きて");
                break;
            case TA_FORM:
                strcpy(conjugated, "きた");
                break;
            default:
                break;
        }
    } else if (strcmp(verb, "食べる") == 0) {
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "食べます");
                break;
            case TE_FORM:
                strcpy(conjugated, "食べて");
                break;
            case TA_FORM:
                strcpy(conjugated, "食べた");
                break;
            default:
                break;
        }
    } else if (strcmp(verb, "飛ぶ") == 0) {
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "飛びます");
                break;
            case TE_FORM:
                strcpy(conjugated, "飛んで");
                break;
            case TA_FORM:
                strcpy(conjugated, "飛んだ");
                break;
            default:
                break;
        }
    } else if (strcmp(verb, "話す") == 0) {
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "話します");
                break;
            case TE_FORM:
                strcpy(conjugated, "話して");
                break;
            case TA_FORM:
                strcpy(conjugated, "話した");
                break;
            default:
                break;
        }
    } else if (strcmp(verb, "ある") == 0) {
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "あります");
                break;
            case TE_FORM:
                strcpy(conjugated, "あって");
                break;
            case TA_FORM:
                strcpy(conjugated, "あった");
                break;
            default:
                break;
        }
    } else if (strcmp(verb, "いる") == 0) {
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "います");
                break;
            case TE_FORM:
                strcpy(conjugated, "いて");
                break;
            case TA_FORM:
                strcpy(conjugated, "いた");
                break;
            default:
                break;
        }
    } else if (strcmp(verb, "なる") == 0) {
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "なります");
                break;
            case TE_FORM:
                strcpy(conjugated, "なって");
                break;
            case TA_FORM:
                strcpy(conjugated, "なった");
                break;
            default:
                break;
        }
    } else if (strcmp(verb, "行く") == 0) {
        switch (form) {
            case MASU_FORM:
                strcpy(conjugated, "行きます");
                break;
            case TE_FORM:
                strcpy(conjugated, "行って");
                break;
            case TA_FORM:
                strcpy(conjugated, "行った");
                break;
            default:
                break;
        }
    } else {
        // 一般的な動詞の活用（簡易版）
        size_t len = strlen(conjugated);
        if (len > 1) {
            char last_char = conjugated[len-1];
            
            // 一段動詞（「る」で終わる）
            if (last_char == 0x82 && len > 2 && conjugated[len-2] == 0xe3) {
                // UTF-8の「る」を削除
                conjugated[len-2] = '\0';
                
                switch (form) {
                    case MASU_FORM:
                        strcat(conjugated, "ます");
                        break;
                    case TE_FORM:
                        strcat(conjugated, "て");
                        break;
                    case TA_FORM:
                        strcat(conjugated, "た");
                        break;
                    default:
                        strcat(conjugated, "る"); // 元に戻す
                        break;
                }
            }
            // 五段動詞（「う」で終わる）
            else if (last_char == 0x86 && len > 2 && conjugated[len-2] == 0x81 && conjugated[len-3] == 0xe3) {
                // UTF-8の「う」を削除
                conjugated[len-3] = '\0';
                
                switch (form) {
                    case MASU_FORM:
                        strcat(conjugated, "います");
                        break;
                    case TE_FORM:
                        strcat(conjugated, "って");
                        break;
                    case TA_FORM:
                        strcat(conjugated, "った");
                        break;
                    default:
                        strcat(conjugated, "う"); // 元に戻す
                        break;
                }
            }
            // その他の場合
            else {
                switch (form) {
                    case MASU_FORM:
                        strcat(conjugated, "ます");
                        break;
                    case TE_FORM:
                        strcat(conjugated, "て");
                        break;
                    case TA_FORM:
                        strcat(conjugated, "た");
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    return conjugated;
}

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
            
            if (conjugation1) {
                strncpy(tokens[token_count].conjugation, conjugation1, MAX_WORD_LEN - 1);
            } else {
                strcpy(tokens[token_count].conjugation, "*");
            }
            tokens[token_count].conjugation[MAX_WORD_LEN - 1] = '\0';
            
            tokens[token_count].confidence = 1.0;
            
            token_count++;
        }
    }
    
    // MeCabを解放
    mecab_destroy(mecab);
    
    // トークンを表示
    printf("形態素解析結果:\n");
    printf("表層形\t品詞\t品詞細分類\t基本形\t活用形\n");
    printf("--------------------------------------------\n");
    
    for (int i = 0; i < token_count; i++) {
        printf("%s\t%s\t%s\t%s\t%s\n", 
               tokens[i].surface, 
               tokens[i].pos, 
               tokens[i].pos_detail, 
               tokens[i].base, 
               tokens[i].conjugation);
    }
}

// 簡易的な文生成
void generate_simple_sentences() {
    // 名詞、動詞、形容詞を収集
    char nouns[MAX_TOKENS][MAX_WORD_LEN];
    char verbs[MAX_TOKENS][MAX_WORD_LEN];
    char adjectives[MAX_TOKENS][MAX_WORD_LEN];
    int noun_count = 0;
    int verb_count = 0;
    int adj_count = 0;
    
    for (int i = 0; i < token_count; i++) {
        // 「好き」は形容詞として扱う
        if (strcmp(tokens[i].base, "好き") == 0 && adj_count < MAX_TOKENS) {
            strncpy(adjectives[adj_count], tokens[i].base, MAX_WORD_LEN - 1);
            adjectives[adj_count][MAX_WORD_LEN - 1] = '\0';
            adj_count++;
        }
        // 通常の名詞
        else if (strcmp(tokens[i].pos, "名詞") == 0 && noun_count < MAX_TOKENS) {
            // 長すぎる名詞や短すぎる名詞は除外
            if (strlen(tokens[i].base) > 1 && strlen(tokens[i].base) < 15) {
                strncpy(nouns[noun_count], tokens[i].base, MAX_WORD_LEN - 1);
                nouns[noun_count][MAX_WORD_LEN - 1] = '\0';
                noun_count++;
            }
        } 
        // 動詞
        else if (strcmp(tokens[i].pos, "動詞") == 0 && verb_count < MAX_TOKENS) {
            strncpy(verbs[verb_count], tokens[i].base, MAX_WORD_LEN - 1);
            verbs[verb_count][MAX_WORD_LEN - 1] = '\0';
            verb_count++;
        } 
        // 形容詞
        else if (strcmp(tokens[i].pos, "形容詞") == 0 && adj_count < MAX_TOKENS) {
            strncpy(adjectives[adj_count], tokens[i].base, MAX_WORD_LEN - 1);
            adjectives[adj_count][MAX_WORD_LEN - 1] = '\0';
            adj_count++;
        }
    }
    
    // 辞書に登録されている基本的な動詞を追加
    if (verb_count < 5) {
        strcpy(verbs[verb_count++], "する");
        strcpy(verbs[verb_count++], "ある");
        strcpy(verbs[verb_count++], "いる");
        strcpy(verbs[verb_count++], "なる");
        strcpy(verbs[verb_count++], "行く");
    }
    
    // 辞書に登録されている基本的な形容詞を追加
    if (adj_count < 5) {
        strcpy(adjectives[adj_count++], "良い");
        strcpy(adjectives[adj_count++], "悪い");
        strcpy(adjectives[adj_count++], "高い");
        strcpy(adjectives[adj_count++], "低い");
        strcpy(adjectives[adj_count++], "大きい");
    }
    
    // 重複を除去
    for (int i = 0; i < noun_count; i++) {
        for (int j = i + 1; j < noun_count; j++) {
            if (strcmp(nouns[i], nouns[j]) == 0) {
                // j番目の要素を削除
                for (int k = j; k < noun_count - 1; k++) {
                    strcpy(nouns[k], nouns[k + 1]);
                }
                noun_count--;
                j--; // 削除した位置を再チェック
            }
        }
    }
    
    for (int i = 0; i < verb_count; i++) {
        for (int j = i + 1; j < verb_count; j++) {
            if (strcmp(verbs[i], verbs[j]) == 0) {
                // j番目の要素を削除
                for (int k = j; k < verb_count - 1; k++) {
                    strcpy(verbs[k], verbs[k + 1]);
                }
                verb_count--;
                j--; // 削除した位置を再チェック
            }
        }
    }
    
    for (int i = 0; i < adj_count; i++) {
        for (int j = i + 1; j < adj_count; j++) {
            if (strcmp(adjectives[i], adjectives[j]) == 0) {
                // j番目の要素を削除
                for (int k = j; k < adj_count - 1; k++) {
                    strcpy(adjectives[k], adjectives[k + 1]);
                }
                adj_count--;
                j--; // 削除した位置を再チェック
            }
        }
    }
    
    printf("\n収集した単語:\n");
    printf("名詞: %d個\n", noun_count);
    for (int i = 0; i < noun_count; i++) {
        printf("  %s\n", nouns[i]);
    }
    
    printf("動詞: %d個\n", verb_count);
    for (int i = 0; i < verb_count; i++) {
        printf("  %s\n", verbs[i]);
    }
    
    printf("形容詞: %d個\n", adj_count);
    for (int i = 0; i < adj_count; i++) {
        printf("  %s\n", adjectives[i]);
    }
    
    // 文生成
    printf("\n生成された文:\n");
    
    // パターン1: [名詞]は[形容詞]です。
    if (noun_count > 0 && adj_count > 0) {
        for (int i = 0; i < 3 && i < noun_count; i++) {
            int noun_idx = rand() % noun_count;
            int adj_idx = rand() % adj_count;
            printf("%sは%sです。\n", nouns[noun_idx], adjectives[adj_idx]);
        }
    }
    
    // パターン2: [名詞]が[動詞]ます。
    if (noun_count > 0 && verb_count > 0) {
        for (int i = 0; i < 3 && i < noun_count; i++) {
            int noun_idx = rand() % noun_count;
            int verb_idx = rand() % verb_count;
            const char* verb_form = conjugate_verb(verbs[verb_idx], MASU_FORM);
            
            // 「ますます」のチェック
            char* dup_check = strstr(verb_form, "ますます");
            if (dup_check) {
                // 重複を修正
                *dup_check = '\0';
                printf("%sが%sます。\n", nouns[noun_idx], verb_form);
            } else {
                printf("%sが%s。\n", nouns[noun_idx], verb_form);
            }
        }
    }
    
    // パターン3: [名詞]は[名詞]を[動詞]ます。
    if (noun_count > 1 && verb_count > 0) {
        for (int i = 0; i < 3; i++) {
            int noun1_idx = rand() % noun_count;
            int noun2_idx = rand() % noun_count;
            int verb_idx = rand() % verb_count;
            
            // 同じ名詞が選ばれた場合は別の名詞を選ぶ
            while (noun2_idx == noun1_idx && noun_count > 1) {
                noun2_idx = rand() % noun_count;
            }
            
            const char* verb_form = conjugate_verb(verbs[verb_idx], MASU_FORM);
            
            // 「ますます」のチェック
            char* dup_check = strstr(verb_form, "ますます");
            if (dup_check) {
                // 重複を修正
                *dup_check = '\0';
                printf("%sは%sを%sます。\n", 
                       nouns[noun1_idx], 
                       nouns[noun2_idx], 
                       verb_form);
            } else {
                printf("%sは%sを%s。\n", 
                       nouns[noun1_idx], 
                       nouns[noun2_idx], 
                       verb_form);
            }
        }
    }
    
    // パターン4: [名詞]が[形容詞]と[動詞]ました。
    if (noun_count > 0 && adj_count > 0 && verb_count > 0) {
        for (int i = 0; i < 3; i++) {
            int noun_idx = rand() % noun_count;
            int adj_idx = rand() % adj_count;
            int verb_idx = rand() % verb_count;
            
            const char* verb_form = conjugate_verb(verbs[verb_idx], TA_FORM);
            
            // 「たました」のチェック
            char* dup_check = strstr(verb_form, "たました");
            if (dup_check) {
                // 重複を修正
                *dup_check = '\0';
                printf("%sが%sと%sました。\n", 
                       nouns[noun_idx], 
                       adjectives[adj_idx], 
                       verb_form);
            } else {
                printf("%sが%sと%s。\n", 
                       nouns[noun_idx], 
                       adjectives[adj_idx], 
                       verb_form);
            }
        }
    }
    
    // パターン5: [名詞]は[名詞]に[動詞]て、[形容詞]です。
    if (noun_count > 1 && verb_count > 0 && adj_count > 0) {
        for (int i = 0; i < 3; i++) {
            int noun1_idx = rand() % noun_count;
            int noun2_idx = rand() % noun_count;
            int verb_idx = rand() % verb_count;
            int adj_idx = rand() % adj_count;
            
            // 同じ名詞が選ばれた場合は別の名詞を選ぶ
            while (noun2_idx == noun1_idx && noun_count > 1) {
                noun2_idx = rand() % noun_count;
            }
            
            const char* verb_form = conjugate_verb(verbs[verb_idx], TE_FORM);
            
            // 「てて」のチェック
            char* dup_check = strstr(verb_form, "てて");
            if (dup_check) {
                // 重複を修正
                *dup_check = '\0';
                printf("%sは%sに%sて、%sです。\n", 
                       nouns[noun1_idx], 
                       nouns[noun2_idx], 
                       verb_form, 
                       adjectives[adj_idx]);
            } else {
                printf("%sは%sに%s、%sです。\n", 
                       nouns[noun1_idx], 
                       nouns[noun2_idx], 
                       verb_form, 
                       adjectives[adj_idx]);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // ロケールを設定（日本語対応）
    setlocale(LC_ALL, "");
    
    // 乱数の初期化
    srand((unsigned int)time(NULL));
    
    // コマンドライン引数のチェック
    if (argc < 2) {
        printf("使用法: %s <テキストファイル>\n", argv[0]);
        return 1;
    }
    
    // ファイルを開く
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("ファイル %s を開けませんでした。\n", argv[1]);
        return 1;
    }
    
    // ファイルを読み込む
    size_t read_size = fread(text_buffer, 1, MAX_TEXT_LEN - 1, file);
    text_buffer[read_size] = '\0';
    fclose(file);
    
    // テキストをトークン化
    tokenize_with_mecab(text_buffer);
    
    // 文を生成
    generate_simple_sentences();
    
    return 0;
}