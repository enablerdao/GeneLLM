#ifndef COMPILER_VALIDATOR_H
#define COMPILER_VALIDATOR_H

#include <stdio.h>

// コンパイラによるコード検証結果
typedef struct {
    int success;            // コンパイル成功したか
    char output[4096];      // コンパイラの出力（警告・エラーなど）
    int warning_count;      // 警告の数
    int error_count;        // エラーの数
} CompilerValidationResult;

// コンパイラによるコード検証を実行
CompilerValidationResult validate_with_compiler(const char* source_code, const char* filename, const char* compiler_options);

// コンパイラによるコード最適化の提案を取得
char* get_compiler_suggestions(const char* source_code, const char* filename);

// コードを実際にコンパイルして実行可能ファイルを生成
int compile_code(const char* source_code, const char* source_filename, const char* output_filename, const char* compiler_options, char* output);

// コンパイルしたプログラムを実行してその出力を取得
char* run_compiled_program(const char* program_path, const char* arguments);

// コードを静的解析ツールで検証（例：cppcheck）
char* analyze_with_static_tool(const char* source_code, const char* filename);

#endif // COMPILER_VALIDATOR_H