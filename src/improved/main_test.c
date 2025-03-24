#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/vector_db.h"

int main() {
    printf("GeneLLM テストプログラム\n");
    
    // グローバルベクトルデータベースの初期化
    init_global_vector_db();
    
    printf("グローバルベクトルデータベースのサイズ: %d\n", get_global_vector_db_size());
    
    return 0;
}
