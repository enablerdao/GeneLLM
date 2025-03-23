#include "vector_db.h"
#include "word_loader.h"
#include <sys/stat.h>
#include <time.h>

// グローバルなベクトルデータベース
VectorDB global_vector_db;

// ベクトルデータベースを初期化
void init_global_vector_db() {
    init_vector_db(&global_vector_db);
    
    // データファイルのパス
    const char* vector_file = "data/word_vectors.dat";
    const char* japanese_words_file = "data/japanese_words.txt";
    
    // 日本語単語ファイルの最終更新時刻を取得
    struct stat japanese_words_stat;
    time_t japanese_words_mtime = 0;
    if (stat(japanese_words_file, &japanese_words_stat) == 0) {
        japanese_words_mtime = japanese_words_stat.st_mtime;
    }
    
    // ベクトルファイルの最終更新時刻を取得
    struct stat vector_file_stat;
    time_t vector_file_mtime = 0;
    if (stat(vector_file, &vector_file_stat) == 0) {
        vector_file_mtime = vector_file_stat.st_mtime;
    }
    
    // ファイルからベクトルを読み込む
    FILE* file = fopen(vector_file, "r");
    if (file && (japanese_words_mtime <= vector_file_mtime)) {
        // ベクトルファイルが存在し、日本語単語ファイルより新しい場合は読み込む
        fclose(file);
        printf("単語ベクトルファイルを読み込んでいます...\n");
        int loaded = load_word_vectors(vector_file, &global_vector_db, 0);
        printf("%d 個の単語ベクトルを読み込みました\n", loaded);
    } else {
        // ファイルが存在しないか、日本語単語ファイルが更新されている場合は再生成
        if (!file) {
            printf("単語ベクトルファイルが見つかりません。新しく生成します...\n");
        } else {
            fclose(file);
            printf("日本語単語ファイルが更新されています。ベクトルを再生成します...\n");
        }
        
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