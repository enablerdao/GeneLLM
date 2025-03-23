#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c_programming_router.h"

#define MAX_INPUT_LENGTH 1024
#define MAX_TAGS_LENGTH 256

// 難易度レベルを文字列に変換
const char* get_level_name(int level) {
    switch (level) {
        case 0: return "基本";
        case 1: return "中級";
        case 2: return "上級";
        default: return "不明";
    }
}

// タグを表示
void print_tags(const char* tags) {
    if (!tags || !*tags) {
        printf("タグ: なし\n");
        return;
    }
    
    printf("タグ: %s\n", tags);
}

// 検索モード
void search_mode(CRouter* router) {
    char input[MAX_INPUT_LENGTH];
    
    printf("\n=== 検索モード ===\n");
    printf("検索キーワードを入力してください（終了するには 'exit'）: ");
    
    if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
        return;
    }
    
    // 改行文字を削除
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }
    
    if (strcmp(input, "exit") == 0) {
        return;
    }
    
    // 検索結果を表示
    printf("\n検索結果:\n");
    
    int found = 0;
    for (int i = 0; i < router->pattern_count; i++) {
        if (contains_pattern(router->patterns[i].pattern, input) || 
            contains_pattern(router->patterns[i].tags, input)) {
            
            printf("- %s (難易度: %s)\n", 
                   router->patterns[i].pattern, 
                   get_level_name(router->patterns[i].category));
            
            print_tags(router->patterns[i].tags);
            printf("\n");
            found++;
        }
    }
    
    if (found == 0) {
        printf("該当する項目は見つかりませんでした。\n");
    } else {
        printf("合計 %d 件の項目が見つかりました。\n", found);
    }
}

// 難易度フィルタリングモード
void filter_by_level(CRouter* router, int level) {
    printf("\n=== 難易度: %s のトピック ===\n", get_level_name(level));
    
    int found = 0;
    for (int i = 0; i < router->pattern_count; i++) {
        if (router->patterns[i].category == level) {
            printf("- %s\n", router->patterns[i].pattern);
            print_tags(router->patterns[i].tags);
            printf("\n");
            found++;
        }
    }
    
    if (found == 0) {
        printf("該当する項目は見つかりませんでした。\n");
    } else {
        printf("合計 %d 件の項目が見つかりました。\n", found);
    }
}

// ヘルプを表示
void show_help() {
    printf("\n=== コマンド一覧 ===\n");
    printf("help       - このヘルプを表示\n");
    printf("search     - キーワード検索モード\n");
    printf("basic      - 基本レベルのトピックを表示\n");
    printf("intermediate - 中級レベルのトピックを表示\n");
    printf("advanced   - 上級レベルのトピックを表示\n");
    printf("exit/quit  - プログラムを終了\n");
    printf("\nその他の入力はC言語に関する質問として処理されます。\n");
}

int main() {
    printf("C言語プログラミングルーター - 拡張対話モード\n");
    printf("コマンド一覧を表示するには 'help' と入力してください\n");
    printf("終了するには 'exit' または 'quit' と入力してください\n\n");
    
    // ルーターの初期化
    CRouter* router = init_c_programming_router();
    if (!router) {
        printf("ルーターの初期化に失敗しました\n");
        return 1;
    }
    
    char input[MAX_INPUT_LENGTH];
    
    while (1) {
        printf("\n質問またはコマンドを入力してください: ");
        
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
        
        // 特殊コマンドをチェック
        if (strcmp(input, "help") == 0) {
            show_help();
        } else if (strcmp(input, "search") == 0) {
            search_mode(router);
        } else if (strcmp(input, "basic") == 0) {
            filter_by_level(router, 0);
        } else if (strcmp(input, "intermediate") == 0) {
            filter_by_level(router, 1);
        } else if (strcmp(input, "advanced") == 0) {
            filter_by_level(router, 2);
        } else {
            // 通常の質問として処理
            int category;
            char tags[MAX_TAGS_LENGTH];
            
            printf("\n=== 応答 ===\n");
            char* response = c_router_generate_response_with_details(router, input, &category, tags, MAX_TAGS_LENGTH);
            printf("%s\n", response);
            
            if (category >= 0) {
                printf("\n難易度: %s\n", get_level_name(category));
                print_tags(tags);
            }
        }
    }
    
    // ルーターの解放
    c_router_free(router);
    
    return 0;
}