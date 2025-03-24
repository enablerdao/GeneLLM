#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mecab.h>
#include <locale.h>
#include <time.h>
#include <unistd.h>
#include <curl/curl.h>
#include <sys/stat.h>

#include "include/vector_db.h"

int main(int argc, char* argv[]) {
    // ロケールを設定（日本語対応）
    setlocale(LC_ALL, "");
    
    // 乱数の初期化
    srand((unsigned int)time(NULL));
    
    // ベクトルデータベースを初期化
    init_global_vector_db();
    
    printf("GeneLLM 簡易版\n");
    printf("ベクトルデータベースのサイズ: %d\n", get_global_vector_db_size());
    
    // コマンドライン引数の解析
    if (argc < 2) {
        printf("使用法: %s [オプション] \"質問文\"\n", argv[0]);
        printf("\nオプション:\n");
        printf("  -h, --help     このヘルプメッセージを表示\n");
        printf("  -v, --version  バージョン情報を表示\n");
        printf("  -d, --debug    デバッグモードで実行\n");
        printf("  -i, --interactive  対話モードで実行\n");
        return 1;
    }
    
    // ヘルプオプションの確認
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        printf("使用法: %s [オプション] \"質問文\"\n", argv[0]);
        printf("\nオプション:\n");
        printf("  -h, --help     このヘルプメッセージを表示\n");
        printf("  -v, --version  バージョン情報を表示\n");
        printf("  -d, --debug    デバッグモードで実行\n");
        printf("  -i, --interactive  対話モードで実行\n");
        return 0;
    }
    
    // バージョン情報の確認
    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
        printf("GeneLLM バージョン 0.1.0\n");
        printf("C言語による超軽量AI実装\n");
        return 0;
    }
    
    // 質問を表示
    printf("質問: %s\n", argv[1]);
    
    // 簡単な応答を生成
    printf("応答: あなたの質問「%s」に対する回答はまだ実装されていません。\n", argv[1]);
    
    return 0;
}
