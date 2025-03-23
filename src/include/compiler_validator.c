#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_OUTPUT_LENGTH 4096

// コンパイラによるコード検証結果
typedef struct {
    int success;            // コンパイル成功したか
    char output[MAX_OUTPUT_LENGTH]; // コンパイラの出力（警告・エラーなど）
    int warning_count;      // 警告の数
    int error_count;        // エラーの数
} CompilerValidationResult;

// コンパイラによるコード検証を実行
CompilerValidationResult validate_with_compiler(const char* source_code, const char* filename, const char* compiler_options) {
    CompilerValidationResult result = {0};
    
    // 一時ファイルにソースコードを書き込む
    FILE* temp_file = fopen(filename, "w");
    if (!temp_file) {
        strcpy(result.output, "一時ファイルを作成できませんでした");
        result.error_count = 1;
        return result;
    }
    
    fputs(source_code, temp_file);
    fclose(temp_file);
    
    // コンパイルコマンドを構築
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "gcc -fsyntax-only %s %s 2>&1", compiler_options, filename);
    
    // コマンドを実行して出力を取得
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        strcpy(result.output, "コンパイラを実行できませんでした");
        result.error_count = 1;
        return result;
    }
    
    // コンパイラの出力を読み取る
    char buffer[MAX_OUTPUT_LENGTH] = {0};
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, pipe);
    buffer[bytes_read] = '\0';
    
    // コマンドの終了ステータスを取得
    int status = pclose(pipe);
    result.success = (status == 0);
    
    // 出力をコピー
    strncpy(result.output, buffer, sizeof(result.output) - 1);
    result.output[sizeof(result.output) - 1] = '\0';
    
    // 警告とエラーの数をカウント
    char* ptr = result.output;
    while ((ptr = strstr(ptr, "warning:")) != NULL) {
        result.warning_count++;
        ptr++;
    }
    
    ptr = result.output;
    while ((ptr = strstr(ptr, "error:")) != NULL) {
        result.error_count++;
        ptr++;
    }
    
    return result;
}

// コンパイラによるコード最適化の提案を取得
char* get_compiler_suggestions(const char* source_code, const char* filename) {
    static char suggestions[MAX_OUTPUT_LENGTH];
    suggestions[0] = '\0';
    
    // 一時ファイルにソースコードを書き込む
    FILE* temp_file = fopen(filename, "w");
    if (!temp_file) {
        strcpy(suggestions, "一時ファイルを作成できませんでした");
        return suggestions;
    }
    
    fputs(source_code, temp_file);
    fclose(temp_file);
    
    // 様々な警告オプションを有効にしてコンパイル
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), 
             "gcc -fsyntax-only -Wall -Wextra -Wpedantic -Wconversion -Wshadow "
             "-Wundef -Wcast-qual -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 "
             "-Wwrite-strings -Wpointer-arith -Wformat=2 -Wformat-truncation "
             "-Wmissing-prototypes -Wredundant-decls -Walloca -Wvla "
             "-Wfloat-equal -Wdouble-promotion %s 2>&1", filename);
    
    // コマンドを実行して出力を取得
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        strcpy(suggestions, "コンパイラを実行できませんでした");
        return suggestions;
    }
    
    // コンパイラの出力を読み取る
    char buffer[MAX_OUTPUT_LENGTH] = {0};
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, pipe);
    buffer[bytes_read] = '\0';
    
    pclose(pipe);
    
    // 出力をコピー
    strncpy(suggestions, buffer, sizeof(suggestions) - 1);
    suggestions[sizeof(suggestions) - 1] = '\0';
    
    return suggestions;
}

// コードを実際にコンパイルして実行可能ファイルを生成
int compile_code(const char* source_code, const char* source_filename, const char* output_filename, const char* compiler_options, char* output) {
    // 一時ファイルにソースコードを書き込む
    FILE* temp_file = fopen(source_filename, "w");
    if (!temp_file) {
        sprintf(output, "一時ファイルを作成できませんでした: %s", source_filename);
        return 0;
    }
    
    fputs(source_code, temp_file);
    fclose(temp_file);
    
    // コンパイルコマンドを構築
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "gcc %s %s -o %s 2>&1", 
             compiler_options, source_filename, output_filename);
    
    // コマンドを実行して出力を取得
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        sprintf(output, "コンパイラを実行できませんでした");
        return 0;
    }
    
    // コンパイラの出力を読み取る
    char buffer[MAX_OUTPUT_LENGTH] = {0};
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, pipe);
    buffer[bytes_read] = '\0';
    
    // コマンドの終了ステータスを取得
    int status = pclose(pipe);
    int success = (status == 0);
    
    // 出力をコピー
    if (output) {
        strncpy(output, buffer, MAX_OUTPUT_LENGTH - 1);
        output[MAX_OUTPUT_LENGTH - 1] = '\0';
    }
    
    return success;
}

// コンパイルしたプログラムを実行してその出力を取得
char* run_compiled_program(const char* program_path, const char* arguments) {
    static char output[MAX_OUTPUT_LENGTH];
    output[0] = '\0';
    
    // 実行コマンドを構築
    char command[MAX_COMMAND_LENGTH];
    if (arguments && *arguments) {
        snprintf(command, sizeof(command), "%s %s 2>&1", program_path, arguments);
    } else {
        snprintf(command, sizeof(command), "%s 2>&1", program_path);
    }
    
    // コマンドを実行して出力を取得
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        strcpy(output, "プログラムを実行できませんでした");
        return output;
    }
    
    // プログラムの出力を読み取る
    char buffer[MAX_OUTPUT_LENGTH] = {0};
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, pipe);
    buffer[bytes_read] = '\0';
    
    pclose(pipe);
    
    // 出力をコピー
    strncpy(output, buffer, sizeof(output) - 1);
    output[sizeof(output) - 1] = '\0';
    
    return output;
}

// コードを静的解析ツールで検証（例：cppcheck）
char* analyze_with_static_tool(const char* source_code, const char* filename) {
    static char output[MAX_OUTPUT_LENGTH];
    output[0] = '\0';
    
    // cppcheckがインストールされているか確認
    FILE* check_pipe = popen("which cppcheck 2>/dev/null", "r");
    if (!check_pipe || fgetc(check_pipe) == EOF) {
        if (check_pipe) pclose(check_pipe);
        strcpy(output, "cppcheckがインストールされていません");
        return output;
    }
    pclose(check_pipe);
    
    // 一時ファイルにソースコードを書き込む
    FILE* temp_file = fopen(filename, "w");
    if (!temp_file) {
        strcpy(output, "一時ファイルを作成できませんでした");
        return output;
    }
    
    fputs(source_code, temp_file);
    fclose(temp_file);
    
    // cppcheckコマンドを構築
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), 
             "cppcheck --enable=all --inconclusive --std=c99 %s 2>&1", filename);
    
    // コマンドを実行して出力を取得
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        strcpy(output, "cppcheckを実行できませんでした");
        return output;
    }
    
    // cppcheckの出力を読み取る
    char buffer[MAX_OUTPUT_LENGTH] = {0};
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, pipe);
    buffer[bytes_read] = '\0';
    
    pclose(pipe);
    
    // 出力をコピー
    strncpy(output, buffer, sizeof(output) - 1);
    output[sizeof(output) - 1] = '\0';
    
    return output;
}

// テスト用のメイン関数
#ifdef TEST_COMPILER_VALIDATOR
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("使用法: %s <ソースファイル> [コンパイラオプション]\n", argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    const char* compiler_options = (argc >= 3) ? argv[2] : "-Wall -Wextra -std=c99";
    
    // ファイルからソースコードを読み込む
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("ファイルを開けませんでした: %s\n", filename);
        return 1;
    }
    
    // ファイルサイズを取得
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // ソースコードを読み込むためのバッファを確保
    char* source_code = (char*)malloc(file_size + 1);
    if (!source_code) {
        printf("メモリを確保できませんでした\n");
        fclose(file);
        return 1;
    }
    
    // ファイルを読み込む
    size_t bytes_read = fread(source_code, 1, file_size, file);
    source_code[bytes_read] = '\0';
    fclose(file);
    
    printf("=== コンパイラによる検証 ===\n");
    CompilerValidationResult result = validate_with_compiler(source_code, "temp_validation.c", compiler_options);
    
    printf("コンパイル %s\n", result.success ? "成功" : "失敗");
    printf("警告: %d, エラー: %d\n", result.warning_count, result.error_count);
    
    if (strlen(result.output) > 0) {
        printf("\nコンパイラ出力:\n%s\n", result.output);
    }
    
    printf("\n=== コンパイラの提案 ===\n");
    char* suggestions = get_compiler_suggestions(source_code, "temp_suggestions.c");
    if (strlen(suggestions) > 0) {
        printf("%s\n", suggestions);
    } else {
        printf("提案はありません（コードは最適です）\n");
    }
    
    printf("\n=== 静的解析ツールの結果 ===\n");
    char* static_analysis = analyze_with_static_tool(source_code, "temp_static.c");
    printf("%s\n", static_analysis);
    
    // コンパイルして実行
    printf("\n=== コンパイルと実行 ===\n");
    char compile_output[MAX_OUTPUT_LENGTH];
    int compile_success = compile_code(source_code, "temp_compile.c", "temp_program", compiler_options, compile_output);
    
    if (compile_success) {
        printf("コンパイル成功\n");
        
        // 実行
        printf("\n実行結果:\n");
        char* run_output = run_compiled_program("./temp_program", "");
        printf("%s\n", run_output);
        
        // 一時ファイルを削除
        unlink("./temp_program");
    } else {
        printf("コンパイル失敗:\n%s\n", compile_output);
    }
    
    // 一時ファイルを削除
    unlink("temp_validation.c");
    unlink("temp_suggestions.c");
    unlink("temp_static.c");
    unlink("temp_compile.c");
    
    // メモリを解放
    free(source_code);
    
    return 0;
}
#endif