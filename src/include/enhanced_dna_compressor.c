#include "enhanced_dna_compressor.h"

// 拡張DNA辞書の初期化
void init_enhanced_dna_dictionary(EnhancedDNADictionary *dict) {
    dict->count = 0;
    dict->capacity = MAX_DICT_ENTRIES;
    memset(dict->entries, 0, sizeof(EnhancedDNAEntry) * MAX_DICT_ENTRIES);
}

// 単語からDNAコードを取得（なければ新規作成）
const char* get_enhanced_dna_code(EnhancedDNADictionary *dict, const char *word, EnhancedDNAType type) {
    if (!word || !dict) return NULL;
    
    // 既存のエントリを検索
    for (int i = 0; i < dict->count; i++) {
        if (dict->entries[i].type == type && strcmp(dict->entries[i].word, word) == 0) {
            return dict->entries[i].code;
        }
    }
    
    // 新しいエントリを作成
    if (dict->count < dict->capacity) {
        EnhancedDNAEntry *entry = &dict->entries[dict->count];
        strncpy(entry->word, word, MAX_WORD_LEN - 1);
        entry->word[MAX_WORD_LEN - 1] = '\0';
        entry->type = type;
        
        // タイプごとのIDカウントを取得
        int type_count = 0;
        for (int i = 0; i < dict->count; i++) {
            if (dict->entries[i].type == type) {
                type_count++;
            }
        }
        entry->id = type_count;
        
        // DNAコードを生成
        char type_char;
        switch (type) {
            case ENTITY:    type_char = 'E'; break;
            case CONCEPT:   type_char = 'C'; break;
            case RESULT:    type_char = 'R'; break;
            case ATTRIBUTE: type_char = 'A'; break;
            case TIME:      type_char = 'T'; break;
            case LOCATION:  type_char = 'L'; break;
            case MANNER:    type_char = 'M'; break;
            case QUANTITY:  type_char = 'Q'; break;
            default:
                fprintf(stderr, "無効な拡張DNAタイプ: %d\n", type);
                return NULL;
        }
        
        snprintf(entry->code, MAX_DNA_CODE_LEN, "%c%02d", type_char, entry->id);
        dict->count++;
        
        return entry->code;
    }
    
    return NULL;
}

// DNAコードから単語を取得
const char* get_word_from_enhanced_dna(EnhancedDNADictionary *dict, const char *code) {
    if (!code || !dict || strlen(code) < 3) return NULL;
    
    for (int i = 0; i < dict->count; i++) {
        if (strcmp(dict->entries[i].code, code) == 0) {
            return dict->entries[i].word;
        }
    }
    
    return NULL;
}

// 文を拡張DNA形式に圧縮
char* compress_to_enhanced_dna(EnhancedDNADictionary *dict, const char *text) {
    if (!text || !dict) return NULL;
    
    // 実際の実装では形態素解析を使用して文を解析し、
    // 主語、動詞、目的語、属性などを抽出する必要があります
    
    // ここでは簡易的な実装として、最初の名詞を主語、最初の動詞を動詞、
    // 2番目の名詞を目的語として扱います
    
    static char dna_code[MAX_DNA_CODE_LEN * 8]; // 最大8つの要素を想定
    dna_code[0] = '\0';
    
    // 実際の実装ではここで形態素解析を行う
    // この例では簡易的に固定値を返す
    const char *entity = "GeneLLM";
    const char *concept = "実装する";
    const char *result = "知識ベース";
    const char *attribute = "高速";
    const char *time = "今日";
    const char *location = "メモリ上";
    const char *manner = "効率的に";
    const char *quantity = "多数";
    
    // 各要素のDNAコードを取得して結合
    const char *e_code = get_enhanced_dna_code(dict, entity, ENTITY);
    const char *c_code = get_enhanced_dna_code(dict, concept, CONCEPT);
    const char *r_code = get_enhanced_dna_code(dict, result, RESULT);
    const char *a_code = get_enhanced_dna_code(dict, attribute, ATTRIBUTE);
    const char *t_code = get_enhanced_dna_code(dict, time, TIME);
    const char *l_code = get_enhanced_dna_code(dict, location, LOCATION);
    const char *m_code = get_enhanced_dna_code(dict, manner, MANNER);
    const char *q_code = get_enhanced_dna_code(dict, quantity, QUANTITY);
    
    if (e_code) strcat(dna_code, e_code);
    if (c_code) strcat(dna_code, c_code);
    if (r_code) strcat(dna_code, r_code);
    if (a_code) strcat(dna_code, a_code);
    if (t_code) strcat(dna_code, t_code);
    if (l_code) strcat(dna_code, l_code);
    if (m_code) strcat(dna_code, m_code);
    if (q_code) strcat(dna_code, q_code);
    
    return dna_code;
}

// 拡張DNA形式から文を再構築
char* decompress_from_enhanced_dna(EnhancedDNADictionary *dict, const char *dna_code) {
    if (!dna_code || !dict) return NULL;
    
    static char text[1024];
    text[0] = '\0';
    
    int i = 0;
    while (i < strlen(dna_code)) {
        char code[4] = {0}; // 3文字のコード + NULL終端
        strncpy(code, dna_code + i, 3);
        code[3] = '\0';
        i += 3;
        
        const char *word = get_word_from_enhanced_dna(dict, code);
        if (!word) continue;
        
        // コードの種類に応じて適切な助詞や接続詞を追加
        switch (code[0]) {
            case 'E': // 主語
                strcat(text, word);
                strcat(text, "は");
                break;
            case 'C': // 動詞
                strcat(text, word);
                break;
            case 'R': // 目的語
                strcat(text, word);
                strcat(text, "を");
                break;
            case 'A': // 属性
                strcat(text, word);
                strcat(text, "な");
                break;
            case 'T': // 時間
                strcat(text, word);
                strcat(text, "に");
                break;
            case 'L': // 場所
                strcat(text, word);
                strcat(text, "で");
                break;
            case 'M': // 様態
                strcat(text, word);
                strcat(text, "に");
                break;
            case 'Q': // 数量
                strcat(text, word);
                strcat(text, "の");
                break;
            default:
                strcat(text, word);
                break;
        }
    }
    
    return text;
}

// 拡張DNA辞書をファイルに保存
int save_enhanced_dna_dictionary(EnhancedDNADictionary *dict, const char *filename) {
    if (!dict || !filename) return 0;
    
    FILE *fp = fopen(filename, "w");
    if (!fp) return 0;
    
    for (int i = 0; i < dict->count; i++) {
        fprintf(fp, "%s|%s\n", dict->entries[i].code, dict->entries[i].word);
    }
    
    fclose(fp);
    return 1;
}

// ファイルから拡張DNA辞書を読み込む
int load_enhanced_dna_dictionary(EnhancedDNADictionary *dict, const char *filename) {
    if (!dict || !filename) return 0;
    
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;
    
    init_enhanced_dna_dictionary(dict);
    
    char line[MAX_WORD_LEN + MAX_DNA_CODE_LEN + 2]; // コード + | + 単語 + NULL終端
    while (fgets(line, sizeof(line), fp)) {
        // 改行を削除
        line[strcspn(line, "\n")] = 0;
        
        // コードと単語を分離
        char *separator = strchr(line, '|');
        if (!separator) continue;
        
        *separator = '\0';
        char *code = line;
        char *word = separator + 1;
        
        if (strlen(code) < 3) continue;
        
        // タイプとIDを解析
        EnhancedDNAType type;
        switch (code[0]) {
            case 'E': type = ENTITY; break;
            case 'C': type = CONCEPT; break;
            case 'R': type = RESULT; break;
            case 'A': type = ATTRIBUTE; break;
            case 'T': type = TIME; break;
            case 'L': type = LOCATION; break;
            case 'M': type = MANNER; break;
            case 'Q': type = QUANTITY; break;
            default: continue; // 不明なタイプはスキップ
        }
        
        int id = atoi(code + 1);
        
        // 辞書に追加
        if (dict->count < dict->capacity) {
            EnhancedDNAEntry *entry = &dict->entries[dict->count];
            strncpy(entry->code, code, MAX_DNA_CODE_LEN - 1);
            entry->code[MAX_DNA_CODE_LEN - 1] = '\0';
            strncpy(entry->word, word, MAX_WORD_LEN - 1);
            entry->word[MAX_WORD_LEN - 1] = '\0';
            entry->type = type;
            entry->id = id;
            dict->count++;
        }
    }
    
    fclose(fp);
    return 1;
}

// 拡張DNA辞書の内容を表示
void print_enhanced_dna_dictionary(EnhancedDNADictionary *dict) {
    if (!dict) return;
    
    printf("拡張DNA辞書の内容（%d/%d エントリ）:\n", dict->count, dict->capacity);
    printf("----------------------------------------\n");
    printf("コード | タイプ | 単語\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < dict->count; i++) {
        const char *type_str;
        switch (dict->entries[i].type) {
            case ENTITY:    type_str = "主語"; break;
            case CONCEPT:   type_str = "動詞"; break;
            case RESULT:    type_str = "目的語"; break;
            case ATTRIBUTE: type_str = "属性"; break;
            case TIME:      type_str = "時間"; break;
            case LOCATION:  type_str = "場所"; break;
            case MANNER:    type_str = "様態"; break;
            case QUANTITY:  type_str = "数量"; break;
            default:        type_str = "不明"; break;
        }
        
        printf("%s | %s | %s\n", dict->entries[i].code, type_str, dict->entries[i].word);
    }
    printf("----------------------------------------\n");
}