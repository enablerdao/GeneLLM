#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <wchar.h>
#include <stdbool.h>
#include <time.h>

#define MAX_WORD_LEN 128
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
    size_t len = strlen(word);
    if (len >= MAX_WORD_LEN || len <= 1) {
        return;
    }
    
    // 数字のみの単語は無視
    bool is_number = true;
    for (size_t i = 0; i < len; i++) {
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

// 日本語の文字かどうかをチェック
bool is_japanese_char(const char* str) {
    // UTF-8の日本語文字は3バイト以上
    unsigned char c = (unsigned char)str[0];
    return (c >= 0xE0);
}

// 文字列が日本語の区切り文字かどうかをチェック
bool is_japanese_delimiter(const char* str) {
    // 句読点、括弧などをチェック
    return (strcmp(str, "。") == 0 || 
            strcmp(str, "、") == 0 || 
            strcmp(str, "，") == 0 || 
            strcmp(str, "．") == 0 || 
            strcmp(str, "！") == 0 || 
            strcmp(str, "？") == 0 || 
            strcmp(str, "「") == 0 || 
            strcmp(str, "」") == 0 || 
            strcmp(str, "（") == 0 || 
            strcmp(str, "）") == 0);
}

// テキストを単語に分割して処理
void process_text(const char* text) {
    char line[MAX_LINE_LEN];
    char* line_ptr = line;
    
    // 1行ずつ処理
    const char* start = text;
    const char* end = strchr(start, '\n');
    
    while (start != NULL) {
        if (end == NULL) {
            // 最後の行
            strncpy(line, start, MAX_LINE_LEN - 1);
            line[MAX_LINE_LEN - 1] = '\0';
        } else {
            int len = end - start;
            if (len >= MAX_LINE_LEN) len = MAX_LINE_LEN - 1;
            strncpy(line, start, len);
            line[len] = '\0';
        }
        
        // 行を単語に分割
        char* token = strtok(line, " \t\r\n.,;:!?()[]{}\"'");
        while (token != NULL) {
            add_word(token);
            token = strtok(NULL, " \t\r\n.,;:!?()[]{}\"'");
        }
        
        // 次の行へ
        if (end == NULL) break;
        start = end + 1;
        end = strchr(start, '\n');
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
        printf("%3d: %-30s %d\n", i + 1, word_freqs[i].word, word_freqs[i].count);
    }
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

// 文字列から日本語の単語を抽出
void extract_japanese_words(const char* text) {
    // 日本語の単語を抽出するための簡易的な方法
    // 実際には形態素解析器を使うべきだが、ここでは簡易的に実装
    
    char buffer[MAX_LINE_LEN];
    strncpy(buffer, text, MAX_LINE_LEN - 1);
    buffer[MAX_LINE_LEN - 1] = '\0';
    
    // 空白で分割
    char* token = strtok(buffer, " \t\r\n");
    while (token != NULL) {
        // 括弧や記号を除去
        char clean_token[MAX_WORD_LEN];
        int clean_len = 0;
        
        for (int i = 0; token[i] != '\0' && clean_len < MAX_WORD_LEN - 1; i++) {
            if (token[i] != '(' && token[i] != ')' && 
                token[i] != '[' && token[i] != ']' && 
                token[i] != '{' && token[i] != '}' && 
                token[i] != '「' && token[i] != '」' && 
                token[i] != '（' && token[i] != '）') {
                clean_token[clean_len++] = token[i];
            }
        }
        clean_token[clean_len] = '\0';
        
        if (clean_len > 0) {
            add_word(clean_token);
        }
        
        token = strtok(NULL, " \t\r\n");
    }
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
    
    // 日本語の単語を抽出
    extract_japanese_words(text_buffer);
    
    // 単語の出現頻度を表示
    print_word_frequencies(30);
    
    printf("\n");
    
    // 簡単な文生成
    printf("簡単な文生成:\n");
    for (int i = 0; i < 5; i++) {
        generate_simple_sentence();
    }
    
    return 0;
}