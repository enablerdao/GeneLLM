#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c_programming_router.c"

int main() {
    // C言語ルーターの初期化
    CRouter* router = init_c_programming_router();
    if (!router) {
        printf("ルーターの初期化に失敗しました\n");
        return 1;
    }
    
    // テスト入力
    const char* test_inputs[] = {
        "C言語でHello Worldを表示するプログラムを教えてください",
        "hello worldプログラムの書き方を教えて",
        "C言語の入門プログラムを教えてください"
    };
    
    // 各入力に対する応答を生成
    for (int i = 0; i < sizeof(test_inputs) / sizeof(test_inputs[0]); i++) {
        printf("入力: %s\n\n", test_inputs[i]);
        
        char* response = c_router_generate_response(router, test_inputs[i]);
        printf("応答:\n%s\n\n", response);
        
        free(response);
        printf("----------------------------------------\n\n");
    }
    
    // ルーターの解放
    c_router_free(router);
    
    return 0;
}
