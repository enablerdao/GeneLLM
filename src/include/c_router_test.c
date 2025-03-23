#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c_programming_router.h"

#define MAX_INPUT_LENGTH 1024

int main() {
    printf("C言語プログラミングルーター - 対話モード\n");
    printf("終了するには 'exit' または 'quit' と入力してください\n\n");
    
    // ルーターの初期化
    CRouter* router = init_c_programming_router();
    if (!router) {
        printf("ルーターの初期化に失敗しました\n");
        return 1;
    }
    
    char input[MAX_INPUT_LENGTH];
    
    while (1) {
        printf("\n質問を入力してください: ");
        
        // 入力を取得
        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
            break;
        }
        
        // 改行文字を削除
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        
        // 終了コマンドをチェック
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            printf("プログラムを終了します\n");
            break;
        }
        
        // 応答を生成
        printf("\n=== 応答 ===\n");
        char* response = c_router_generate_response(router, input);
        printf("%s\n", response);
    }
    
    // ルーターの解放
    c_router_free(router);
    
    return 0;
}