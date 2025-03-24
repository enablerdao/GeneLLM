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
    int category;                     // カテゴリ（0: 基本, 1: 中級, 2: 上級）
    char tags[256];                   // 関連タグ（カンマ区切り）
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

// ルーティングパターンの追加（拡張版）
int c_router_add_pattern_ex(CRouter* router, const char* pattern, float confidence, 
                          const char* response, int is_code_generator, const char* code_template,
                          int category, const char* tags) {
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
    
    p->category = category;
    
    if (tags) {
        strncpy(p->tags, tags, sizeof(p->tags) - 1);
        p->tags[sizeof(p->tags) - 1] = '\0';
    } else {
        p->tags[0] = '\0';
    }
    
    router->pattern_count++;
    return 1;
}

// 後方互換性のための関数
int c_router_add_pattern(CRouter* router, const char* pattern, float confidence, 
                        const char* response, int is_code_generator, const char* code_template) {
    return c_router_add_pattern_ex(router, pattern, confidence, response, 
                                 is_code_generator, code_template, 0, NULL);
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

// 入力に対する応答を生成（詳細情報付き）
char* c_router_generate_response_with_details(CRouter* router, const char* input, int* category, char* tags_buffer, size_t tags_buffer_size) {
    static char response[MAX_OUTPUT_LENGTH];
    response[0] = '\0';
    
    RoutingPattern* best_match = c_router_find_best_match(router, input);
    if (!best_match) {
        strcpy(response, "申し訳ありませんが、C言語に関連する質問として認識できませんでした。");
        if (category) *category = -1;
        if (tags_buffer) tags_buffer[0] = '\0';
        return response;
    }
    
    // カテゴリと関連タグを返す
    if (category) *category = best_match->category;
    if (tags_buffer && tags_buffer_size > 0) {
        strncpy(tags_buffer, best_match->tags, tags_buffer_size - 1);
        tags_buffer[tags_buffer_size - 1] = '\0';
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
            
            // 実行可能なコードの場合は実行結果も表示
            if (validation.success) {
                char output_filename[] = "temp_executable";
                char compile_output[MAX_OUTPUT_LENGTH];
                
                if (compile_code(code, temp_filename, output_filename, "-Wall -Wextra -std=c99", compile_output)) {
                    strcat(response, "\n**実行結果:**\n```\n");
                    char* run_output = run_compiled_program(output_filename, "");
                    strncat(response, run_output, MAX_OUTPUT_LENGTH - strlen(response) - 1);
                    strcat(response, "\n```\n");
                    
                    // 実行ファイルを削除
                    remove(output_filename);
                }
            }
        }
        
        // 一時ファイルを削除
        remove(temp_filename);
        
        // 難易度に応じた追加情報
        if (best_match->category == 1) { // 中級
            strcat(response, "\n**補足情報（中級）:**\n");
            strcat(response, "このコードは基本的な実装ですが、より効率的なアルゴリズムや最適化の余地があります。\n");
        } else if (best_match->category == 2) { // 上級
            strcat(response, "\n**補足情報（上級）:**\n");
            strcat(response, "このコードは高度な概念を含んでいます。メモリ管理やパフォーマンスに注意してください。\n");
        }
    } else {
        // 通常の応答
        strcpy(response, best_match->response);
        
        // 難易度に応じた追加情報
        if (best_match->category == 1) { // 中級
            strcat(response, "\n\n**補足情報（中級）:**\n");
            strcat(response, "この概念は基本的なC言語の知識を前提としています。\n");
        } else if (best_match->category == 2) { // 上級
            strcat(response, "\n\n**補足情報（上級）:**\n");
            strcat(response, "これは高度なC言語の概念です。基本的な構文やメモリ管理の理解が前提となります。\n");
        }
    }
    
    return response;
}

// 後方互換性のための関数
char* c_router_generate_response(CRouter* router, const char* input) {
    return c_router_generate_response_with_details(router, input, NULL, NULL, 0);
}

// C言語ルーターの初期化と基本パターンの設定
CRouter* init_c_programming_router() {
    CRouter* router = c_router_init();
    if (!router) {
        return NULL;
    }
    
    // 基本的なC言語パターン（基本レベル）
    
    // 1. 配列操作
    c_router_add_pattern_ex(router, "配列", 0.8f, 
        "C言語の配列は連続したメモリ領域に同じ型の要素を格納するデータ構造です。配列の宣言は `int arr[10];` のように行い、インデックスは0から始まります。", 
        0, NULL, 0, "配列,基本,データ構造");
    
    // 2. ポインタ
    c_router_add_pattern_ex(router, "ポインタ", 0.9f, 
        "ポインタはメモリアドレスを格納する変数で、C言語の強力な機能の一つです。`int *p;` のように宣言し、`&`演算子でアドレスを取得、`*`演算子で参照先の値にアクセスします。", 
        0, NULL, 0, "ポインタ,基本,メモリ");
    
    // 3. メモリ管理
    c_router_add_pattern_ex(router, "メモリ", 0.8f, 
        "C言語ではmalloc()、free()などの関数を使って動的にメモリを管理します。これらの関数を使用するには `#include <stdlib.h>` が必要です。", 
        0, NULL, 0, "メモリ,基本,malloc,free");
    
    // 4. 構造体
    c_router_add_pattern_ex(router, "構造体", 0.8f, 
        "構造体(struct)は異なる型のデータをグループ化するためのユーザー定義型です。`struct Person { char name[50]; int age; };` のように定義します。", 
        0, NULL, 0, "構造体,基本,データ型");
    
    // 5. ファイル操作
    c_router_add_pattern_ex(router, "ファイル", 0.8f, 
        "C言語ではfopen()、fread()、fwrite()などの関数を使ってファイル操作を行います。これらの関数を使用するには `#include <stdio.h>` が必要です。", 
        0, NULL, 0, "ファイル,基本,入出力");
    
    // 中級レベルのパターン
    
    // 1. 関数ポインタ
    c_router_add_pattern_ex(router, "関数ポインタ", 0.9f, 
        "関数ポインタは関数のアドレスを格納する変数で、コールバック関数の実装などに使用されます。`int (*func_ptr)(int, int);` のように宣言します。", 
        0, NULL, 1, "ポインタ,関数,中級");
    
    // 2. ビット操作
    c_router_add_pattern_ex(router, "ビット操作", 0.9f, 
        "C言語ではビット単位の論理演算子（&, |, ^, ~）やシフト演算子（<<, >>）を使ってビット操作を行えます。これはハードウェア制御や最適化に役立ちます。", 
        0, NULL, 1, "ビット,演算子,中級");
    
    // 3. 再帰関数
    c_router_add_pattern_ex(router, "再帰", 0.8f, 
        "再帰関数は自分自身を呼び出す関数です。複雑な問題を単純な部分問題に分割して解決するのに役立ちますが、スタックオーバーフローに注意が必要です。", 
        0, NULL, 1, "関数,再帰,中級,アルゴリズム");
    
    // 上級レベルのパターン
    
    // 1. マルチスレッド
    c_router_add_pattern_ex(router, "スレッド", 0.9f, 
        "C言語でマルチスレッドプログラミングを行うには、POSIXスレッド（pthread）ライブラリを使用します。スレッド間の同期には、ミューテックスやセマフォを使用します。", 
        0, NULL, 2, "スレッド,pthread,上級,並行処理");
    
    // 2. メモリアライメント
    c_router_add_pattern_ex(router, "アライメント", 0.9f, 
        "メモリアライメントはデータ構造のメモリ配置に関する概念です。適切なアライメントはパフォーマンスに大きく影響し、特に構造体のパディングを理解することが重要です。", 
        0, NULL, 2, "メモリ,最適化,上級");
    
    // 3. シグナル処理
    c_router_add_pattern_ex(router, "シグナル", 0.9f, 
        "シグナルはプロセスに送信される非同期通知です。signal()関数でシグナルハンドラを設定し、特定のシグナルを受信したときの動作をカスタマイズできます。", 
        0, NULL, 2, "プロセス,シグナル,上級");
    
    // コード生成パターン（基本レベル）
    
    // 1. Hello World
    c_router_add_pattern_ex(router, "hello world", 0.9f,
        "以下はC言語で「Hello, World!」を表示する基本的なプログラムです：", 1,
        "#include <stdio.h>\n\n"
        "int main() {\n"
        "    printf(\"Hello, World!\\n\");\n"
        "    return 0;\n"
        "}", 0, "基本,入門,printf");
    
    // 2. 配列のソート（基本）
    c_router_add_pattern_ex(router, "配列 ソート", 0.9f, 
        "以下は整数配列をバブルソートするC言語の関数です：", 1,
        "#include <stdio.h>\n\n"
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
        "    printf(\"\\n\");\n"
        "    \n"
        "    return 0;\n"
        "}", 0, "配列,ソート,バブルソート,基本,アルゴリズム");
    
    // 3. 線形検索（基本）
    c_router_add_pattern_ex(router, "配列 検索", 0.9f, 
        "以下は整数配列内の要素を線形検索するC言語の関数です：", 1,
        "#include <stdio.h>\n\n"
        "int %FUNCTION_NAME%(int arr[], int size, int key) {\n"
        "    for (int i = 0; i < size; i++) {\n"
        "        if (arr[i] == key) {\n"
        "            return i; // 要素が見つかった場合、インデックスを返す\n"
        "        }\n"
        "    }\n"
        "    return -1; // 要素が見つからなかった場合\n"
        "}\n\n"
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
        "}", 0, "配列,検索,線形検索,基本,アルゴリズム");
    
    // 4. 構造体の定義と使用（基本）
    c_router_add_pattern_ex(router, "構造体 定義", 0.9f, 
        "以下は構造体を定義して使用するC言語のコード例です：", 1,
        "#include <stdio.h>\n\n"
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
        "}", 0, "構造体,typedef,基本,データ型");
    
    // 5. ファイル操作（基本）
    c_router_add_pattern_ex(router, "ファイル 読み込み", 0.9f, 
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
        "}", 0, "ファイル,入出力,fopen,基本");
    
    // 6. メモリ管理（基本）
    c_router_add_pattern_ex(router, "動的メモリ", 0.9f, 
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
        "}", 0, "メモリ,malloc,free,基本");
    
    // 中級レベルのコード生成パターン
    
    // 1. クイックソート（中級）
    c_router_add_pattern_ex(router, "クイックソート", 0.9f, 
        "以下はクイックソートアルゴリズムを実装したC言語のコード例です：", 1,
        "#include <stdio.h>\n\n"
        "// 配列の2要素を交換する関数\n"
        "void swap(int* a, int* b) {\n"
        "    int temp = *a;\n"
        "    *a = *b;\n"
        "    *b = temp;\n"
        "}\n\n"
        "// パーティション処理（ピボットを中心に配列を分割）\n"
        "int partition(int arr[], int low, int high) {\n"
        "    int pivot = arr[high];  // ピボットとして最後の要素を選択\n"
        "    int i = (low - 1);      // 小さい要素のインデックス\n"
        "    \n"
        "    for (int j = low; j <= high - 1; j++) {\n"
        "        // 現在の要素がピボット以下の場合\n"
        "        if (arr[j] <= pivot) {\n"
        "            i++;    // 小さい要素のインデックスをインクリメント\n"
        "            swap(&arr[i], &arr[j]);\n"
        "        }\n"
        "    }\n"
        "    swap(&arr[i + 1], &arr[high]);\n"
        "    return (i + 1);\n"
        "}\n\n"
        "// クイックソートの再帰関数\n"
        "void %FUNCTION_NAME%(int arr[], int low, int high) {\n"
        "    if (low < high) {\n"
        "        // パーティションインデックスを取得\n"
        "        int pi = partition(arr, low, high);\n"
        "        \n"
        "        // パーティションの前後をそれぞれソート\n"
        "        %FUNCTION_NAME%(arr, low, pi - 1);\n"
        "        %FUNCTION_NAME%(arr, pi + 1, high);\n"
        "    }\n"
        "}\n\n"
        "int main() {\n"
        "    int arr[] = {10, 7, 8, 9, 1, 5, 2, 4, 3, 6};\n"
        "    int n = sizeof(arr) / sizeof(arr[0]);\n"
        "    \n"
        "    printf(\"ソート前の配列: \");\n"
        "    for (int i = 0; i < n; i++) {\n"
        "        printf(\"%d \", arr[i]);\n"
        "    }\n"
        "    \n"
        "    %FUNCTION_NAME%(arr, 0, n - 1);\n"
        "    \n"
        "    printf(\"\\nソート後の配列: \");\n"
        "    for (int i = 0; i < n; i++) {\n"
        "        printf(\"%d \", arr[i]);\n"
        "    }\n"
        "    printf(\"\\n\");\n"
        "    \n"
        "    return 0;\n"
        "}", 1, "ソート,クイックソート,再帰,中級,アルゴリズム");
    
    // 2. 二分探索（中級）
    c_router_add_pattern_ex(router, "二分探索", 0.9f, 
        "以下は二分探索アルゴリズムを実装したC言語のコード例です：", 1,
        "#include <stdio.h>\n\n"
        "// 二分探索関数（ソート済み配列が前提）\n"
        "int %FUNCTION_NAME%(int arr[], int left, int right, int key) {\n"
        "    while (left <= right) {\n"
        "        int mid = left + (right - left) / 2; // オーバーフロー対策\n"
        "        \n"
        "        // 中央の要素が探索キーと一致\n"
        "        if (arr[mid] == key) {\n"
        "            return mid;\n"
        "        }\n"
        "        \n"
        "        // キーが中央より大きい場合は右半分を探索\n"
        "        if (arr[mid] < key) {\n"
        "            left = mid + 1;\n"
        "        }\n"
        "        // キーが中央より小さい場合は左半分を探索\n"
        "        else {\n"
        "            right = mid - 1;\n"
        "        }\n"
        "    }\n"
        "    \n"
        "    // 要素が見つからなかった場合\n"
        "    return -1;\n"
        "}\n\n"
        "int main() {\n"
        "    // ソート済みの配列\n"
        "    int arr[] = {2, 5, 8, 12, 16, 23, 38, 45, 56, 72, 91};\n"
        "    int n = sizeof(arr) / sizeof(arr[0]);\n"
        "    int key = 23;\n"
        "    \n"
        "    printf(\"配列: \");\n"
        "    for (int i = 0; i < n; i++) {\n"
        "        printf(\"%d \", arr[i]);\n"
        "    }\n"
        "    printf(\"\\n\");\n"
        "    \n"
        "    int result = %FUNCTION_NAME%(arr, 0, n - 1, key);\n"
        "    \n"
        "    if (result != -1) {\n"
        "        printf(\"%dは配列内のインデックス%dに存在します\\n\", key, result);\n"
        "    } else {\n"
        "        printf(\"%dは配列内に存在しません\\n\", key);\n"
        "    }\n"
        "    \n"
        "    return 0;\n"
        "}", 1, "検索,二分探索,中級,アルゴリズム,効率");
    
    // 3. 連結リスト（中級）
    c_router_add_pattern_ex(router, "連結リスト", 0.9f, 
        "以下は単方向連結リストを実装したC言語のコード例です：", 1,
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n\n"
        "// リストのノード構造体\n"
        "typedef struct Node {\n"
        "    int data;\n"
        "    struct Node* next;\n"
        "} Node;\n\n"
        "// 新しいノードを作成する関数\n"
        "Node* create_node(int data) {\n"
        "    Node* new_node = (Node*)malloc(sizeof(Node));\n"
        "    if (new_node == NULL) {\n"
        "        printf(\"メモリ割り当てエラー\\n\");\n"
        "        exit(1);\n"
        "    }\n"
        "    new_node->data = data;\n"
        "    new_node->next = NULL;\n"
        "    return new_node;\n"
        "}\n\n"
        "// リストの先頭に要素を追加\n"
        "Node* insert_at_beginning(Node* head, int data) {\n"
        "    Node* new_node = create_node(data);\n"
        "    new_node->next = head;\n"
        "    return new_node;\n"
        "}\n\n"
        "// リストの末尾に要素を追加\n"
        "Node* insert_at_end(Node* head, int data) {\n"
        "    Node* new_node = create_node(data);\n"
        "    \n"
        "    // リストが空の場合\n"
        "    if (head == NULL) {\n"
        "        return new_node;\n"
        "    }\n"
        "    \n"
        "    // 末尾を探す\n"
        "    Node* current = head;\n"
        "    while (current->next != NULL) {\n"
        "        current = current->next;\n"
        "    }\n"
        "    \n"
        "    // 末尾に新しいノードを追加\n"
        "    current->next = new_node;\n"
        "    return head;\n"
        "}\n\n"
        "// リストの要素を表示\n"
        "void display_list(Node* head) {\n"
        "    Node* current = head;\n"
        "    \n"
        "    if (current == NULL) {\n"
        "        printf(\"リストは空です\\n\");\n"
        "        return;\n"
        "    }\n"
        "    \n"
        "    printf(\"リスト: \");\n"
        "    while (current != NULL) {\n"
        "        printf(\"%d -> \", current->data);\n"
        "        current = current->next;\n"
        "    }\n"
        "    printf(\"NULL\\n\");\n"
        "}\n\n"
        "// リストのメモリを解放\n"
        "void free_list(Node* head) {\n"
        "    Node* current = head;\n"
        "    Node* next;\n"
        "    \n"
        "    while (current != NULL) {\n"
        "        next = current->next;\n"
        "        free(current);\n"
        "        current = next;\n"
        "    }\n"
        "}\n\n"
        "int main() {\n"
        "    Node* head = NULL;\n"
        "    \n"
        "    // リストに要素を追加\n"
        "    head = insert_at_end(head, 10);\n"
        "    head = insert_at_end(head, 20);\n"
        "    head = insert_at_beginning(head, 5);\n"
        "    head = insert_at_end(head, 30);\n"
        "    \n"
        "    // リストを表示\n"
        "    display_list(head);\n"
        "    \n"
        "    // メモリを解放\n"
        "    free_list(head);\n"
        "    \n"
        "    return 0;\n"
        "}", 1, "連結リスト,データ構造,動的メモリ,中級");
    
    // 上級レベルのコード生成パターン
    
    // 1. スレッド（上級）
    c_router_add_pattern_ex(router, "マルチスレッド", 0.9f, 
        "以下はPOSIXスレッドを使用したマルチスレッドプログラムの例です：", 1,
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <pthread.h>\n"
        "#include <unistd.h>\n\n"
        "#define NUM_THREADS 5\n\n"
        "// ミューテックス（スレッド間の排他制御用）\n"
        "pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;\n\n"
        "// スレッド間で共有するカウンタ\n"
        "int shared_counter = 0;\n\n"
        "// スレッド関数\n"
        "void* thread_function(void* arg) {\n"
        "    int thread_id = *((int*)arg);\n"
        "    \n"
        "    printf(\"スレッド %d: 開始\\n\", thread_id);\n"
        "    \n"
        "    // 各スレッドで5回ループ\n"
        "    for (int i = 0; i < 5; i++) {\n"
        "        // ミューテックスをロック（クリティカルセクション開始）\n"
        "        pthread_mutex_lock(&mutex);\n"
        "        \n"
        "        // 共有リソースへのアクセス\n"
        "        shared_counter++;\n"
        "        printf(\"スレッド %d: カウンタを %d に増加\\n\", thread_id, shared_counter);\n"
        "        \n"
        "        // ミューテックスをアンロック（クリティカルセクション終了）\n"
        "        pthread_mutex_unlock(&mutex);\n"
        "        \n"
        "        // スレッドの実行を少し遅らせる\n"
        "        usleep(rand() % 1000000); // 0-1秒のランダムな遅延\n"
        "    }\n"
        "    \n"
        "    printf(\"スレッド %d: 終了\\n\", thread_id);\n"
        "    pthread_exit(NULL);\n"
        "}\n\n"
        "int main() {\n"
        "    pthread_t threads[NUM_THREADS];\n"
        "    int thread_ids[NUM_THREADS];\n"
        "    \n"
        "    // 乱数の初期化\n"
        "    srand(time(NULL));\n"
        "    \n"
        "    printf(\"マルチスレッドプログラム開始\\n\");\n"
        "    \n"
        "    // スレッドの作成\n"
        "    for (int i = 0; i < NUM_THREADS; i++) {\n"
        "        thread_ids[i] = i;\n"
        "        int result = pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);\n"
        "        if (result) {\n"
        "            printf(\"エラー: pthread_create() が %d を返しました\\n\", result);\n"
        "            exit(-1);\n"
        "        }\n"
        "    }\n"
        "    \n"
        "    // すべてのスレッドの終了を待機\n"
        "    for (int i = 0; i < NUM_THREADS; i++) {\n"
        "        pthread_join(threads[i], NULL);\n"
        "    }\n"
        "    \n"
        "    // ミューテックスの破棄\n"
        "    pthread_mutex_destroy(&mutex);\n"
        "    \n"
        "    printf(\"すべてのスレッドが終了しました。最終カウンタ値: %d\\n\", shared_counter);\n"
        "    printf(\"マルチスレッドプログラム終了\\n\");\n"
        "    \n"
        "    return 0;\n"
        "}", 2, "スレッド,pthread,並行処理,ミューテックス,上級");
    
    // 2. ソケットプログラミング（上級）
    c_router_add_pattern_ex(router, "ソケット", 0.9f, 
        "以下はTCPソケットを使用した簡単なサーバープログラムの例です：", 1,
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <string.h>\n"
        "#include <unistd.h>\n"
        "#include <sys/types.h>\n"
        "#include <sys/socket.h>\n"
        "#include <netinet/in.h>\n"
        "#include <arpa/inet.h>\n\n"
        "#define PORT 8080\n"
        "#define BUFFER_SIZE 1024\n\n"
        "int main() {\n"
        "    int server_fd, new_socket;\n"
        "    struct sockaddr_in address;\n"
        "    int opt = 1;\n"
        "    int addrlen = sizeof(address);\n"
        "    char buffer[BUFFER_SIZE] = {0};\n"
        "    char* message = \"サーバーからのメッセージ: 接続成功!\\n\";\n"
        "    \n"
        "    // ソケットの作成\n"
        "    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {\n"
        "        perror(\"ソケット作成失敗\");\n"
        "        exit(EXIT_FAILURE);\n"
        "    }\n"
        "    \n"
        "    // ソケットオプションの設定\n"
        "    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {\n"
        "        perror(\"setsockopt\");\n"
        "        exit(EXIT_FAILURE);\n"
        "    }\n"
        "    \n"
        "    // アドレスの設定\n"
        "    address.sin_family = AF_INET;\n"
        "    address.sin_addr.s_addr = INADDR_ANY;\n"
        "    address.sin_port = htons(PORT);\n"
        "    \n"
        "    // ソケットをアドレスにバインド\n"
        "    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {\n"
        "        perror(\"バインド失敗\");\n"
        "        exit(EXIT_FAILURE);\n"
        "    }\n"
        "    \n"
        "    // 接続待機\n"
        "    if (listen(server_fd, 3) < 0) {\n"
        "        perror(\"listen\");\n"
        "        exit(EXIT_FAILURE);\n"
        "    }\n"
        "    \n"
        "    printf(\"サーバーがポート %d で起動しました。接続を待機中...\\n\", PORT);\n"
        "    \n"
        "    // クライアントからの接続を受け入れる\n"
        "    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {\n"
        "        perror(\"accept\");\n"
        "        exit(EXIT_FAILURE);\n"
        "    }\n"
        "    \n"
        "    // クライアントのIPアドレスを表示\n"
        "    char client_ip[INET_ADDRSTRLEN];\n"
        "    inet_ntop(AF_INET, &(address.sin_addr), client_ip, INET_ADDRSTRLEN);\n"
        "    printf(\"クライアント %s:%d が接続しました\\n\", client_ip, ntohs(address.sin_port));\n"
        "    \n"
        "    // クライアントからのメッセージを受信\n"
        "    int valread = read(new_socket, buffer, BUFFER_SIZE);\n"
        "    printf(\"クライアントからのメッセージ: %s\\n\", buffer);\n"
        "    \n"
        "    // クライアントにメッセージを送信\n"
        "    send(new_socket, message, strlen(message), 0);\n"
        "    printf(\"メッセージをクライアントに送信しました\\n\");\n"
        "    \n"
        "    // ソケットをクローズ\n"
        "    close(new_socket);\n"
        "    close(server_fd);\n"
        "    \n"
        "    printf(\"サーバーを終了します\\n\");\n"
        "    return 0;\n"
        "}", 2, "ソケット,ネットワーク,TCP/IP,上級,通信");
    
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