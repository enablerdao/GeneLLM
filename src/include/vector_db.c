#include "vector_db.h"
#include "word_loader.h"

// グローバルなベクトルデータベース
VectorDB global_vector_db;

// ベクトルデータベースを初期化
void init_global_vector_db() {
    init_vector_db(&global_vector_db);
    
    // データファイルのパス
    const char* vector_file = "data/word_vectors.dat";
    
    // ファイルからベクトルを読み込む
    FILE* file = fopen(vector_file, "r");
    if (file) {
        fclose(file);
        printf("単語ベクトルファイルを読み込んでいます...\n");
        int loaded = load_word_vectors(vector_file, &global_vector_db, 0);
        printf("%d 個の単語ベクトルを読み込みました\n", loaded);
    } else {
        // ファイルが存在しない場合は大量のベクトルを生成
        printf("単語ベクトルファイルが見つかりません。新しく生成します...\n");
        generate_large_word_vector_dataset(&global_vector_db, 20000);
        
        // 生成したベクトルを保存
        printf("生成した単語ベクトルを保存しています...\n");
        save_word_vectors(vector_file, &global_vector_db);
    }
}

// 現在のベクトルデータベースのサイズを取得
int get_global_vector_db_size() {
    return global_vector_db.size;
}