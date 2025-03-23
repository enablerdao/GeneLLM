#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <wchar.h>
#include <stdbool.h>
#include <time.h>

#define MAX_WORD_LEN 64
#define MAX_WORDS 10000
#define MAX_TEXT_LEN 1000000
#define MAX_LINE_LEN 10000

// 単語の出現頻度を記録する構造体
typedef struct {
    char word[MAX_WORD_LEN];
    int count;
} WordFreq;

// 単語の出現頻度を記録する配列
WordFreq word_freqs[MAX_WORDS];
int word_count = 0;

// テキストバッファ
char text_buffer[MAX_TEXT_LEN];
int text_len = 0;

// 単語が既に配列に存在するかチェックし、存在する場合はそのインデックスを返す
int find_word(const char* word) {
    for (int i = 0; i < word_count; i++) {
        if (strcmp(word_freqs[i].word, word) == 0) {
            return i;
        }
    }
    return -1;
}

// 単語を追加または出現回数を増やす
void add_word(const char* word) {
    // 単語の長さをチェック
    if (strlen(word) >= MAX_WORD_LEN || strlen(word) <= 1) {
        return;
    }
    
    // 数字のみの単語は無視
    bool is_number = true;
    for (int i = 0; word[i] != '\0'; i++) {
        if (!isdigit((unsigned char)word[i])) {
            is_number = false;
            break;
        }
    }
    if (is_number) {
        return;
    }
    
    int index = find_word(word);
    if (index >= 0) {
        // 既存の単語の出現回数を増やす
        word_freqs[index].count++;
    } else if (word_count < MAX_WORDS) {
        // 新しい単語を追加
        strncpy(word_freqs[word_count].word, word, MAX_WORD_LEN - 1);
        word_freqs[word_count].word[MAX_WORD_LEN - 1] = '\0';
        word_freqs[word_count].count = 1;
        word_count++;
    }
}

// 単語の出現頻度でソート（降順）
int compare_word_freq(const void* a, const void* b) {
    const WordFreq* wa = (const WordFreq*)a;
    const WordFreq* wb = (const WordFreq*)b;
    return wb->count - wa->count;
}

// テキストを単語に分割して処理
void process_text(const char* text) {
    char word[MAX_WORD_LEN];
    int word_len = 0;
    
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        
        // 単語の区切り文字かどうかをチェック
        if (isalnum((unsigned char)c) || c == '_' || (unsigned char)c >= 128) {
            // 単語の一部として追加
            if (word_len < MAX_WORD_LEN - 1) {
                word[word_len++] = c;
            }
        } else {
            // 単語の終わり
            if (word_len > 0) {
                word[word_len] = '\0';
                add_word(word);
                word_len = 0;
            }
        }
    }
    
    // 最後の単語を処理
    if (word_len > 0) {
        word[word_len] = '\0';
        add_word(word);
    }
}

// ファイルからテキストを読み込む
int read_text_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("ファイルを開けませんでした: %s\n", filename);
        return 0;
    }
    
    text_len = 0;
    char line[MAX_LINE_LEN];
    
    while (fgets(line, MAX_LINE_LEN, file) && text_len < MAX_TEXT_LEN - MAX_LINE_LEN) {
        int line_len = strlen(line);
        if (text_len + line_len < MAX_TEXT_LEN) {
            strcpy(text_buffer + text_len, line);
            text_len += line_len;
        } else {
            break;
        }
    }
    
    fclose(file);
    return 1;
}

// 単語の出現頻度を表示
void print_word_frequencies(int top_n) {
    // 出現頻度でソート
    qsort(word_freqs, word_count, sizeof(WordFreq), compare_word_freq);
    
    // 上位N個の単語を表示
    printf("上位%d個の単語の出現頻度:\n", top_n);
    for (int i = 0; i < top_n && i < word_count; i++) {
        printf("%3d: %-20s %d\n", i + 1, word_freqs[i].word, word_freqs[i].count);
    }
}

// 単語の共起関係を分析
void analyze_cooccurrence(const char* target_word, int window_size) {
    // 対象単語のインデックスを取得
    int target_index = find_word(target_word);
    if (target_index < 0) {
        printf("単語 '%s' は見つかりませんでした。\n", target_word);
        return;
    }
    
    // 共起頻度を記録する配列
    int* cooccurrence = (int*)calloc(word_count, sizeof(int));
    if (!cooccurrence) {
        printf("メモリ割り当てに失敗しました。\n");
        return;
    }
    
    // テキストを再度処理して共起関係を分析
    char words[MAX_WORDS][MAX_WORD_LEN];
    int words_count = 0;
    
    char word[MAX_WORD_LEN];
    int word_len = 0;
    
    // テキストを単語に分割
    for (int i = 0; text_buffer[i] != '\0'; i++) {
        char c = text_buffer[i];
        
        if (isalnum((unsigned char)c) || c == '_' || (unsigned char)c >= 128) {
            if (word_len < MAX_WORD_LEN - 1) {
                word[word_len++] = c;
            }
        } else {
            if (word_len > 0) {
                word[word_len] = '\0';
                if (words_count < MAX_WORDS) {
                    strncpy(words[words_count], word, MAX_WORD_LEN - 1);
                    words[words_count][MAX_WORD_LEN - 1] = '\0';
                    words_count++;
                }
                word_len = 0;
            }
        }
    }
    
    // 最後の単語を処理
    if (word_len > 0 && words_count < MAX_WORDS) {
        word[word_len] = '\0';
        strncpy(words[words_count], word, MAX_WORD_LEN - 1);
        words[words_count][MAX_WORD_LEN - 1] = '\0';
        words_count++;
    }
    
    // 共起関係を分析
    for (int i = 0; i < words_count; i++) {
        if (strcmp(words[i], target_word) == 0) {
            // ウィンドウ内の単語を処理
            for (int j = i - window_size; j <= i + window_size; j++) {
                if (j >= 0 && j < words_count && j != i) {
                    int index = find_word(words[j]);
                    if (index >= 0) {
                        cooccurrence[index]++;
                    }
                }
            }
        }
    }
    
    // 共起頻度の高い単語を表示
    printf("単語 '%s' との共起頻度が高い単語:\n", target_word);
    
    // 共起頻度でソートするための一時配列
    typedef struct {
        int index;
        int count;
    } CooccurrenceItem;
    
    CooccurrenceItem* items = (CooccurrenceItem*)malloc(word_count * sizeof(CooccurrenceItem));
    if (!items) {
        printf("メモリ割り当てに失敗しました。\n");
        free(cooccurrence);
        return;
    }
    
    for (int i = 0; i < word_count; i++) {
        items[i].index = i;
        items[i].count = cooccurrence[i];
    }
    
    // 共起頻度でソート
    for (int i = 0; i < word_count - 1; i++) {
        for (int j = i + 1; j < word_count; j++) {
            if (items[i].count < items[j].count) {
                CooccurrenceItem temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }
    
    // 上位10個の共起単語を表示
    for (int i = 0; i < 10 && i < word_count; i++) {
        if (items[i].count > 0) {
            printf("%3d: %-20s %d\n", i + 1, word_freqs[items[i].index].word, items[i].count);
        }
    }
    
    free(items);
    free(cooccurrence);
}

// 簡単な文生成
void generate_simple_sentence() {
    // 出現頻度でソート
    qsort(word_freqs, word_count, sizeof(WordFreq), compare_word_freq);
    
    // 上位の単語から選択
    int top_range = word_count > 100 ? 100 : word_count;
    
    // ランダムに単語を選択
    int subject_idx = rand() % top_range;
    int verb_idx = rand() % top_range;
    int object_idx = rand() % top_range;
    
    printf("生成された文: %s は %s を %s。\n", 
           word_freqs[subject_idx].word, 
           word_freqs[object_idx].word, 
           word_freqs[verb_idx].word);
}

int main(int argc, char* argv[]) {
    // ロケールを設定（日本語対応）
    setlocale(LC_ALL, "");
    
    // 乱数の初期化
    srand((unsigned int)time(NULL));
    
    if (argc < 2) {
        printf("使用方法: %s <テキストファイル>\n", argv[0]);
        return 1;
    }
    
    // テキストファイルを読み込む
    if (!read_text_file(argv[1])) {
        return 1;
    }
    
    // テキストを処理
    process_text(text_buffer);
    
    // 単語の出現頻度を表示
    print_word_frequencies(30);
    
    printf("\n");
    
    // 共起関係を分析（上位の単語から選択）
    if (word_count > 0) {
        // 出現頻度でソート
        qsort(word_freqs, word_count, sizeof(WordFreq), compare_word_freq);
        
        // 上位の単語の共起関係を分析
        if (word_count > 0) {
            analyze_cooccurrence(word_freqs[0].word, 5);
        }
    }
    
    printf("\n");
    
    // 簡単な文生成
    for (int i = 0; i < 5; i++) {
        generate_simple_sentence();
    }
    
    return 0;
}