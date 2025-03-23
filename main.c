#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_STR_LEN 1024

// 機能の種類
typedef enum {
    SYNTAX_ANALYSIS,    // 構文解析
    DNA_COMPRESSION,    // DNA圧縮
    VECTOR_SEARCH,      // ベクトル検索
    GRAPH_GENERATION,   // グラフ生成
    ROUTER_MODEL,       // ルーターモデル
    UNKNOWN_FUNCTION    // 不明な機能
} FunctionType;

// 機能の説明を表示
void print_usage() {
    printf("C言語による超軽量AI実装プロジェクト\n");
    printf("\n");
    printf("使用方法: ./main <機能> [引数...]\n");
    printf("\n");
    printf("機能一覧:\n");
    printf("  1. syntax     - 構文解析（例: ./main syntax \"猫が魚を食べて嬉しかった。\"）\n");
    printf("  2. dna        - DNA圧縮（例: ./main dna \"猫\" \"食べる\" \"嬉しい\"）\n");
    printf("  3. vector     - ベクトル検索（例: ./main vector）\n");
    printf("  4. graph      - グラフ生成（例: ./main graph）\n");
    printf("  5. router     - ルーターモデル（例: ./main router \"今日はとても嬉しい気持ちです。\"）\n");
    printf("\n");
}

// 機能の種類を文字列から取得
FunctionType get_function_type(const char* function_str) {
    if (strcmp(function_str, "syntax") == 0) {
        return SYNTAX_ANALYSIS;
    } else if (strcmp(function_str, "dna") == 0) {
        return DNA_COMPRESSION;
    } else if (strcmp(function_str, "vector") == 0) {
        return VECTOR_SEARCH;
    } else if (strcmp(function_str, "graph") == 0) {
        return GRAPH_GENERATION;
    } else if (strcmp(function_str, "router") == 0) {
        return ROUTER_MODEL;
    } else {
        return UNKNOWN_FUNCTION;
    }
}

// 外部プログラムを実行
void execute_program(const char* program, const char* args) {
    char command[MAX_STR_LEN];
    snprintf(command, MAX_STR_LEN, "./%s %s", program, args);
    system(command);
}

int main(int argc, char* argv[]) {
    // 引数のチェック
    if (argc < 2) {
        print_usage();
        return 1;
    }
    
    // 機能の種類を取得
    FunctionType function_type = get_function_type(argv[1]);
    
    // 引数の文字列を構築
    char args[MAX_STR_LEN] = "";
    for (int i = 2; i < argc; i++) {
        strcat(args, "\"");
        strcat(args, argv[i]);
        strcat(args, "\" ");
    }
    
    // 機能に応じて処理を実行
    switch (function_type) {
        case SYNTAX_ANALYSIS:
            if (argc < 3) {
                printf("構文解析には文の引数が必要です。\n");
                printf("例: ./main syntax \"猫が魚を食べて嬉しかった。\"\n");
                return 1;
            }
            execute_program("simple_analyzer", args);
            break;
            
        case DNA_COMPRESSION:
            if (argc < 5) {
                printf("DNA圧縮には主語、動詞、結果の3つの引数が必要です。\n");
                printf("例: ./main dna \"猫\" \"食べる\" \"嬉しい\"\n");
                return 1;
            }
            execute_program("dna_compressor", args);
            break;
            
        case VECTOR_SEARCH:
            execute_program("vector_search", "");
            break;
            
        case GRAPH_GENERATION:
            execute_program("graph_generator", "");
            break;
            
        case ROUTER_MODEL:
            if (argc < 3) {
                printf("ルーターモデルには文の引数が必要です。\n");
                printf("例: ./main router \"今日はとても嬉しい気持ちです。\"\n");
                return 1;
            }
            execute_program("c_implementation/improved_router_model", args);
            break;
            
        case UNKNOWN_FUNCTION:
        default:
            printf("不明な機能: %s\n", argv[1]);
            print_usage();
            return 1;
    }
    
    return 0;
}