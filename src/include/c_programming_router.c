#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "compiler_validator.h"

#define MAX_INPUT_LENGTH 4096
#define MAX_OUTPUT_LENGTH 8192
#define MAX_PATTERNS 50
#define MAX_PATTERN_LENGTH 256
#define MAX_RESPONSE_LENGTH 4096

// ルーティングパターン
typedef struct {
    char pattern[MAX_PATTERN_LENGTH];  // マッチングパターン
    float confidence;                  // 信頼度
    char response[MAX_RESPONSE_LENGTH]; // 応答テンプレート
    int is_code_generator;            // コード生成器かどうか
    char code_template[MAX_RESPONSE_LENGTH]; // コードテンプレート
} RoutingPattern;

// ルーター
typedef struct {
    RoutingPattern patterns[MAX_PATTERNS]; // ルーティングパターン配列
    int pattern_count;                    // パターン数
} CRouter;

// ルーターの初期化
CRouter* c_router_init() {
    CRouter* router = (CRouter*)malloc(sizeof(CRouter));
    if (!router) {
        fprintf(stderr, "メモリ割り当てエラー: ルーターの初期化に失敗しました\n");
        return NULL;
    }
    
    router->pattern_count = 0;
    
    return router;
}

// ルーターの解放
void c_router_free(CRouter* router) {
    if (router) {
        free(router);
    }
}

// ルーティングパターンの追加
int c_router_add_pattern(CRouter* router, const char* pattern, float confidence, 
                        const char* response, int is_code_generator, const char* code_template) {
    if (!router || router->pattern_count >= MAX_PATTERNS) {
        return 0;
    }
    
    RoutingPattern* p = &router->patterns[router->pattern_count];
    
    strncpy(p->pattern, pattern, MAX_PATTERN_LENGTH - 1);
    p->pattern[MAX_PATTERN_LENGTH - 1] = '\0';
    
    p->confidence = confidence;
    
    strncpy(p->response, response, MAX_RESPONSE_LENGTH - 1);
    p->response[MAX_RESPONSE_LENGTH - 1] = '\0';
    
    p->is_code_generator = is_code_generator;
    
    if (code_template) {
        strncpy(p->code_template, code_template, MAX_RESPONSE_LENGTH - 1);
        p->code_template[MAX_RESPONSE_LENGTH - 1] = '\0';
    } else {
        p->code_template[0] = '\0';
    }
    
    router->pattern_count++;
    return 1;
}

// 文字列が特定のパターンを含むかチェック
int contains_pattern(const char* text, const char* pattern) {
    char* text_lower = strdup(text);
    char* pattern_lower = strdup(pattern);
    
    if (!text_lower || !pattern_lower) {
        if (text_lower) free(text_lower);
        if (pattern_lower) free(pattern_lower);
        return 0;
    }
    
    // 小文字に変換
    for (int i = 0; text_lower[i]; i++) {
        text_lower[i] = tolower(text_lower[i]);
    }
    
    for (int i = 0; pattern_lower[i]; i++) {
        pattern_lower[i] = tolower(pattern_lower[i]);
    }
    
    int result = strstr(text_lower, pattern_lower) != NULL;
    
    free(text_lower);
    free(pattern_lower);
    
    return result;
}

// 入力に対する最適なルーティングパターンを見つける
RoutingPattern* c_router_find_best_match(CRouter* router, const char* input) {
    if (!router || !input) {
        return NULL;
    }
    
    RoutingPattern* best_match = NULL;
    float best_confidence = 0.0f;
    
    for (int i = 0; i < router->pattern_count; i++) {
        RoutingPattern* pattern = &router->patterns[i];
        
        if (contains_pattern(input, pattern->pattern) && pattern->confidence > best_confidence) {
            best_match = pattern;
            best_confidence = pattern->confidence;
        }
    }
    
    return best_match;
}

// コードテンプレートを入力に基づいてカスタマイズ
char* customize_code_template(const char* template, const char* input) {
    static char customized[MAX_RESPONSE_LENGTH];
    strcpy(customized, template);
    
    // ここでは簡単な置換のみを行う
    // 実際のシステムではより高度なカスタマイズが必要
    
    // 例: %FUNCTION_NAME% を適切な関数名に置換
    char function_name[64] = "example_function";
    
    // 入力から関数名を抽出する簡易ロジック
    if (strstr(input, "ソート") || strstr(input, "sort")) {
        strcpy(function_name, "sort_array");
    } else if (strstr(input, "検索") || strstr(input, "search")) {
        strcpy(function_name, "search_element");
    } else if (strstr(input, "計算") || strstr(input, "calculate")) {
        strcpy(function_name, "calculate_result");
    }
    
    // テンプレート内のプレースホルダーを置換
    char* placeholder = strstr(customized, "%FUNCTION_NAME%");
    while (placeholder) {
        // プレースホルダーを関数名で置換
        memmove(placeholder + strlen(function_name), 
                placeholder + 15, // %FUNCTION_NAME% の長さ
                strlen(placeholder + 15) + 1); // +1 for null terminator
        
        memcpy(placeholder, function_name, strlen(function_name));
        
        // 次のプレースホルダーを検索
        placeholder = strstr(placeholder + strlen(function_name), "%FUNCTION_NAME%");
    }
    
    return customized;
}

// 入力に対する応答を生成
char* c_router_generate_response(CRouter* router, const char* input) {
    static char response[MAX_OUTPUT_LENGTH];
    response[0] = '\0';
    
    RoutingPattern* best_match = c_router_find_best_match(router, input);
    if (!best_match) {
        strcpy(response, "申し訳ありませんが、C言語に関連する質問として認識できませんでした。");
        return response;
    }
    
    if (best_match->is_code_generator) {
        // コード生成の場合
        char* code = customize_code_template(best_match->code_template, input);
        
        // コードの検証
        char temp_filename[] = "temp_generated_code.c";
        CompilerValidationResult validation = validate_with_compiler(code, temp_filename, "-Wall -Wextra -std=c99");
        
        // 応答の生成
        sprintf(response, "%s\n\n```c\n%s\n```\n\n", best_match->response, code);
        
        // コンパイラの警告やエラーがあれば追加
        if (validation.warning_count > 0 || validation.error_count > 0) {
            strcat(response, "\n**コンパイラの警告/エラー:**\n```\n");
            strcat(response, validation.output);
            strcat(response, "\n```\n");
            
            // 修正案の提案
            if (validation.error_count > 0) {
                strcat(response, "\n**修正案:**\n");
                // ここでは簡易的な修正案を提示
                if (strstr(validation.output, "undefined reference")) {
                    strcat(response, "- 未定義の関数や変数が使用されています。宣言や定義を確認してください。\n");
                } else if (strstr(validation.output, "implicit declaration")) {
                    strcat(response, "- 関数の暗黙的な宣言があります。適切なヘッダファイルをインクルードするか、関数プロトタイプを追加してください。\n");
                }
            }
        } else {
            strcat(response, "\nコードは正常にコンパイルされました。\n");
        }
        
        // 一時ファイルを削除
        remove(temp_filename);
    } else {
        // 通常の応答
        strcpy(response, best_match->response);
    }
    
    return response;
}

// C言語ルーターの初期化と基本パターンの設定
CRouter* init_c_programming_router() {
    CRouter* router = c_router_init();
    if (!router) {
        return NULL;
    }
    
    // 基本的なC言語パターンを追加
    
    // 1. 配列操作
    c_router_add_pattern(router, "配列", 0.8f, 
        "C言語の配列は連続したメモリ領域に同じ型の要素を格納するデータ構造です。", 0, NULL);
    
    // 2. ポインタ
    c_router_add_pattern(router, "ポインタ", 0.9f, 
        "ポインタはメモリアドレスを格納する変数で、C言語の強力な機能の一つです。", 0, NULL);
    
    // 3. メモリ管理
    c_router_add_pattern(router, "メモリ", 0.8f, 
        "C言語ではmalloc()、free()などの関数を使って動的にメモリを管理します。", 0, NULL);
    
    // 4. 構造体
    c_router_add_pattern(router, "構造体", 0.8f, 
        "構造体(struct)は異なる型のデータをグループ化するためのユーザー定義型です。", 0, NULL);
    
    // 5. ファイル操作
    c_router_add_pattern(router, "ファイル", 0.8f, 
        "C言語ではfopen()、fread()、fwrite()などの関数を使ってファイル操作を行います。", 0, NULL);
    
    // コード生成パターン
    
    // 1. 配列のソート
    c_router_add_pattern(router, "配列 ソート", 0.9f, 
        "以下は整数配列をバブルソートするC言語の関数です：", 1,
        "void %FUNCTION_NAME%(int arr[], int size) {\n"
        "    for (int i = 0; i < size - 1; i++) {\n"
        "        for (int j = 0; j < size - i - 1; j++) {\n"
        "            if (arr[j] > arr[j + 1]) {\n"
        "                // 要素を交換\n"
        "                int temp = arr[j];\n"
        "                arr[j] = arr[j + 1];\n"
        "                arr[j + 1] = temp;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}\n\n"
        "// 使用例\n"
        "int main() {\n"
        "    int numbers[] = {64, 34, 25, 12, 22, 11, 90};\n"
        "    int n = sizeof(numbers) / sizeof(numbers[0]);\n"
        "    \n"
        "    printf(\"ソート前の配列: \");\n"
        "    for (int i = 0; i < n; i++) {\n"
        "        printf(\"%d \", numbers[i]);\n"
        "    }\n"
        "    \n"
        "    %FUNCTION_NAME%(numbers, n);\n"
        "    \n"
        "    printf(\"\\nソート後の配列: \");\n"
        "    for (int i = 0; i < n; i++) {\n"
        "        printf(\"%d \", numbers[i]);\n"
        "    }\n"
        "    \n"
        "    return 0;\n"
        "}");
    
    // 2. 線形検索
    c_router_add_pattern(router, "配列 検索", 0.9f, 
        "以下は整数配列内の要素を線形検索するC言語の関数です：", 1,
        "int %FUNCTION_NAME%(int arr[], int size, int key) {\n"
        "    for (int i = 0; i < size; i++) {\n"
        "        if (arr[i] == key) {\n"
        "            return i; // 要素が見つかった場合、インデックスを返す\n"
        "        }\n"
        "    }\n"
        "    return -1; // 要素が見つからなかった場合\n"
        "}\n\n"
        "// 使用例\n"
        "int main() {\n"
        "    int numbers[] = {10, 23, 5, 17, 42, 8, 31};\n"
        "    int n = sizeof(numbers) / sizeof(numbers[0]);\n"
        "    int key = 17;\n"
        "    \n"
        "    int result = %FUNCTION_NAME%(numbers, n, key);\n"
        "    \n"
        "    if (result != -1) {\n"
        "        printf(\"%dは配列内のインデックス%dに存在します\\n\", key, result);\n"
        "    } else {\n"
        "        printf(\"%dは配列内に存在しません\\n\", key);\n"
        "    }\n"
        "    \n"
        "    return 0;\n"
        "}");
    
    // 3. 構造体の定義と使用
    c_router_add_pattern(router, "構造体 定義", 0.9f, 
        "以下は構造体を定義して使用するC言語のコード例です：", 1,
        "// 構造体の定義\n"
        "typedef struct {\n"
        "    char name[50];\n"
        "    int age;\n"
        "    float height;\n"
        "} Person;\n\n"
        "// 構造体を操作する関数\n"
        "void %FUNCTION_NAME%(Person* person) {\n"
        "    printf(\"名前: %s\\n\", person->name);\n"
        "    printf(\"年齢: %d\\n\", person->age);\n"
        "    printf(\"身長: %.2f cm\\n\", person->height);\n"
        "}\n\n"
        "int main() {\n"
        "    // 構造体変数の宣言と初期化\n"
        "    Person person1 = {\"山田太郎\", 30, 175.5};\n"
        "    \n"
        "    // 構造体メンバへのアクセス\n"
        "    printf(\"構造体の直接アクセス:\\n\");\n"
        "    printf(\"名前: %s\\n\", person1.name);\n"
        "    printf(\"年齢: %d\\n\", person1.age);\n"
        "    printf(\"身長: %.2f cm\\n\\n\", person1.height);\n"
        "    \n"
        "    // 関数を使った構造体の操作\n"
        "    printf(\"関数を使った構造体の操作:\\n\");\n"
        "    %FUNCTION_NAME%(&person1);\n"
        "    \n"
        "    return 0;\n"
        "}");
    
    // 4. ファイル操作
    c_router_add_pattern(router, "ファイル 読み込み", 0.9f, 
        "以下はファイルを読み込むC言語のコード例です：", 1,
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n\n"
        "void %FUNCTION_NAME%(const char* filename) {\n"
        "    FILE* file = fopen(filename, \"r\");\n"
        "    if (file == NULL) {\n"
        "        printf(\"ファイル '%s' を開けませんでした\\n\", filename);\n"
        "        return;\n"
        "    }\n"
        "    \n"
        "    char buffer[256];\n"
        "    int line_count = 0;\n"
        "    \n"
        "    // ファイルを1行ずつ読み込む\n"
        "    while (fgets(buffer, sizeof(buffer), file) != NULL) {\n"
        "        printf(\"%3d: %s\", ++line_count, buffer);\n"
        "    }\n"
        "    \n"
        "    fclose(file);\n"
        "}\n\n"
        "int main() {\n"
        "    const char* filename = \"example.txt\";\n"
        "    \n"
        "    // テスト用のファイルを作成\n"
        "    FILE* test_file = fopen(filename, \"w\");\n"
        "    if (test_file != NULL) {\n"
        "        fprintf(test_file, \"これはテストファイルです。\\n\");\n"
        "        fprintf(test_file, \"C言語でファイル操作を学んでいます。\\n\");\n"
        "        fprintf(test_file, \"ファイルの読み書きは重要なスキルです。\\n\");\n"
        "        fclose(test_file);\n"
        "        \n"
        "        // ファイルを読み込む\n"
        "        printf(\"%sの内容:\\n\", filename);\n"
        "        %FUNCTION_NAME%(filename);\n"
        "    } else {\n"
        "        printf(\"テストファイルを作成できませんでした\\n\");\n"
        "    }\n"
        "    \n"
        "    return 0;\n"
        "}");
    
    // 5. メモリ管理
    c_router_add_pattern(router, "動的メモリ", 0.9f, 
        "以下は動的メモリ管理を行うC言語のコード例です：", 1,
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n\n"
        "int* %FUNCTION_NAME%(int size) {\n"
        "    // 動的にメモリを確保\n"
        "    int* array = (int*)malloc(size * sizeof(int));\n"
        "    \n"
        "    if (array == NULL) {\n"
        "        printf(\"メモリの確保に失敗しました\\n\");\n"
        "        return NULL;\n"
        "    }\n"
        "    \n"
        "    // 配列を初期化\n"
        "    for (int i = 0; i < size; i++) {\n"
        "        array[i] = i * 10;\n"
        "    }\n"
        "    \n"
        "    return array;\n"
        "}\n\n"
        "int main() {\n"
        "    int size = 5;\n"
        "    \n"
        "    // 動的配列を作成\n"
        "    int* dynamic_array = %FUNCTION_NAME%(size);\n"
        "    \n"
        "    if (dynamic_array != NULL) {\n"
        "        printf(\"動的に確保した配列の内容:\\n\");\n"
        "        for (int i = 0; i < size; i++) {\n"
        "            printf(\"%d \", dynamic_array[i]);\n"
        "        }\n"
        "        printf(\"\\n\");\n"
        "        \n"
        "        // メモリを解放\n"
        "        free(dynamic_array);\n"
        "        printf(\"メモリを解放しました\\n\");\n"
        "    }\n"
        "    \n"
        "    return 0;\n"
        "}");
    
    return router;
}

// テスト用のメイン関数
#ifdef TEST_C_PROGRAMMING_ROUTER
int main() {
    // C言語ルーターの初期化
    CRouter* router = init_c_programming_router();
    if (!router) {
        printf("ルーターの初期化に失敗しました\n");
        return 1;
    }
    
    // テスト入力
    const char* test_inputs[] = {
        "C言語でポインタの使い方を教えてください",
        "配列のソートアルゴリズムを実装したい",
        "構造体の定義方法を教えてください",
        "ファイルから読み込む方法は？",
        "動的メモリ確保の方法を教えて",
        "Pythonでリストを操作する方法は？" // C言語に関係ない質問
    };
    
    // 各入力に対する応答を生成
    for (int i = 0; i < sizeof(test_inputs) / sizeof(test_inputs[0]); i++) {
        printf("入力: %s\n\n", test_inputs[i]);
        
        char* response = c_router_generate_response(router, test_inputs[i]);
        printf("応答:\n%s\n\n", response);
        
        printf("----------------------------------------\n\n");
    }
    
    // ルーターの解放
    c_router_free(router);
    
    return 0;
}
#endif