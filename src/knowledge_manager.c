#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>
#include "knowledge_manager.h"

#define MAX_PATH_LENGTH 1024
#define MAX_LINE_LENGTH 8192
#define MAX_CONTENT_LENGTH 1048576  // 1MB
#define MAX_THREADS 8

// グローバル変数
char workspace_dir[MAX_PATH_LENGTH] = "/workspace";
char knowledge_dir[MAX_PATH_LENGTH];
char docs_dir[MAX_PATH_LENGTH];
char answers_file[MAX_PATH_LENGTH];
char temp_dir[MAX_PATH_LENGTH];
char output_dir[MAX_PATH_LENGTH];
char word_list_file[MAX_PATH_LENGTH];
char vectors_file[MAX_PATH_LENGTH];

// ミューテックス
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

// 知識ベース構造体
typedef struct {
    char question[MAX_LINE_LENGTH];
    char answer[MAX_CONTENT_LENGTH];
} QAPair;

typedef struct {
    QAPair *pairs;
    int count;
    int capacity;
} KnowledgeBase;

// スレッド処理用構造体
typedef struct {
    char filepath[MAX_PATH_LENGTH];
    int thread_id;
    int file_index;
    int total_files;
} ThreadData;

// 関数プロトタイプ
void init_paths();
int update_knowledge();
int tokenize_knowledge();
void *process_file_for_qa(void *arg);
void *process_file_for_tokenize(void *arg);
int extract_qa_from_file(const char *filepath, KnowledgeBase *kb);
int extract_qa_from_text(const char *filepath, KnowledgeBase *kb);
int extract_qa_from_markdown(const char *filepath, KnowledgeBase *kb);
int save_knowledge_base(const KnowledgeBase *kb, const char *filepath);
int load_knowledge_base(KnowledgeBase *kb, const char *filepath);
void free_knowledge_base(KnowledgeBase *kb);
int tokenize_file(const char *filepath, const char *output_filepath);
int update_word_vectors(const char *tokenized_dir);
int is_file_changed(const char *filepath, const char *processed_files);
char *get_file_extension(const char *filepath);
int create_directory(const char *path);
int file_exists(const char *filepath);
int is_file_empty(const char *filepath);
int is_directory(const char *path);
char *read_file_content(const char *filepath);
int write_file_content(const char *filepath, const char *content);
void append_to_file(const char *filepath, const char *content);
char *get_relative_path(const char *base_path, const char *full_path);

// 初期化関数
void init_paths() {
    snprintf(knowledge_dir, MAX_PATH_LENGTH, "%s/knowledge", workspace_dir);
    snprintf(docs_dir, MAX_PATH_LENGTH, "%s/docs", workspace_dir);
    snprintf(answers_file, MAX_PATH_LENGTH, "%s/base/answers.txt", knowledge_dir);
    snprintf(temp_dir, MAX_PATH_LENGTH, "/tmp/genellm_knowledge");
    snprintf(output_dir, MAX_PATH_LENGTH, "%s/data/tokenized", workspace_dir);
    snprintf(word_list_file, MAX_PATH_LENGTH, "%s/data/word_list.txt", workspace_dir);
    snprintf(vectors_file, MAX_PATH_LENGTH, "%s/data/word_vectors.dat", workspace_dir);
    
    // 必要なディレクトリを作成
    create_directory(temp_dir);
    create_directory(output_dir);
    create_directory("/tmp/genellm_vectors");
}

// メイン関数
int main(int argc, char *argv[]) {
    // 初期化
    init_paths();
    
    if (argc < 2) {
        printf("使用方法: %s [update|tokenize|all]\n", argv[0]);
        return 1;
    }
    
    // コマンドライン引数に基づいて処理を実行
    if (strcmp(argv[1], "update") == 0) {
        return update_knowledge();
    } else if (strcmp(argv[1], "tokenize") == 0) {
        return tokenize_knowledge();
    } else if (strcmp(argv[1], "all") == 0) {
        int result = update_knowledge();
        if (result != 0) return result;
        return tokenize_knowledge();
    } else {
        printf("不明なコマンド: %s\n", argv[1]);
        printf("使用方法: %s [update|tokenize|all]\n", argv[0]);
        return 1;
    }
}

// 知識ベースの更新
int update_knowledge() {
    printf("知識ベースを更新しています...\n");
    
    // 処理済みファイルのリストを保存するファイル
    char processed_files[MAX_PATH_LENGTH];
    snprintf(processed_files, MAX_PATH_LENGTH, "%s/processed_files.txt", temp_dir);
    
    // 既存の回答ファイルをバックアップ
    char backup_file[MAX_PATH_LENGTH];
    snprintf(backup_file, MAX_PATH_LENGTH, "%s.bak", answers_file);
    
    if (file_exists(answers_file)) {
        char command[MAX_PATH_LENGTH * 2];
        snprintf(command, MAX_PATH_LENGTH * 2, "cp %s %s", answers_file, backup_file);
        system(command);
    }
    
    // 変更されたファイルを検出
    printf("変更されたファイルを検出しています...\n");
    
    // ファイルリストを取得
    char file_list[MAX_PATH_LENGTH];
    snprintf(file_list, MAX_PATH_LENGTH, "%s/file_list.txt", temp_dir);
    
    char command[MAX_PATH_LENGTH * 3];
    snprintf(command, MAX_PATH_LENGTH * 3, 
             "find %s %s -type f \\( -name \"*.txt\" -o -name \"*.md\" \\) > %s",
             knowledge_dir, docs_dir, file_list);
    system(command);
    
    // ファイルリストを読み込む
    FILE *fp = fopen(file_list, "r");
    if (!fp) {
        printf("エラー: ファイルリストを開けません: %s\n", file_list);
        return 1;
    }
    
    // 変更されたファイルの数をカウント
    int changed_files_count = 0;
    char filepath[MAX_PATH_LENGTH];
    
    while (fgets(filepath, MAX_PATH_LENGTH, fp)) {
        // 改行を削除
        filepath[strcspn(filepath, "\n")] = 0;
        
        // answers.txtは除外
        if (strcmp(filepath, answers_file) == 0) {
            continue;
        }
        
        // ファイルが変更されているか確認
        if (is_file_changed(filepath, processed_files)) {
            changed_files_count++;
        }
    }
    
    fclose(fp);
    
    if (changed_files_count == 0) {
        printf("変更されたファイルはありません。処理を終了します。\n");
        return 0;
    }
    
    printf("変更されたファイル数: %d\n", changed_files_count);
    
    // 変更されたファイルを処理
    printf("変更されたファイルを処理しています...\n");
    
    // スレッド処理の準備
    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];
    int thread_count = 0;
    int file_index = 0;
    
    // 知識ベースの初期化
    KnowledgeBase kb;
    kb.count = 0;
    kb.capacity = 1000;
    kb.pairs = (QAPair *)malloc(kb.capacity * sizeof(QAPair));
    
    if (!kb.pairs) {
        printf("エラー: メモリ割り当てに失敗しました\n");
        return 1;
    }
    
    // 既存の回答ファイルを読み込む
    if (file_exists(answers_file)) {
        load_knowledge_base(&kb, answers_file);
    }
    
    // ファイルリストを再度読み込む
    fp = fopen(file_list, "r");
    if (!fp) {
        printf("エラー: ファイルリストを開けません: %s\n", file_list);
        free_knowledge_base(&kb);
        return 1;
    }
    
    // 変更されたファイルを並列処理
    while (fgets(filepath, MAX_PATH_LENGTH, fp)) {
        // 改行を削除
        filepath[strcspn(filepath, "\n")] = 0;
        
        // answers.txtは除外
        if (strcmp(filepath, answers_file) == 0) {
            continue;
        }
        
        // ファイルが変更されているか確認
        if (is_file_changed(filepath, processed_files)) {
            // スレッドデータを設定
            strcpy(thread_data[thread_count].filepath, filepath);
            thread_data[thread_count].thread_id = thread_count;
            thread_data[thread_count].file_index = file_index++;
            thread_data[thread_count].total_files = changed_files_count;
            
            // スレッドを作成
            if (pthread_create(&threads[thread_count], NULL, process_file_for_qa, &thread_data[thread_count]) != 0) {
                printf("エラー: スレッドの作成に失敗しました\n");
                free_knowledge_base(&kb);
                fclose(fp);
                return 1;
            }
            
            thread_count++;
            
            // スレッド数が最大に達したら待機
            if (thread_count >= MAX_THREADS) {
                for (int i = 0; i < thread_count; i++) {
                    void *result;
                    pthread_join(threads[i], &result);
                    if (result) {
                        // 結果を知識ベースに追加
                        KnowledgeBase *thread_kb = (KnowledgeBase *)result;
                        for (int j = 0; j < thread_kb->count; j++) {
                            // 容量を確認し、必要に応じて拡張
                            if (kb.count >= kb.capacity) {
                                kb.capacity *= 2;
                                kb.pairs = (QAPair *)realloc(kb.pairs, kb.capacity * sizeof(QAPair));
                                if (!kb.pairs) {
                                    printf("エラー: メモリ再割り当てに失敗しました\n");
                                    free_knowledge_base(thread_kb);
                                    free(result);
                                    fclose(fp);
                                    return 1;
                                }
                            }
                            
                            // 質問と回答をコピー
                            strcpy(kb.pairs[kb.count].question, thread_kb->pairs[j].question);
                            strcpy(kb.pairs[kb.count].answer, thread_kb->pairs[j].answer);
                            kb.count++;
                        }
                        
                        free_knowledge_base(thread_kb);
                        free(result);
                    }
                }
                thread_count = 0;
            }
            
            // 処理済みファイルとして記録
            char processed_entry[MAX_PATH_LENGTH + 20];
            snprintf(processed_entry, MAX_PATH_LENGTH + 20, "%s|%ld\n", filepath, time(NULL));
            append_to_file(processed_files, processed_entry);
        }
    }
    
    fclose(fp);
    
    // 残りのスレッドを待機
    for (int i = 0; i < thread_count; i++) {
        void *result;
        pthread_join(threads[i], &result);
        if (result) {
            // 結果を知識ベースに追加
            KnowledgeBase *thread_kb = (KnowledgeBase *)result;
            for (int j = 0; j < thread_kb->count; j++) {
                // 容量を確認し、必要に応じて拡張
                if (kb.count >= kb.capacity) {
                    kb.capacity *= 2;
                    kb.pairs = (QAPair *)realloc(kb.pairs, kb.capacity * sizeof(QAPair));
                    if (!kb.pairs) {
                        printf("エラー: メモリ再割り当てに失敗しました\n");
                        free_knowledge_base(thread_kb);
                        free(result);
                        return 1;
                    }
                }
                
                // 質問と回答をコピー
                strcpy(kb.pairs[kb.count].question, thread_kb->pairs[j].question);
                strcpy(kb.pairs[kb.count].answer, thread_kb->pairs[j].answer);
                kb.count++;
            }
            
            free_knowledge_base(thread_kb);
            free(result);
        }
    }
    
    // 重複を削除して回答ファイルを更新
    printf("重複を削除して回答ファイルを更新しています...\n");
    
    // 重複を削除
    int unique_count = 0;
    for (int i = 0; i < kb.count; i++) {
        int is_duplicate = 0;
        for (int j = 0; j < unique_count; j++) {
            if (strcmp(kb.pairs[i].question, kb.pairs[j].question) == 0) {
                is_duplicate = 1;
                break;
            }
        }
        
        if (!is_duplicate) {
            if (i != unique_count) {
                strcpy(kb.pairs[unique_count].question, kb.pairs[i].question);
                strcpy(kb.pairs[unique_count].answer, kb.pairs[i].answer);
            }
            unique_count++;
        }
    }
    
    // 知識ベースのサイズを更新
    int old_count = 0;
    if (file_exists(answers_file)) {
        FILE *fp = fopen(answers_file, "r");
        if (fp) {
            char line[MAX_LINE_LENGTH];
            while (fgets(line, MAX_LINE_LENGTH, fp)) {
                old_count++;
            }
            fclose(fp);
        }
    }
    
    kb.count = unique_count;
    
    // 回答ファイルを保存
    if (save_knowledge_base(&kb, answers_file) != 0) {
        printf("エラー: 回答ファイルの保存に失敗しました\n");
        free_knowledge_base(&kb);
        return 1;
    }
    
    // 新しい回答の数を計算
    int added_count = unique_count - old_count;
    printf("%d 件の新しい知識が追加されました。\n", added_count);
    
    // 知識ベースを解放
    free_knowledge_base(&kb);
    
    printf("知識の更新が完了しました。\n");
    return 0;
}

// トークナイズ処理
int tokenize_knowledge() {
    printf("ナレッジとドキュメントをトークナイズしています...\n");
    
    // ファイルリストを取得
    char file_list[MAX_PATH_LENGTH];
    snprintf(file_list, MAX_PATH_LENGTH, "%s/tokenize_files.txt", temp_dir);
    
    char command[MAX_PATH_LENGTH * 3];
    snprintf(command, MAX_PATH_LENGTH * 3, 
             "find %s %s -type f \\( -name \"*.txt\" -o -name \"*.md\" \\) | grep -v \"answers.txt\" | grep -v \"answers_new.txt\" | grep -v \"answers_additional.txt\" > %s",
             knowledge_dir, docs_dir, file_list);
    system(command);
    
    // ファイル数を取得
    FILE *fp = fopen(file_list, "r");
    if (!fp) {
        printf("エラー: ファイルリストを開けません: %s\n", file_list);
        return 1;
    }
    
    int file_count = 0;
    char filepath[MAX_PATH_LENGTH];
    
    while (fgets(filepath, MAX_PATH_LENGTH, fp)) {
        file_count++;
    }
    
    rewind(fp);
    
    printf("合計 %d 個のファイルを処理します。\n", file_count);
    
    // スレッド処理の準備
    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];
    int thread_count = 0;
    int file_index = 0;
    int tokenized_count = 0;
    
    // ファイルを並列処理
    while (fgets(filepath, MAX_PATH_LENGTH, fp)) {
        // 改行を削除
        filepath[strcspn(filepath, "\n")] = 0;
        
        // スレッドデータを設定
        strcpy(thread_data[thread_count].filepath, filepath);
        thread_data[thread_count].thread_id = thread_count;
        thread_data[thread_count].file_index = file_index++;
        thread_data[thread_count].total_files = file_count;
        
        // スレッドを作成
        if (pthread_create(&threads[thread_count], NULL, process_file_for_tokenize, &thread_data[thread_count]) != 0) {
            printf("エラー: スレッドの作成に失敗しました\n");
            fclose(fp);
            return 1;
        }
        
        thread_count++;
        
        // スレッド数が最大に達したら待機
        if (thread_count >= MAX_THREADS) {
            for (int i = 0; i < thread_count; i++) {
                void *result;
                pthread_join(threads[i], &result);
                if (result && *(int *)result == 1) {
                    tokenized_count++;
                }
                free(result);
            }
            thread_count = 0;
        }
    }
    
    fclose(fp);
    
    // 残りのスレッドを待機
    for (int i = 0; i < thread_count; i++) {
        void *result;
        pthread_join(threads[i], &result);
        if (result && *(int *)result == 1) {
            tokenized_count++;
        }
        free(result);
    }
    
    printf("処理完了: %d/%d ファイルがトークナイズされました。\n", tokenized_count, file_count);
    printf("トークナイズされたファイルは %s に保存されました。\n", output_dir);
    
    // 単語ベクトルの更新
    printf("単語ベクトルを更新しています...\n");
    update_word_vectors(output_dir);
    
    printf("ナレッジとドキュメントのトークナイズが完了しました。\n");
    return 0;
}

// ファイルからQAペアを抽出するスレッド関数
void *process_file_for_qa(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    char *filepath = data->filepath;
    int file_index = data->file_index;
    int total_files = data->total_files;
    
    printf("[%d/%d] ファイル %s を処理中...\n", file_index + 1, total_files, filepath);
    
    // 知識ベースの初期化
    KnowledgeBase *kb = (KnowledgeBase *)malloc(sizeof(KnowledgeBase));
    if (!kb) {
        printf("エラー: メモリ割り当てに失敗しました\n");
        return NULL;
    }
    
    kb->count = 0;
    kb->capacity = 100;
    kb->pairs = (QAPair *)malloc(kb->capacity * sizeof(QAPair));
    
    if (!kb->pairs) {
        printf("エラー: メモリ割り当てに失敗しました\n");
        free(kb);
        return NULL;
    }
    
    // ファイルの拡張子を取得
    char *ext = get_file_extension(filepath);
    
    if (ext && strcmp(ext, "txt") == 0) {
        // テキストファイルの処理
        extract_qa_from_text(filepath, kb);
    } else if (ext && strcmp(ext, "md") == 0) {
        // マークダウンファイルの処理
        extract_qa_from_markdown(filepath, kb);
    }
    
    return kb;
}

// ファイルをトークナイズするスレッド関数
void *process_file_for_tokenize(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    char *filepath = data->filepath;
    int file_index = data->file_index;
    int total_files = data->total_files;
    
    printf("[%d/%d] ファイル %s を処理中...\n", file_index + 1, total_files, filepath);
    
    // 出力ファイル名を生成
    char *rel_path = get_relative_path(workspace_dir, filepath);
    char output_filepath[MAX_PATH_LENGTH];
    
    // パス区切り文字を置換
    char *p = rel_path;
    while (*p) {
        if (*p == '/') *p = '_';
        p++;
    }
    
    snprintf(output_filepath, MAX_PATH_LENGTH, "%s/%s.tokens", output_dir, rel_path);
    free(rel_path);
    
    // トークナイズ処理
    int *result = (int *)malloc(sizeof(int));
    *result = tokenize_file(filepath, output_filepath);
    
    if (*result) {
        printf("  トークナイズ成功: %s\n", output_filepath);
    } else {
        printf("  警告: トークナイズ結果が空です: %s\n", filepath);
    }
    
    return result;
}

// テキストファイルからQAペアを抽出
int extract_qa_from_text(const char *filepath, KnowledgeBase *kb) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        printf("エラー: ファイルを開けません: %s\n", filepath);
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    char question[MAX_LINE_LENGTH] = "";
    char answer[MAX_CONTENT_LENGTH] = "";
    
    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        // 改行を削除
        line[strcspn(line, "\n")] = 0;
        
        // 質問|回答形式を検出
        char *separator = strchr(line, '|');
        if (separator && line[0] != '#') {
            *separator = '\0';
            
            // 質問と回答を取得
            strcpy(question, line);
            strcpy(answer, separator + 1);
            
            // 知識ベースに追加
            if (strlen(question) > 0 && strlen(answer) > 0) {
                // 容量を確認し、必要に応じて拡張
                if (kb->count >= kb->capacity) {
                    kb->capacity *= 2;
                    kb->pairs = (QAPair *)realloc(kb->pairs, kb->capacity * sizeof(QAPair));
                    if (!kb->pairs) {
                        printf("エラー: メモリ再割り当てに失敗しました\n");
                        fclose(fp);
                        return 0;
                    }
                }
                
                // 質問と回答をコピー
                strcpy(kb->pairs[kb->count].question, question);
                strcpy(kb->pairs[kb->count].answer, answer);
                kb->count++;
            }
        }
        // # 質問形式を検出
        else if (line[0] == '#' && line[1] == ' ') {
            // 前の質問と回答があれば追加
            if (strlen(question) > 0 && strlen(answer) > 0) {
                // 容量を確認し、必要に応じて拡張
                if (kb->count >= kb->capacity) {
                    kb->capacity *= 2;
                    kb->pairs = (QAPair *)realloc(kb->pairs, kb->capacity * sizeof(QAPair));
                    if (!kb->pairs) {
                        printf("エラー: メモリ再割り当てに失敗しました\n");
                        fclose(fp);
                        return 0;
                    }
                }
                
                // 質問と回答をコピー
                strcpy(kb->pairs[kb->count].question, question);
                strcpy(kb->pairs[kb->count].answer, answer);
                kb->count++;
            }
            
            // 新しい質問を設定
            strcpy(question, line + 2);
            answer[0] = '\0';
        }
        // 回答の続き
        else if (strlen(question) > 0 && line[0] != '#') {
            // 回答に行を追加
            if (strlen(answer) > 0) {
                strcat(answer, "\n");
            }
            strcat(answer, line);
        }
    }
    
    // 最後の質問と回答があれば追加
    if (strlen(question) > 0 && strlen(answer) > 0) {
        // 容量を確認し、必要に応じて拡張
        if (kb->count >= kb->capacity) {
            kb->capacity *= 2;
            kb->pairs = (QAPair *)realloc(kb->pairs, kb->capacity * sizeof(QAPair));
            if (!kb->pairs) {
                printf("エラー: メモリ再割り当てに失敗しました\n");
                fclose(fp);
                return 0;
            }
        }
        
        // 質問と回答をコピー
        strcpy(kb->pairs[kb->count].question, question);
        strcpy(kb->pairs[kb->count].answer, answer);
        kb->count++;
    }
    
    fclose(fp);
    return 1;
}

// マークダウンファイルからQAペアを抽出
int extract_qa_from_markdown(const char *filepath, KnowledgeBase *kb) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        printf("エラー: ファイルを開けません: %s\n", filepath);
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    char question[MAX_LINE_LENGTH] = "";
    char answer[MAX_CONTENT_LENGTH] = "";
    
    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        // 改行を削除
        line[strcspn(line, "\n")] = 0;
        
        // ## 見出しを検出
        if (line[0] == '#' && line[1] == '#' && line[2] == ' ') {
            // 前の質問と回答があれば追加
            if (strlen(question) > 0 && strlen(answer) > 0) {
                // 容量を確認し、必要に応じて拡張
                if (kb->count >= kb->capacity) {
                    kb->capacity *= 2;
                    kb->pairs = (QAPair *)realloc(kb->pairs, kb->capacity * sizeof(QAPair));
                    if (!kb->pairs) {
                        printf("エラー: メモリ再割り当てに失敗しました\n");
                        fclose(fp);
                        return 0;
                    }
                }
                
                // 質問と回答をコピー
                strcpy(kb->pairs[kb->count].question, question);
                strcpy(kb->pairs[kb->count].answer, answer);
                kb->count++;
            }
            
            // 新しい質問を設定
            strcpy(question, line + 3);
            answer[0] = '\0';
        }
        // 回答の続き
        else if (strlen(question) > 0 && line[0] != '#') {
            // 回答に行を追加
            if (strlen(answer) > 0) {
                strcat(answer, "\n");
            }
            strcat(answer, line);
        }
    }
    
    // 最後の質問と回答があれば追加
    if (strlen(question) > 0 && strlen(answer) > 0) {
        // 容量を確認し、必要に応じて拡張
        if (kb->count >= kb->capacity) {
            kb->capacity *= 2;
            kb->pairs = (QAPair *)realloc(kb->pairs, kb->capacity * sizeof(QAPair));
            if (!kb->pairs) {
                printf("エラー: メモリ再割り当てに失敗しました\n");
                fclose(fp);
                return 0;
            }
        }
        
        // 質問と回答をコピー
        strcpy(kb->pairs[kb->count].question, question);
        strcpy(kb->pairs[kb->count].answer, answer);
        kb->count++;
    }
    
    fclose(fp);
    return 1;
}

// 知識ベースを保存
int save_knowledge_base(const KnowledgeBase *kb, const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        printf("エラー: ファイルを開けません: %s\n", filepath);
        return 0;
    }
    
    for (int i = 0; i < kb->count; i++) {
        fprintf(fp, "%s|%s\n", kb->pairs[i].question, kb->pairs[i].answer);
    }
    
    fclose(fp);
    return 1;
}

// 知識ベースを読み込み
int load_knowledge_base(KnowledgeBase *kb, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        printf("エラー: ファイルを開けません: %s\n", filepath);
        return 0;
    }
    
    char line[MAX_LINE_LENGTH + MAX_CONTENT_LENGTH];
    
    while (fgets(line, MAX_LINE_LENGTH + MAX_CONTENT_LENGTH, fp)) {
        // 改行を削除
        line[strcspn(line, "\n")] = 0;
        
        // 質問|回答形式を検出
        char *separator = strchr(line, '|');
        if (separator) {
            *separator = '\0';
            
            // 容量を確認し、必要に応じて拡張
            if (kb->count >= kb->capacity) {
                kb->capacity *= 2;
                kb->pairs = (QAPair *)realloc(kb->pairs, kb->capacity * sizeof(QAPair));
                if (!kb->pairs) {
                    printf("エラー: メモリ再割り当てに失敗しました\n");
                    fclose(fp);
                    return 0;
                }
            }
            
            // 質問と回答をコピー
            strcpy(kb->pairs[kb->count].question, line);
            strcpy(kb->pairs[kb->count].answer, separator + 1);
            kb->count++;
        }
    }
    
    fclose(fp);
    return 1;
}

// 知識ベースを解放
void free_knowledge_base(KnowledgeBase *kb) {
    if (kb->pairs) {
        free(kb->pairs);
        kb->pairs = NULL;
    }
    kb->count = 0;
    kb->capacity = 0;
}

// ファイルをトークナイズ
int tokenize_file(const char *filepath, const char *output_filepath) {
    // ファイルの内容を読み込む
    char *content = read_file_content(filepath);
    if (!content) {
        return 0;
    }
    
    // 内容が空の場合はスキップ
    if (strlen(content) == 0) {
        free(content);
        return 0;
    }
    
    // ファイルの拡張子を取得
    char *ext = get_file_extension(filepath);
    
    // マークダウンファイルの場合、マークダウン記法を除去
    if (ext && strcmp(ext, "md") == 0) {
        // マークダウン記法を簡易的に除去
        char *processed_content = (char *)malloc(strlen(content) + 1);
        if (processed_content) {
            char *src = content;
            char *dst = processed_content;
            int in_code_block = 0;
            
            while (*src) {
                // コードブロックをスキップ
                if (strncmp(src, "```", 3) == 0) {
                    in_code_block = !in_code_block;
                    src += 3;
                    while (*src && *src != '\n') src++;
                    if (*src) src++;
                    continue;
                }
                
                // コードブロック内ならスキップ
                if (in_code_block) {
                    while (*src && *src != '\n') src++;
                    if (*src) src++;
                    continue;
                }
                
                // 見出しの#を削除
                if (*src == '#') {
                    while (*src == '#') src++;
                    if (*src == ' ') src++;
                }
                
                // リンク [text](url) -> text
                if (*src == '[') {
                    char *link_start = src;
                    src++;
                    while (*src && *src != ']') {
                        *dst++ = *src++;
                    }
                    if (*src == ']' && *(src+1) == '(') {
                        src += 2;
                        while (*src && *src != ')') src++;
                        if (*src) src++;
                    } else {
                        // リンク形式でなければ元に戻す
                        src = link_start;
                        *dst++ = *src++;
                    }
                    continue;
                }
                
                // 強調 **text** -> text または *text* -> text
                if (*src == '*') {
                    int count = 0;
                    while (*src == '*') {
                        count++;
                        src++;
                    }
                    if (count == 1 || count == 2) {
                        char *end = strstr(src, count == 1 ? "*" : "**");
                        if (end) {
                            while (src < end) {
                                *dst++ = *src++;
                            }
                            src += count;
                            continue;
                        }
                    }
                    // 強調形式でなければ元に戻す
                    for (int i = 0; i < count; i++) {
                        *dst++ = '*';
                    }
                    continue;
                }
                
                // その他の文字はそのままコピー
                *dst++ = *src++;
            }
            
            *dst = '\0';
            free(content);
            content = processed_content;
        }
    }
    // テキストファイルの場合、質問|回答形式を処理
    else if (ext && strcmp(ext, "txt") == 0) {
        // 質問|回答形式を検出
        if (strchr(content, '|')) {
            // 質問と回答を抽出
            char *questions = NULL;
            char *answers = NULL;
            size_t questions_size = 0;
            size_t answers_size = 0;
            size_t questions_capacity = 1024;
            size_t answers_capacity = 1024;
            
            questions = (char *)malloc(questions_capacity);
            answers = (char *)malloc(answers_capacity);
            
            if (questions && answers) {
                questions[0] = '\0';
                answers[0] = '\0';
                
                char *line = content;
                char *next_line;
                
                while (line && *line) {
                    next_line = strchr(line, '\n');
                    if (next_line) {
                        *next_line = '\0';
                    }
                    
                    // 質問|回答形式の行を処理
                    char *separator = strchr(line, '|');
                    if (separator && line[0] != '#') {
                        *separator = '\0';
                        
                        // 質問を追加
                        size_t line_len = strlen(line);
                        if (questions_size + line_len + 2 > questions_capacity) {
                            questions_capacity *= 2;
                            questions = (char *)realloc(questions, questions_capacity);
                            if (!questions) break;
                        }
                        
                        if (questions_size > 0) {
                            strcat(questions, "\n");
                            questions_size++;
                        }
                        strcat(questions, line);
                        questions_size += line_len;
                        
                        // 回答を追加
                        line_len = strlen(separator + 1);
                        if (answers_size + line_len + 2 > answers_capacity) {
                            answers_capacity *= 2;
                            answers = (char *)realloc(answers, answers_capacity);
                            if (!answers) break;
                        }
                        
                        if (answers_size > 0) {
                            strcat(answers, "\n");
                            answers_size++;
                        }
                        strcat(answers, separator + 1);
                        answers_size += line_len;
                        
                        *separator = '|';  // 元に戻す
                    }
                    
                    if (next_line) {
                        *next_line = '\n';  // 元に戻す
                        line = next_line + 1;
                    } else {
                        break;
                    }
                }
                
                // 質問と回答を結合
                if (questions_size > 0 && answers_size > 0) {
                    free(content);
                    content = (char *)malloc(questions_size + answers_size + 3);
                    if (content) {
                        sprintf(content, "%s\n\n%s", questions, answers);
                    }
                }
                
                // メモリを解放
                free(questions);
                free(answers);
                
                if (!content) {
                    return 0;
                }
            }
        }
    }
    
    // 内容が空白文字のみの場合はスキップ
    int is_empty = 1;
    for (char *p = content; *p; p++) {
        if (!isspace((unsigned char)*p)) {
            is_empty = 0;
            break;
        }
    }
    
    if (is_empty) {
        free(content);
        return 0;
    }
    
    // 出力ディレクトリを作成
    char output_dir[MAX_PATH_LENGTH];
    strcpy(output_dir, output_filepath);
    char *last_slash = strrchr(output_dir, '/');
    if (last_slash) {
        *last_slash = '\0';
        create_directory(output_dir);
    }
    
    // トークナイズ処理用の一時ファイル
    char temp_content_file[MAX_PATH_LENGTH];
    snprintf(temp_content_file, MAX_PATH_LENGTH, "%s/temp_content_%d.txt", temp_dir, (int)time(NULL));
    
    // 一時ファイルを作成
    FILE *fp = fopen(temp_content_file, "w");
    if (!fp) {
        free(content);
        return 0;
    }
    
    fputs(content, fp);
    fclose(fp);
    
    // 大きなファイルの場合は分割して処理
    if (strlen(content) > 10000) {
        printf("  大きなファイルを分割して処理します: %s\n", filepath);
        
        // 内容を一時ファイルに書き込む
        FILE *fp = fopen(temp_content_file, "w");
        if (fp) {
            fputs(content, fp);
            fclose(fp);
            
            // 最初の10000文字だけをトークナイズ
            char command[MAX_PATH_LENGTH * 3];
            snprintf(command, MAX_PATH_LENGTH * 3, 
                     "%s/bin/tokens tokenize \"$(head -c 10000 %s)\" > %s",
                     workspace_dir, temp_content_file, output_filepath);
            system(command);
        } else {
            printf("  警告: 一時ファイルの作成に失敗しました\n");
            return 0;
        }
    } else {
        // 内容を一時ファイルに書き込む
        FILE *fp = fopen(temp_content_file, "w");
        if (fp) {
            fputs(content, fp);
            fclose(fp);
            
            // 通常のトークナイズ処理
            char command[MAX_PATH_LENGTH * 3];
            snprintf(command, MAX_PATH_LENGTH * 3, 
                     "%s/bin/tokens tokenize \"$(cat %s)\" > %s",
                     workspace_dir, temp_content_file, output_filepath);
            system(command);
        } else {
            printf("  警告: 一時ファイルの作成に失敗しました\n");
            return 0;
        }
    }
    
    // 一時ファイルを削除
    remove(temp_content_file);
    
    // メモリを解放
    free(content);
    
    // トークナイズ結果を確認
    if (file_exists(output_filepath) && !is_file_empty(output_filepath)) {
        return 1;
    } else {
        return 0;
    }
}

// 単語ベクトルを更新
int update_word_vectors(const char *tokenized_dir) {
    // 単語ベクトル更新コマンドを実行
    char command[MAX_PATH_LENGTH * 3];
    snprintf(command, MAX_PATH_LENGTH * 3, 
             "%s/bin/update-vectors %s",
             workspace_dir, tokenized_dir);
    return system(command) == 0;
}

// ファイルが変更されているか確認
int is_file_changed(const char *filepath, const char *processed_files) {
    if (!file_exists(processed_files)) {
        return 1;
    }
    
    FILE *fp = fopen(processed_files, "r");
    if (!fp) {
        return 1;
    }
    
    char line[MAX_PATH_LENGTH + 20];
    while (fgets(line, MAX_PATH_LENGTH + 20, fp)) {
        // 改行を削除
        line[strcspn(line, "\n")] = 0;
        
        // ファイルパスとタイムスタンプを分離
        char *separator = strchr(line, '|');
        if (separator) {
            *separator = '\0';
            
            // ファイルパスが一致する場合
            if (strcmp(line, filepath) == 0) {
                struct stat file_stat;
                if (stat(filepath, &file_stat) == 0) {
                    // タイムスタンプを比較
                    time_t processed_time = atol(separator + 1);
                    if (file_stat.st_mtime <= processed_time) {
                        fclose(fp);
                        return 0;
                    }
                }
                break;
            }
        }
    }
    
    fclose(fp);
    return 1;
}

// ファイルの拡張子を取得
char *get_file_extension(const char *filepath) {
    char *dot = strrchr(filepath, '.');
    if (!dot || dot == filepath) {
        return NULL;
    }
    return dot + 1;
}

// ディレクトリを作成
int create_directory(const char *path) {
    char tmp[MAX_PATH_LENGTH];
    char *p = NULL;
    size_t len;
    
    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    
    // 末尾のスラッシュを削除
    if (tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }
    
    // ディレクトリが既に存在する場合
    if (is_directory(tmp)) {
        return 1;
    }
    
    // 再帰的にディレクトリを作成
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            // ディレクトリが存在しない場合は作成
            if (!is_directory(tmp)) {
                if (mkdir(tmp, 0755) != 0) {
                    return 0;
                }
            }
            *p = '/';
        }
    }
    
    // 最後のディレクトリを作成
    if (!is_directory(tmp)) {
        if (mkdir(tmp, 0755) != 0) {
            return 0;
        }
    }
    
    return 1;
}

// ファイルが存在するか確認
int file_exists(const char *filepath) {
    return access(filepath, F_OK) != -1;
}

// ファイルが空かどうか確認
int is_file_empty(const char *filepath) {
    struct stat st;
    if (stat(filepath, &st) == 0) {
        return st.st_size == 0;
    }
    return 1;
}

// パスがディレクトリかどうか確認
int is_directory(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return 0;
}

// ファイルの内容を読み込む
char *read_file_content(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        return NULL;
    }
    
    // ファイルサイズを取得
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    
    // メモリを割り当て
    char *content = (char *)malloc(size + 1);
    if (!content) {
        fclose(fp);
        return NULL;
    }
    
    // ファイルを読み込む
    size_t read_size = fread(content, 1, size, fp);
    content[read_size] = '\0';
    
    fclose(fp);
    return content;
}

// ファイルに内容を書き込む
int write_file_content(const char *filepath, const char *content) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        return 0;
    }
    
    fputs(content, fp);
    fclose(fp);
    return 1;
}

// ファイルに内容を追加
void append_to_file(const char *filepath, const char *content) {
    pthread_mutex_lock(&file_mutex);
    
    FILE *fp = fopen(filepath, "a");
    if (fp) {
        fputs(content, fp);
        fclose(fp);
    }
    
    pthread_mutex_unlock(&file_mutex);
}

// 相対パスを取得
char *get_relative_path(const char *base_path, const char *full_path) {
    size_t base_len = strlen(base_path);
    size_t full_len = strlen(full_path);
    
    if (full_len < base_len || strncmp(base_path, full_path, base_len) != 0) {
        // ベースパスが含まれていない場合はフルパスをコピー
        char *result = (char *)malloc(full_len + 1);
        if (result) {
            strcpy(result, full_path);
        }
        return result;
    }
    
    // ベースパスの後の部分を取得
    char *result = (char *)malloc(full_len - base_len + 1);
    if (result) {
        if (full_path[base_len] == '/') {
            strcpy(result, full_path + base_len + 1);
        } else {
            strcpy(result, full_path + base_len);
        }
    }
    return result;
}