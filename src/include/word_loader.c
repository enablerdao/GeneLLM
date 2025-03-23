#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <math.h>
#include "word_loader.h"

// cURLのコールバック関数用の構造体
typedef struct {
    char* data;
    size_t size;
} MemoryStruct;

// cURLのコールバック関数
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    MemoryStruct* mem = (MemoryStruct*)userp;
    
    char* ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) {
        printf("メモリ不足エラー\n");
        return 0;
    }
    
    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;
    
    return realsize;
}

// 単語ベクトルをファイルから読み込む
int load_word_vectors(const char* filename, VectorDB* db, int max_words) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("ファイル %s を開けませんでした\n", filename);
        return 0;
    }
    
    char line[1024];
    char word[256];
    float vector[VECTOR_DIM];
    int word_count = 0;
    
    // ファイルの各行を読み込む
    while (fgets(line, sizeof(line), file)) {
        // 最大単語数に達したら終了
        if (max_words > 0 && word_count >= max_words) {
            break;
        }
        
        // 行から単語とベクトルを抽出
        char* token = strtok(line, " ");
        if (!token) continue;
        
        // 単語を取得
        strncpy(word, token, sizeof(word) - 1);
        word[sizeof(word) - 1] = '\0';
        
        // ベクトルを取得
        for (int i = 0; i < VECTOR_DIM; i++) {
            token = strtok(NULL, " ");
            if (!token) break;
            vector[i] = atof(token);
        }
        
        // ベクトルを正規化
        normalize_vector(vector);
        
        // ベクトルをデータベースに追加
        if (add_vector(db, vector, word_count)) {
            word_count++;
        } else {
            printf("ベクトルデータベースが満杯です\n");
            break;
        }
    }
    
    fclose(file);
    return word_count;
}

// 単語ベクトルをファイルに保存する
int save_word_vectors(const char* filename, VectorDB* db) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("ファイル %s を作成できませんでした\n", filename);
        return 0;
    }
    
    // データベース内の各ベクトルを保存
    for (int i = 0; i < db->size; i++) {
        // 単語リストから単語を取得（なければIDを使用）
        char word[256];
        FILE* word_file = fopen("data/japanese_words.txt", "r");
        if (word_file && db->entries[i].id < 5000) {  // 単語リストの範囲内のIDのみ
            // 対応する行を読み込む
            for (int j = 0; j <= db->entries[i].id; j++) {
                if (fgets(word, sizeof(word), word_file) == NULL) {
                    break;
                }
            }
            
            // 改行を削除
            size_t len = strlen(word);
            if (len > 0 && word[len-1] == '\n') {
                word[len-1] = '\0';
            }
            
            fclose(word_file);
            
            // 空の場合はIDを使用
            if (strlen(word) == 0) {
                sprintf(word, "word%d", db->entries[i].id);
            }
        } else {
            sprintf(word, "word%d", db->entries[i].id);
        }
        
        // 単語とベクトルを保存
        fprintf(file, "%s", word);
        
        // ベクトルを保存
        for (int j = 0; j < VECTOR_DIM; j++) {
            fprintf(file, " %.6f", db->entries[i].vector[j]);
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    return db->size;
}

// Webから単語リストを取得して単語ベクトルを生成
int fetch_word_vectors_from_web(const char* url, VectorDB* db, int max_words) {
    CURL* curl;
    CURLcode res;
    MemoryStruct chunk;
    int word_count = 0;
    
    // メモリ初期化
    chunk.data = malloc(1);
    chunk.size = 0;
    
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        
        res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // 取得したデータを行ごとに処理
            char* line = strtok(chunk.data, "\n");
            while (line && (max_words <= 0 || word_count < max_words)) {
                // 単語を取得（この例では行全体を単語として扱う）
                char* word = line;
                
                // 単語からベクトルを生成（この例ではランダムベクトルを生成）
                float vector[VECTOR_DIM];
                generate_random_vector(vector);
                normalize_vector(vector);
                
                // ベクトルをデータベースに追加
                if (add_vector(db, vector, word_count)) {
                    word_count++;
                } else {
                    printf("ベクトルデータベースが満杯です\n");
                    break;
                }
                
                line = strtok(NULL, "\n");
            }
        }
        
        curl_easy_cleanup(curl);
    }
    
    curl_global_cleanup();
    free(chunk.data);
    
    return word_count;
}

// 日本語Wikipediaから頻出単語を取得するURL
#define WIKIPEDIA_FREQUENT_WORDS_URL "https://dumps.wikimedia.org/jawiki/latest/jawiki-latest-all-titles-in-ns0.gz"

// 単語ベクトルを大量に生成する関数
int generate_large_word_vector_dataset(VectorDB* db, int target_size) {
    // 既存のベクトル数を確認
    int current_size = db->size;
    int remaining = target_size - current_size;
    
    if (remaining <= 0) {
        return current_size; // 既に目標数に達している
    }
    
    printf("大規模単語ベクトルデータセットを生成中...\n");
    printf("現在のベクトル数: %d, 目標: %d, 追加必要数: %d\n", 
           current_size, target_size, remaining);
    
    // 日本語単語リストファイルを読み込む
    FILE* file = fopen("data/japanese_words.txt", "r");
    if (!file) {
        printf("日本語単語リストファイルが見つかりません。ランダムベクトルを生成します。\n");
        // テスト用のベクトルを追加する関数を使用
        add_test_vectors_to_db(db, remaining);
    } else {
        char word[256];
        int added = 0;
        int word_id = current_size;
        
        printf("日本語単語リストから単語ベクトルを生成しています...\n");
        
        // ファイルから単語を読み込み、ベクトル化して追加
        while (fgets(word, sizeof(word), file) && added < remaining) {
            // 改行を削除
            size_t len = strlen(word);
            if (len > 0 && word[len-1] == '\n') {
                word[len-1] = '\0';
            }
            
            // 空行をスキップ
            if (strlen(word) == 0) {
                continue;
            }
            
            // 単語からベクトルを生成（ここでは単語の文字コードを使用して決定論的に生成）
            float vector[VECTOR_DIM];
            for (int i = 0; i < VECTOR_DIM; i++) {
                // 単語の各文字のコードを使用して決定論的にベクトル値を生成
                float val = 0.0f;
                for (size_t j = 0; j < strlen(word); j++) {
                    val += (float)(word[j] * (j+1) * (i+1)) / 10000.0f;
                }
                // -1.0から1.0の範囲に正規化
                vector[i] = fmodf(val, 2.0f) - 1.0f;
            }
            
            // ベクトルを正規化
            normalize_vector(vector);
            
            // ベクトルをデータベースに追加
            if (add_vector(db, vector, word_id++)) {
                added++;
                
                // 進捗表示
                if (added % 1000 == 0) {
                    printf("  %d / %d ベクトルを追加しました\n", added, remaining);
                }
            } else {
                printf("ベクトルデータベースが満杯です\n");
                break;
            }
        }
        
        fclose(file);
        
        // 単語リストが足りない場合は残りをランダムベクトルで補完
        if (added < remaining) {
            printf("単語リストが不足しています。残り %d ベクトルをランダム生成します。\n", 
                   remaining - added);
            add_test_vectors_to_db(db, remaining - added);
        }
    }
    
    printf("合計 %d ベクトルを追加しました（総数: %d）\n", db->size - current_size, db->size);
    return db->size;
}