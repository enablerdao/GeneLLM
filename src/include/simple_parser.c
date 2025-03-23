#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mecab.h>

// 簡易構文解析器の構造体
typedef struct {
    char subject[256];    // 主語
    char verb[256];       // 動詞
    char result[256];     // 結果
    int has_subject;      // 主語があるかどうか
    int has_verb;         // 動詞があるかどうか
    int has_result;       // 結果があるかどうか
} SimpleParser;

// 簡易構文解析器の初期化
void simple_parser_init(SimpleParser* parser) {
    memset(parser->subject, 0, sizeof(parser->subject));
    memset(parser->verb, 0, sizeof(parser->verb));
    memset(parser->result, 0, sizeof(parser->result));
    parser->has_subject = 0;
    parser->has_verb = 0;
    parser->has_result = 0;
}

// 簡易構文解析を実行
int simple_parser_parse(SimpleParser* parser, const char* text) {
    // MeCabの初期化
    mecab_t* mecab = mecab_new2("");
    if (!mecab) {
        fprintf(stderr, "MeCabの初期化に失敗しました\n");
        return 0;
    }

    // 形態素解析を実行
    const mecab_node_t* node = mecab_sparse_tonode(mecab, text);
    if (!node) {
        fprintf(stderr, "形態素解析に失敗しました\n");
        mecab_destroy(mecab);
        return 0;
    }

    // 解析結果を処理
    int in_subject = 1;  // 最初は主語を探す
    int in_verb = 0;     // 動詞フラグ
    int in_result = 0;   // 結果フラグ

    // 主語、動詞、結果の一時バッファ
    char subject_buf[256] = {0};
    char verb_buf[256] = {0};
    char result_buf[256] = {0};

    // ノードを順に処理
    for (; node; node = node->next) {
        // BOS/EOSノードはスキップ
        if (node->stat == MECAB_BOS_NODE || node->stat == MECAB_EOS_NODE) {
            continue;
        }

        // 形態素の表層形を取得
        char surface[256];
        strncpy(surface, node->surface, node->length);
        surface[node->length] = '\0';

        // 品詞情報を取得
        char feature[256];
        strncpy(feature, node->feature, sizeof(feature) - 1);
        feature[sizeof(feature) - 1] = '\0';
        char* pos = strtok(feature, ",");  // 品詞

        // 主語を探す
        if (in_subject) {
            // 名詞または代名詞を主語として扱う
            if (strcmp(pos, "名詞") == 0 || strcmp(pos, "代名詞") == 0) {
                if (strlen(subject_buf) > 0) {
                    strcat(subject_buf, surface);
                } else {
                    strcpy(subject_buf, surface);
                }
                parser->has_subject = 1;
            }
            // 助詞「は」「が」「も」が来たら主語の終わり
            else if (strcmp(pos, "助詞") == 0 && 
                    (strcmp(surface, "は") == 0 || 
                     strcmp(surface, "が") == 0 || 
                     strcmp(surface, "も") == 0)) {
                in_subject = 0;
                in_verb = 1;  // 次は動詞を探す
            }
        }
        // 動詞を探す
        else if (in_verb) {
            // 動詞を見つけたら記録
            if (strcmp(pos, "動詞") == 0) {
                if (strlen(verb_buf) > 0) {
                    strcat(verb_buf, surface);
                } else {
                    strcpy(verb_buf, surface);
                }
                parser->has_verb = 1;
                in_verb = 0;
                in_result = 1;  // 次は結果を探す
            }
        }
        // 結果を探す
        else if (in_result) {
            // 形容詞、副詞、名詞などを結果として扱う
            if (strcmp(pos, "形容詞") == 0 || 
                strcmp(pos, "副詞") == 0 || 
                strcmp(pos, "名詞") == 0 ||
                strcmp(pos, "形容動詞") == 0) {
                if (strlen(result_buf) > 0) {
                    strcat(result_buf, surface);
                } else {
                    strcpy(result_buf, surface);
                }
                parser->has_result = 1;
            }
        }
    }

    // 基本形に変換（簡易的な処理）
    if (parser->has_subject) {
        strcpy(parser->subject, subject_buf);
    }
    
    if (parser->has_verb) {
        // 動詞の基本形に変換（簡易的な処理）
        char* verb_base = verb_buf;
        // 「ます」「ました」などの語尾を取り除く簡易処理
        if (strstr(verb_buf, "ます")) {
            char* suffix = strstr(verb_buf, "ます");
            *suffix = '\0';
            strcat(verb_base, "る");
        } else if (strstr(verb_buf, "た")) {
            char* suffix = strstr(verb_buf, "た");
            *suffix = '\0';
            strcat(verb_base, "る");
        }
        strcpy(parser->verb, verb_base);
    }
    
    if (parser->has_result) {
        strcpy(parser->result, result_buf);
    }

    // MeCabのリソースを解放
    mecab_destroy(mecab);
    return 1;
}

// 解析結果を表示
void simple_parser_print(SimpleParser* parser) {
    printf("解析結果:\n");
    if (parser->has_subject) {
        printf("  主語: %s\n", parser->subject);
    } else {
        printf("  主語: なし\n");
    }
    
    if (parser->has_verb) {
        printf("  動詞: %s\n", parser->verb);
    } else {
        printf("  動詞: なし\n");
    }
    
    if (parser->has_result) {
        printf("  結果: %s\n", parser->result);
    } else {
        printf("  結果: なし\n");
    }
}

// 解析結果をDNA風に圧縮
void simple_parser_to_dna(SimpleParser* parser, char* dna_output) {
    strcpy(dna_output, "");
    
    if (parser->has_subject) {
        strcat(dna_output, "E00");  // 主語のコード
    }
    
    if (parser->has_verb) {
        strcat(dna_output, "C01");  // 動詞のコード
    }
    
    if (parser->has_result) {
        strcat(dna_output, "R02");  // 結果のコード
    }
}

// DNA風の圧縮から文を再構築
void simple_parser_from_dna(SimpleParser* parser, const char* dna, char* output) {
    strcpy(output, "");
    
    // DNAコードを解析
    if (strstr(dna, "E00") && parser->has_subject) {
        strcat(output, parser->subject);
        strcat(output, "は");
    }
    
    if (strstr(dna, "C01") && parser->has_verb) {
        strcat(output, parser->verb);
        if (parser->has_result) {
            strcat(output, "て");
        } else {
            strcat(output, "ました");
        }
    }
    
    if (strstr(dna, "R02") && parser->has_result) {
        strcat(output, parser->result);
        strcat(output, "になりました");
    }
}

// テスト用のメイン関数
#ifdef TEST_SIMPLE_PARSER
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("使用法: %s \"解析する文\"\n", argv[0]);
        return 1;
    }
    
    SimpleParser parser;
    simple_parser_init(&parser);
    
    if (simple_parser_parse(&parser, argv[1])) {
        simple_parser_print(&parser);
        
        char dna[256];
        simple_parser_to_dna(&parser, dna);
        printf("DNA風圧縮: %s\n", dna);
        
        char reconstructed[512];
        simple_parser_from_dna(&parser, dna, reconstructed);
        printf("再構築された文: %s\n", reconstructed);
    } else {
        printf("解析に失敗しました\n");
    }
    
    return 0;
}
#endif