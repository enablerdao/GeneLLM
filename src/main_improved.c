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
#include "improved/include/answer_db.h"

int main(int argc, char* argv[]) {
    // ロケールを設定（日本語対応）
    setlocale(LC_ALL, "");

    // 乱数の初期化
    srand((unsigned int)time(NULL));

    // ベクトルデータベースを初期化
    init_global_vector_db();
    
    // 回答データベースを初期化
    init_answer_db();

    printf("GeneLLM 簡易版\n");
    printf("ベクトルデータベースのサイズ: %d\n", get_global_vector_db_size());

    // オプションと質問文を処理
    int debug_mode = 0;
    int interactive_mode = 0;
    char* question = NULL;
    
    // 引数の処理
    for (int i = 1; i < argc; i++) {
        // オプションの処理
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                printf("使用法: %s [オプション] \"質問文\"\n", argv[0]);
                printf("\nオプション:\n");
                printf("  -h, --help     このヘルプメッセージを表示\n");
                printf("  -v, --version  バージョン情報を表示\n");
                printf("  -d, --debug    デバッグモードで実行\n");
                printf("  -i, --interactive  対話モードで実行\n");
                return 0;
            } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
                printf("GeneLLM バージョン 0.1.0\n");
                printf("C言語による超軽量AI実装\n");
                return 0;
            } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
                debug_mode = 1;
            } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0) {
                interactive_mode = 1;
            } else {
                printf("警告: 不明なオプション '%s'\n", argv[i]);
            }
        } else {
            // 質問文の処理
            question = argv[i];
        }
    }
    
    // 対話モードの処理
    if (interactive_mode) {
        printf("対話モードを開始します。終了するには 'exit' または 'quit' と入力してください。\n");
        char input[1024];
        while (1) {
            printf("\n> ");
            if (fgets(input, sizeof(input), stdin) == NULL) break;
            
            // 改行文字を削除
            input[strcspn(input, "\n")] = 0;
            
            // 終了コマンドのチェック
            if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
                printf("対話モードを終了します。\n");
                break;
            }
            
            // 質問を処理
            printf("質問: %s\n", input);
            if (debug_mode) {
                printf("デバッグ情報: 対話モードで質問を処理中...\n");
            }
            
            // 回答を検索
            double similarity_score = 0.0;
            const char* answer = find_answer_with_score(input, &similarity_score);
            
            if (debug_mode) {
                printf("デバッグ情報: 類似度スコア: %.4f\n", similarity_score);
                if (similarity_score > 0.5) {
                    printf("デバッグ情報: 十分な類似度があります\n");
                    printf("デバッグ情報: マッチした質問: %s\n", get_matched_question());
                } else {
                    printf("デバッグ情報: 類似度が低いため回答が見つかりません\n");
                }
            }
            
            if (answer != NULL) {
                printf("応答: %s\n", answer);
            } else {
                printf("応答: あなたの質問「%s」に対する回答はまだ実装されていません。\n", input);
            }
        }
        return 0;
    }
    
    // 通常モードの処理
    if (question == NULL) {
        printf("エラー: 質問文が指定されていません。\n");
        printf("使用法: %s [オプション] \"質問文\"\n", argv[0]);
        return 1;
    }
    
    // 質問を表示
    printf("質問: %s\n", question);
    
    // デバッグモードの場合は追加情報を表示
    if (debug_mode) {
        printf("デバッグ情報: 質問を処理中...\n");
        printf("デバッグ情報: 回答データベースサイズ: %d件\n", get_answer_db_size());
    }
    
    // 回答を検索
    double similarity_score = 0.0;
    const char* answer = find_answer_with_score(question, &similarity_score);
    
    if (debug_mode) {
        printf("デバッグ情報: 類似度スコア: %.4f\n", similarity_score);
        if (similarity_score > 0.5) {
            printf("デバッグ情報: 十分な類似度があります\n");
            printf("デバッグ情報: マッチした質問: %s\n", get_matched_question());
        } else {
            printf("デバッグ情報: 類似度が低いため回答が見つかりません\n");
        }
    }
    
    if (answer != NULL) {
        printf("応答: %s\n", answer);
    } else {
        printf("応答: あなたの質問「%s」に対する回答はまだ実装されていません。\n", question);
    }

    return 0;
}