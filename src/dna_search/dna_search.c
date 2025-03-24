#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024
#define MAX_DNA_LENGTH 32
#define MAX_DESC_LENGTH 256
#define MAX_RESULTS 20
#define MAX_COMBINATIONS 10000
#define SAMPLE_SIZE 1000
#define TIMEOUT_SECONDS 5

// DNAコードの構造体
typedef struct {
    char code[MAX_DNA_LENGTH];
    char description[MAX_DESC_LENGTH];
    double similarity;
} DNAEntry;

// DNAコードの要素
typedef struct {
    char entity[8];      // 主語 (E0, E1, ...)
    char concept[8];     // 動詞 (C0, C1, ...)
    char result[8];      // 目的語 (R0, R1, ...)
    char attribute[8];   // 属性 (A0, A1, ...)
    char location[8];    // 場所 (L0, L1, ...)
    char manner[8];      // 様態 (M0, M1, ...)
} DNAComponents;

// 結果配列
DNAEntry results[MAX_COMBINATIONS];
int result_count = 0;

// 文字列から部分文字列を抽出する関数
void extract_substring(const char* src, char* dest, char start_char, char end_char) {
    dest[0] = '\0';
    char* start = strchr(src, start_char);
    if (!start) return;
    
    start++;
    char* end = NULL;
    
    if (end_char == '\0') {
        // 終了文字が指定されていない場合は文字列の終わりまで
        strcpy(dest, start);
        return;
    }
    
    end = strchr(start, end_char);
    if (!end) {
        strcpy(dest, start);
    } else {
        strncpy(dest, start, end - start);
        dest[end - start] = '\0';
    }
}

// DNAコードを解析して構成要素に分解する関数
void parse_dna_code(const char* dna_code, DNAComponents* components) {
    // 初期化
    components->entity[0] = '\0';
    components->concept[0] = '\0';
    components->result[0] = '\0';
    components->attribute[0] = '\0';
    components->location[0] = '\0';
    components->manner[0] = '\0';
    
    // 主語（E）の抽出
    if (dna_code[0] == 'E') {
        int i = 0;
        while (dna_code[i] != 'C' && dna_code[i] != '\0') {
            i++;
        }
        strncpy(components->entity, dna_code, i);
        components->entity[i] = '\0';
    }
    
    // 動詞（C）の抽出
    char* c_pos = strchr(dna_code, 'C');
    if (c_pos) {
        int i = 0;
        c_pos++;
        while (c_pos[i] != 'R' && c_pos[i] != '\0') {
            i++;
        }
        strncpy(components->concept, c_pos - 1, i + 1);
        components->concept[i + 1] = '\0';
    }
    
    // 目的語（R）の抽出
    char* r_pos = strchr(dna_code, 'R');
    if (r_pos) {
        int i = 0;
        r_pos++;
        while (r_pos[i] != 'A' && r_pos[i] != 'L' && r_pos[i] != 'M' && r_pos[i] != '\0') {
            i++;
        }
        strncpy(components->result, r_pos - 1, i + 1);
        components->result[i + 1] = '\0';
    }
    
    // 属性（A）の抽出
    char* a_pos = strchr(dna_code, 'A');
    if (a_pos) {
        int i = 0;
        a_pos++;
        while (a_pos[i] != 'L' && a_pos[i] != 'M' && a_pos[i] != '\0') {
            i++;
        }
        strncpy(components->attribute, a_pos - 1, i + 1);
        components->attribute[i + 1] = '\0';
    }
    
    // 場所（L）の抽出
    char* l_pos = strchr(dna_code, 'L');
    if (l_pos) {
        int i = 0;
        l_pos++;
        while (l_pos[i] != 'M' && l_pos[i] != '\0') {
            i++;
        }
        strncpy(components->location, l_pos - 1, i + 1);
        components->location[i + 1] = '\0';
    }
    
    // 様態（M）の抽出
    char* m_pos = strchr(dna_code, 'M');
    if (m_pos) {
        strcpy(components->manner, m_pos);
    }
}

// 類似度を計算する関数
double calculate_similarity(const char* dna1, const char* dna2) {
    DNAComponents comp1, comp2;
    parse_dna_code(dna1, &comp1);
    parse_dna_code(dna2, &comp2);
    
    double similarity_score = 0.0;
    double weight_sum = 0.0;
    
    // 主語（E）の比較（重み: 3.0）
    if (comp1.entity[0] != '\0' && comp2.entity[0] != '\0') {
        weight_sum += 3.0;
        if (strcmp(comp1.entity, comp2.entity) == 0) {
            similarity_score += 3.0;
        }
    }
    
    // 動詞（C）の比較（重み: 2.0）
    if (comp1.concept[0] != '\0' && comp2.concept[0] != '\0') {
        weight_sum += 2.0;
        if (strcmp(comp1.concept, comp2.concept) == 0) {
            similarity_score += 2.0;
        }
    }
    
    // 目的語（R）の比較（重み: 2.0）
    if (comp1.result[0] != '\0' && comp2.result[0] != '\0') {
        weight_sum += 2.0;
        if (strcmp(comp1.result, comp2.result) == 0) {
            similarity_score += 2.0;
        }
    }
    
    // 属性（A）の比較（重み: 1.0）
    if (comp1.attribute[0] != '\0' && comp2.attribute[0] != '\0') {
        weight_sum += 1.0;
        if (strcmp(comp1.attribute, comp2.attribute) == 0) {
            similarity_score += 1.0;
        }
    }
    
    // 場所（L）の比較（重み: 1.0）
    if (comp1.location[0] != '\0' && comp2.location[0] != '\0') {
        weight_sum += 1.0;
        if (strcmp(comp1.location, comp2.location) == 0) {
            similarity_score += 1.0;
        }
    }
    
    // 様態（M）の比較（重み: 1.0）
    if (comp1.manner[0] != '\0' && comp2.manner[0] != '\0') {
        weight_sum += 1.0;
        if (strcmp(comp1.manner, comp2.manner) == 0) {
            similarity_score += 1.0;
        }
    }
    
    // 最終的な類似度スコアを計算
    if (weight_sum == 0.0) {
        return 0.0;
    } else {
        return similarity_score / weight_sum;
    }
}

// 質問からDNAコードを生成する関数
void generate_dna_from_query(const char* query, char* dna_code) {
    // デフォルトのDNAコード
    strcpy(dna_code, "E0C3R1");  // GeneLLMは解析する知識ベースを
    
    // 主語の抽出
    if (strstr(query, "知識グラフ")) {
        dna_code[1] = '5';  // E5
    } else if (strstr(query, "自然言語処理")) {
        dna_code[1] = '6';  // E6
    } else if (strstr(query, "機械学習")) {
        dna_code[1] = '7';  // E7
    } else if (strstr(query, "深層学習")) {
        dna_code[1] = '8';  // E8
    } else if (strstr(query, "強化学習")) {
        dna_code[1] = '9';  // E9
    }
    
    // 動詞の抽出
    if (strstr(query, "最適化")) {
        dna_code[3] = '5';  // C5
    } else if (strstr(query, "生成")) {
        dna_code[3] = '4';  // C4
    } else if (strstr(query, "学習")) {
        dna_code[3] = '1';  // C10 (2桁なので特別処理)
        dna_code[4] = '0';
        dna_code[5] = 'R';
    } else if (strstr(query, "処理")) {
        dna_code[3] = '1';  // C11 (2桁なので特別処理)
        dna_code[4] = '1';
        dna_code[5] = 'R';
    } else if (strstr(query, "実装")) {
        dna_code[3] = '1';  // C1
    } else if (strstr(query, "高速化")) {
        dna_code[3] = '2';  // C2
    } else if (strstr(query, "抽出")) {
        dna_code[3] = '7';  // C7
    } else if (strstr(query, "分類")) {
        dna_code[3] = '8';  // C8
    } else if (strstr(query, "予測")) {
        dna_code[3] = '9';  // C9
    } else if (strstr(query, "分析")) {
        dna_code[3] = '3';  // C3
    }
    
    // 目的語の抽出
    if (strstr(query, "テキストデータ")) {
        if (dna_code[4] == '0' && dna_code[5] == 'R') {
            dna_code[6] = '3';  // R3
        } else {
            dna_code[5] = '3';  // R3
        }
    } else if (strstr(query, "バイナリデータ")) {
        if (dna_code[4] == '0' && dna_code[5] == 'R') {
            dna_code[6] = '4';  // R4
        } else {
            dna_code[5] = '4';  // R4
        }
    } else if (strstr(query, "構造化データ")) {
        if (dna_code[4] == '0' && dna_code[5] == 'R') {
            dna_code[6] = '5';  // R5
        } else {
            dna_code[5] = '5';  // R5
        }
    } else if (strstr(query, "非構造化データ")) {
        if (dna_code[4] == '0' && dna_code[5] == 'R') {
            dna_code[6] = '6';  // R6
        } else {
            dna_code[5] = '6';  // R6
        }
    } else if (strstr(query, "メタデータ")) {
        if (dna_code[4] == '0' && dna_code[5] == 'R') {
            dna_code[6] = '7';  // R7
        } else {
            dna_code[5] = '7';  // R7
        }
    } else if (strstr(query, "特徴量")) {
        if (dna_code[4] == '0' && dna_code[5] == 'R') {
            dna_code[6] = '8';  // R8
        } else {
            dna_code[5] = '8';  // R8
        }
    } else if (strstr(query, "パターン")) {
        if (dna_code[4] == '0' && dna_code[5] == 'R') {
            dna_code[6] = '9';  // R9
        } else {
            dna_code[5] = '9';  // R9
        }
    } else if (strstr(query, "モデル")) {
        if (dna_code[4] == '0' && dna_code[5] == 'R') {
            dna_code[6] = '1';  // R10 (2桁なので特別処理)
            dna_code[7] = '0';
        } else {
            dna_code[5] = '1';  // R10 (2桁なので特別処理)
            dna_code[6] = '0';
        }
    }
    
    // 属性の抽出
    int attr_pos = strlen(dna_code);
    if (strstr(query, "高速")) {
        dna_code[attr_pos] = 'A';
        dna_code[attr_pos+1] = '0';
        dna_code[attr_pos+2] = '\0';
    } else if (strstr(query, "効率的")) {
        dna_code[attr_pos] = 'A';
        dna_code[attr_pos+1] = '1';
        dna_code[attr_pos+2] = '\0';
    } else if (strstr(query, "安定")) {
        dna_code[attr_pos] = 'A';
        dna_code[attr_pos+1] = '2';
        dna_code[attr_pos+2] = '\0';
    } else if (strstr(query, "信頼性")) {
        dna_code[attr_pos] = 'A';
        dna_code[attr_pos+1] = '3';
        dna_code[attr_pos+2] = '\0';
    } else if (strstr(query, "スケーラブル")) {
        dna_code[attr_pos] = 'A';
        dna_code[attr_pos+1] = '4';
        dna_code[attr_pos+2] = '\0';
    } else if (strstr(query, "柔軟")) {
        dna_code[attr_pos] = 'A';
        dna_code[attr_pos+1] = '5';
        dna_code[attr_pos+2] = '\0';
    }
    
    // 場所の抽出
    int loc_pos = strlen(dna_code);
    if (strstr(query, "メモリ上")) {
        dna_code[loc_pos] = 'L';
        dna_code[loc_pos+1] = '0';
        dna_code[loc_pos+2] = '\0';
    } else if (strstr(query, "クラウド")) {
        dna_code[loc_pos] = 'L';
        dna_code[loc_pos+1] = '2';
        dna_code[loc_pos+2] = '\0';
    } else if (strstr(query, "ローカル環境")) {
        dna_code[loc_pos] = 'L';
        dna_code[loc_pos+1] = '3';
        dna_code[loc_pos+2] = '\0';
    } else if (strstr(query, "サーバー")) {
        dna_code[loc_pos] = 'L';
        dna_code[loc_pos+1] = '4';
        dna_code[loc_pos+2] = '\0';
    } else if (strstr(query, "データベース")) {
        dna_code[loc_pos] = 'L';
        dna_code[loc_pos+1] = '5';
        dna_code[loc_pos+2] = '\0';
    } else if (strstr(query, "分散システム")) {
        dna_code[loc_pos] = 'L';
        dna_code[loc_pos+1] = '9';
        dna_code[loc_pos+2] = '\0';
    }
    
    // 様態の抽出
    int manner_pos = strlen(dna_code);
    if (strstr(query, "効率的に")) {
        dna_code[manner_pos] = 'M';
        dna_code[manner_pos+1] = '0';
        dna_code[manner_pos+2] = '\0';
    } else if (strstr(query, "安全に")) {
        dna_code[manner_pos] = 'M';
        dna_code[manner_pos+1] = '1';
        dna_code[manner_pos+2] = '\0';
    } else if (strstr(query, "自動的に")) {
        dna_code[manner_pos] = 'M';
        dna_code[manner_pos+1] = '2';
        dna_code[manner_pos+2] = '\0';
    } else if (strstr(query, "迅速に")) {
        dna_code[manner_pos] = 'M';
        dna_code[manner_pos+1] = '3';
        dna_code[manner_pos+2] = '\0';
    } else if (strstr(query, "正確に")) {
        dna_code[manner_pos] = 'M';
        dna_code[manner_pos+1] = '4';
        dna_code[manner_pos+2] = '\0';
    }
}

// 結果を比較する関数（qsort用）
int compare_results(const void* a, const void* b) {
    const DNAEntry* entry_a = (const DNAEntry*)a;
    const DNAEntry* entry_b = (const DNAEntry*)b;
    
    if (entry_a->similarity > entry_b->similarity) return -1;
    if (entry_a->similarity < entry_b->similarity) return 1;
    return 0;
}

// DNAコードの意味を解析して回答を生成する関数
void generate_answer_from_dna(const char* dna_code, char* answer, size_t answer_size) {
    char entity[64] = "GeneLLM";
    char concept[64] = "解析する";
    char result[64] = "知識ベース";
    char attribute[64] = "";
    char location[64] = "";
    char manner[64] = "";
    
    // 主語（E）の解析
    if (strncmp(dna_code, "E0", 2) == 0) {
        strcpy(entity, "GeneLLM");
    } else if (strncmp(dna_code, "E5", 2) == 0) {
        strcpy(entity, "知識グラフ");
    } else if (strncmp(dna_code, "E6", 2) == 0) {
        strcpy(entity, "自然言語処理");
    } else if (strncmp(dna_code, "E7", 2) == 0) {
        strcpy(entity, "機械学習");
    } else if (strncmp(dna_code, "E8", 2) == 0) {
        strcpy(entity, "深層学習");
    }
    
    // 動詞（C）の解析
    if (strstr(dna_code, "C3")) {
        strcpy(concept, "解析する");
    } else if (strstr(dna_code, "C4")) {
        strcpy(concept, "生成する");
    } else if (strstr(dna_code, "C5")) {
        strcpy(concept, "最適化する");
    } else if (strstr(dna_code, "C10")) {
        strcpy(concept, "学習する");
    } else if (strstr(dna_code, "C11")) {
        strcpy(concept, "処理する");
    }
    
    // 目的語（R）の解析
    if (strstr(dna_code, "R1")) {
        strcpy(result, "知識ベース");
    } else if (strstr(dna_code, "R3")) {
        strcpy(result, "テキストデータ");
    } else if (strstr(dna_code, "R5")) {
        strcpy(result, "構造化データ");
    } else if (strstr(dna_code, "R6")) {
        strcpy(result, "非構造化データ");
    }
    
    // 属性（A）の解析
    if (strstr(dna_code, "A0")) {
        strcpy(attribute, "高速な");
    } else if (strstr(dna_code, "A1")) {
        strcpy(attribute, "効率的な");
    }
    
    // 場所（L）の解析
    if (strstr(dna_code, "L2")) {
        strcpy(location, "クラウド環境で");
    } else if (strstr(dna_code, "L5")) {
        strcpy(location, "データベース内で");
    }
    
    // 様態（M）の解析
    if (strstr(dna_code, "M0")) {
        strcpy(manner, "効率的に");
    } else if (strstr(dna_code, "M3")) {
        strcpy(manner, "迅速に");
    }
    
    // 回答を生成
    snprintf(answer, answer_size, 
        "%sは%sを%sします\n\n"
        "%sは以下のような方法で%sを%sします：\n\n"
        "1. データの前処理: 入力データを正規化し、ノイズを除去します\n"
        "2. 効率的なインデックス作成: 高速な検索のためのインデックスを構築します\n"
        "3. 冗長性の排除: 重複情報を削除して効率化します\n"
        "4. 分散処理: 大規模データを複数のノードで並列処理します\n"
        "5. キャッシュ最適化: 頻繁にアクセスされるデータをキャッシュします\n\n"
        "これらの技術により、%sは%sを効率的に%sし、ユーザーの質問に迅速に回答できます。",
        entity, result, concept,
        entity, result, concept,
        entity, result, concept
    );
    
    // 属性、場所、様態の情報を追加（存在する場合）
    if (attribute[0] != '\0' || location[0] != '\0' || manner[0] != '\0') {
        char additional_info[512] = "";
        
        if (attribute[0] != '\0') {
            snprintf(additional_info + strlen(additional_info), sizeof(additional_info) - strlen(additional_info),
                "\n\n%sは%sアルゴリズムを使用して処理を最適化します。", entity, attribute);
        }
        
        if (location[0] != '\0') {
            snprintf(additional_info + strlen(additional_info), sizeof(additional_info) - strlen(additional_info),
                "\n\n処理は%s実行されます。", location);
        }
        
        if (manner[0] != '\0') {
            snprintf(additional_info + strlen(additional_info), sizeof(additional_info) - strlen(additional_info),
                "\n\nシステムは%s動作するよう設計されています。", manner);
        }
        
        strncat(answer, additional_info, answer_size - strlen(answer) - 1);
    }
}

// タイムアウト処理用のアラームハンドラ
volatile sig_atomic_t timeout_flag = 0;

void timeout_handler(int sig) {
    timeout_flag = 1;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("使用方法: %s \"質問\" [結果数]\n", argv[0]);
        return 1;
    }
    
    const char* query = argv[1];
    int max_results = MAX_RESULTS;
    
    if (argc >= 3) {
        max_results = atoi(argv[2]);
        if (max_results <= 0 || max_results > MAX_RESULTS) {
            max_results = MAX_RESULTS;
        }
    }
    
    printf("質問: %s\n", query);
    printf("----------------------------------------\n");
    
    // 質問からDNAコードを生成
    char query_dna_code[MAX_DNA_LENGTH];
    generate_dna_from_query(query, query_dna_code);
    printf("質問から生成したDNAコード: %s\n", query_dna_code);
    
    // DNAコンビネーションファイルを開く
    FILE* file = fopen("/workspace/data/dna_combinations.txt", "r");
    if (!file) {
        printf("DNAコンビネーションファイルが見つかりません\n");
        return 1;
    }
    
    printf("類似したDNAコードを検索しています...\n");
    
    // サンプリング用の配列
    DNAEntry samples[SAMPLE_SIZE];
    int sample_count = 0;
    
    // ファイルからランダムにサンプリング
    srand(time(NULL));
    char line[MAX_LINE_LENGTH];
    int line_count = 0;
    
    // まずファイルの行数をカウント
    while (fgets(line, sizeof(line), file)) {
        line_count++;
    }
    
    // ファイルポインタを先頭に戻す
    rewind(file);
    
    // サンプリング
    printf("DNAコードデータベースからサンプリング中...\n");
    for (int i = 0; i < SAMPLE_SIZE && i < line_count; i++) {
        int random_line = rand() % line_count;
        rewind(file);
        
        // ランダムな行まで移動
        for (int j = 0; j < random_line; j++) {
            if (!fgets(line, sizeof(line), file)) {
                break;
            }
        }
        
        // サンプルを取得
        if (fgets(line, sizeof(line), file)) {
            // 改行を削除
            line[strcspn(line, "\n")] = '\0';
            
            // DNAコードと説明を分離
            char* separator = strchr(line, '|');
            if (separator) {
                *separator = '\0';
                strncpy(samples[sample_count].code, line, MAX_DNA_LENGTH - 1);
                samples[sample_count].code[MAX_DNA_LENGTH - 1] = '\0';
                
                strncpy(samples[sample_count].description, separator + 1, MAX_DESC_LENGTH - 1);
                samples[sample_count].description[MAX_DESC_LENGTH - 1] = '\0';
                
                sample_count++;
            }
        }
    }
    
    fclose(file);
    
    // 類似度計算
    printf("類似度計算中（タイムアウト: %d秒）...\n", TIMEOUT_SECONDS);
    
    // タイムアウト処理の設定
    signal(SIGALRM, timeout_handler);
    alarm(TIMEOUT_SECONDS);
    
    for (int i = 0; i < sample_count && !timeout_flag; i++) {
        double similarity = calculate_similarity(query_dna_code, samples[i].code);
        samples[i].similarity = similarity;
        
        // 結果配列に追加
        if (result_count < MAX_COMBINATIONS) {
            results[result_count] = samples[i];
            result_count++;
        }
    }
    
    // タイムアウトアラームを解除
    alarm(0);
    
    // 結果を類似度でソート
    qsort(results, result_count, sizeof(DNAEntry), compare_results);
    
    // 結果を表示
    printf("検索結果:\n");
    printf("----------------------------------------\n");
    
    if (timeout_flag) {
        printf("検索がタイムアウトしました。部分的な結果を表示します。\n");
    }
    
    int display_count = (result_count < max_results) ? result_count : max_results;
    for (int i = 0; i < display_count; i++) {
        printf("[%.4f] %s: %s\n", results[i].similarity, results[i].code, results[i].description);
    }
    
    printf("検索完了\n");
    
    // DNAコードから回答を生成
    char answer[4096];
    generate_answer_from_dna(query_dna_code, answer, sizeof(answer));
    
    printf("\n回答:\n");
    printf("----------------------------------------\n");
    printf("%s\n", answer);
    
    return 0;
}