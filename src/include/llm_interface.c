// 外部LLMインターフェース（ダミー実装）
// 実際のLLMとの連携が必要な場合は、このファイルを適切に実装してください

#include <stdio.h>
#include <string.h>

// LLMに問い合わせる関数
int query_llm(const char* unused_input, char* output, int max_output_len) {
    // 未使用パラメータの警告を抑制
    (void)unused_input;
    
    // ダミー実装：実際のLLMとの連携は行わず、固定の応答を返す
    const char* dummy_response = "この機能は現在実装されていません。";

    // 出力バッファに応答をコピー
    strncpy(output, dummy_response, max_output_len - 1);
    output[max_output_len - 1] = '\0';

    return 1; // 成功
}