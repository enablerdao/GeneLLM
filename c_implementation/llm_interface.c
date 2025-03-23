#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_RESPONSE_LENGTH 2048

/**
 * 外部LLMに質問を送信し、回答を取得する
 * 
 * @param query 質問文
 * @param response 回答を格納するバッファ
 * @param max_length responseバッファの最大長
 * @return 成功した場合は1、失敗した場合は0
 */
int query_external_llm(const char* query, char* response, size_t max_length) {
    char command[MAX_COMMAND_LENGTH];
    FILE* fp;
    
    // コマンドを構築
    snprintf(command, MAX_COMMAND_LENGTH, "python3 llm_query.py \"%s\"", query);
    
    // コマンドを実行し、出力を取得
    fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "外部LLMへの問い合わせに失敗しました\n");
        return 0;
    }
    
    // 出力を読み取る
    char* result = fgets(response, max_length, fp);
    
    // プロセスを閉じる
    int status = pclose(fp);
    
    // 結果を確認
    if (result == NULL || status != 0) {
        fprintf(stderr, "外部LLMからの応答の取得に失敗しました\n");
        return 0;
    }
    
    // 末尾の改行を削除
    size_t len = strlen(response);
    if (len > 0 && response[len-1] == '\n') {
        response[len-1] = '\0';
    }
    
    printf("外部LLMからの応答: %s\n", response);
    return 1;
}