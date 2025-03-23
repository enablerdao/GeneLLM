#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 動詞の活用形
typedef enum {
    VERB_FORM_BASIC,      // 基本形（辞書形）: 食べる
    VERB_FORM_MASU,       // ます形: 食べます
    VERB_FORM_TE,         // て形: 食べて
    VERB_FORM_TA,         // た形: 食べた
    VERB_FORM_NAI,        // ない形: 食べない
    VERB_FORM_PASSIVE,    // 受身形: 食べられる
    VERB_FORM_CAUSATIVE,  // 使役形: 食べさせる
    VERB_FORM_POTENTIAL,  // 可能形: 食べられる
    VERB_FORM_IMPERATIVE, // 命令形: 食べろ/食べよ
    VERB_FORM_VOLITIONAL  // 意志形: 食べよう
} VerbForm;

// 動詞の種類
typedef enum {
    VERB_TYPE_GODAN,      // 五段動詞: 書く、読む
    VERB_TYPE_ICHIDAN,    // 一段動詞: 食べる、見る
    VERB_TYPE_IRREGULAR,  // 不規則動詞: する、来る
    VERB_TYPE_UNKNOWN     // 不明
} VerbType;

// 動詞の活用情報
typedef struct {
    char basic[256];      // 基本形
    VerbType type;        // 動詞の種類
    char stem[256];       // 語幹
    char ending[8];       // 語尾（UTF-8対応のため文字列として扱う）
} VerbInfo;

// UTF-8文字列の最後の文字を取得
bool get_last_utf8_char(const char* str, char* last_char) {
    if (!str || !*str) {
        *last_char = '\0';
        return false;
    }
    
    size_t len = strlen(str);
    const char* p = str + len;
    
    // UTF-8の先頭バイトを見つけるまで戻る
    while (p > str) {
        p--;
        // UTF-8の先頭バイトは0xxxxxxx または 11xxxxxx
        if ((*p & 0x80) == 0 || (*p & 0xC0) == 0xC0) {
            strcpy(last_char, p);
            return true;
        }
    }
    
    // 最初の文字が先頭バイト
    strcpy(last_char, p);
    return true;
}

// UTF-8文字列の最後の文字を除いた部分を取得
void get_utf8_except_last(const char* str, char* result, size_t result_size) {
    if (!str || !*str) {
        *result = '\0';
        return;
    }
    
    size_t len = strlen(str);
    const char* p = str + len;
    
    // UTF-8の先頭バイトを見つけるまで戻る
    while (p > str) {
        p--;
        // UTF-8の先頭バイトは0xxxxxxx または 11xxxxxx
        if ((*p & 0x80) == 0 || (*p & 0xC0) == 0xC0) {
            size_t stem_len = p - str;
            if (stem_len < result_size - 1) {
                strncpy(result, str, stem_len);
                result[stem_len] = '\0';
            }
            return;
        }
    }
    
    // 最初の文字が先頭バイト（つまり1文字だけの文字列）
    *result = '\0';
}

// 動詞の種類を判定
VerbType determine_verb_type(const char* verb) {
    size_t len = strlen(verb);
    
    // 長さが2未満の場合は判定不能
    if (len < 2) {
        return VERB_TYPE_UNKNOWN;
    }
    
    // 不規則動詞の判定
    if (strcmp(verb, "する") == 0 || 
        strcmp(verb, "くる") == 0 || 
        strcmp(verb, "来る") == 0) {
        return VERB_TYPE_IRREGULAR;
    }
    
    // 最後の文字を取得
    char last_char[8] = {0};
    if (!get_last_utf8_char(verb, last_char)) {
        return VERB_TYPE_UNKNOWN;
    }
    
    // 「る」で終わるか確認
    if (strcmp(last_char, "る") == 0) {
        // 最後から2番目の文字を取得
        char stem[256] = {0};
        get_utf8_except_last(verb, stem, sizeof(stem));
        
        char second_last[8] = {0};
        if (get_last_utf8_char(stem, second_last)) {
            // 一段動詞の判定（「〜る」で終わり、直前の文字が「い」段または「え」段）
            const char* i_dan[] = {"い", "き", "し", "ち", "に", "ひ", "み", "り", "ぎ", "じ", "び", "ぴ"};
            const char* e_dan[] = {"え", "け", "せ", "て", "ね", "へ", "め", "れ", "げ", "じぇ", "べ", "ぺ"};
            
            for (size_t i = 0; i < sizeof(i_dan) / sizeof(i_dan[0]); i++) {
                if (strcmp(second_last, i_dan[i]) == 0) {
                    return VERB_TYPE_ICHIDAN;
                }
            }
            
            for (size_t i = 0; i < sizeof(e_dan) / sizeof(e_dan[0]); i++) {
                if (strcmp(second_last, e_dan[i]) == 0) {
                    return VERB_TYPE_ICHIDAN;
                }
            }
        }
    }
    
    // 上記以外は五段動詞と判定
    return VERB_TYPE_GODAN;
}

// 動詞の情報を解析
void analyze_verb(const char* verb, VerbInfo* info) {
    strncpy(info->basic, verb, sizeof(info->basic) - 1);
    info->basic[sizeof(info->basic) - 1] = '\0';
    
    info->type = determine_verb_type(verb);
    
    switch (info->type) {
        case VERB_TYPE_ICHIDAN:
            // 一段動詞: 語幹は「る」を除いた部分
            get_utf8_except_last(verb, info->stem, sizeof(info->stem));
            strcpy(info->ending, "る");
            break;
            
        case VERB_TYPE_GODAN:
            // 五段動詞: 語幹は最後の1文字を除いた部分
            get_utf8_except_last(verb, info->stem, sizeof(info->stem));
            get_last_utf8_char(verb, info->ending);
            break;
            
        case VERB_TYPE_IRREGULAR:
            // 不規則動詞: 特殊処理
            if (strcmp(verb, "する") == 0) {
                strcpy(info->stem, "");
                strcpy(info->ending, "する"); // 特殊マーカー
            } else if (strcmp(verb, "くる") == 0 || strcmp(verb, "来る") == 0) {
                strcpy(info->stem, "");
                strcpy(info->ending, "くる"); // 特殊マーカー
            }
            break;
            
        default:
            // 不明な場合はそのまま
            strcpy(info->stem, verb);
            info->ending[0] = '\0';
            break;
    }
}

// 五段動詞の活用語尾を取得
const char* get_godan_ending(const char* base, VerbForm form) {
    // 行ごとの活用語尾
    static const char* u_row[] = {"う", "います", "って", "った", "わない", "われる", "わせる", "える", "え", "おう"};
    static const char* k_row[] = {"く", "きます", "いて", "いた", "かない", "かれる", "かせる", "ける", "け", "こう"};
    static const char* g_row[] = {"ぐ", "ぎます", "いで", "いだ", "がない", "がれる", "がせる", "げる", "げ", "ごう"};
    static const char* s_row[] = {"す", "します", "して", "した", "さない", "される", "させる", "せる", "せ", "そう"};
    static const char* t_row[] = {"つ", "ちます", "って", "った", "たない", "たれる", "たせる", "てる", "て", "とう"};
    static const char* n_row[] = {"ぬ", "にます", "んで", "んだ", "なない", "なれる", "なせる", "ねる", "ね", "のう"};
    static const char* b_row[] = {"ぶ", "びます", "んで", "んだ", "ばない", "ばれる", "ばせる", "べる", "べ", "ぼう"};
    static const char* m_row[] = {"む", "みます", "んで", "んだ", "まない", "まれる", "ませる", "める", "め", "もう"};
    static const char* r_row[] = {"る", "ります", "って", "った", "らない", "られる", "らせる", "れる", "れ", "ろう"};
    
    const char** row = NULL;
    
    if (strcmp(base, "う") == 0) row = u_row;
    else if (strcmp(base, "く") == 0) row = k_row;
    else if (strcmp(base, "ぐ") == 0) row = g_row;
    else if (strcmp(base, "す") == 0) row = s_row;
    else if (strcmp(base, "つ") == 0) row = t_row;
    else if (strcmp(base, "ぬ") == 0) row = n_row;
    else if (strcmp(base, "ぶ") == 0) row = b_row;
    else if (strcmp(base, "む") == 0) row = m_row;
    else if (strcmp(base, "る") == 0) row = r_row;
    else return NULL;
    
    return row[form];
}

// 動詞を活用させる
char* conjugate_verb(const char* verb, VerbForm form) {
    static char result[256];
    VerbInfo info;
    
    analyze_verb(verb, &info);
    
    switch (info.type) {
        case VERB_TYPE_ICHIDAN:
            // 一段動詞の活用
            switch (form) {
                case VERB_FORM_BASIC:
                    snprintf(result, sizeof(result), "%s", verb);
                    break;
                case VERB_FORM_MASU:
                    snprintf(result, sizeof(result), "%sます", info.stem);
                    break;
                case VERB_FORM_TE:
                    snprintf(result, sizeof(result), "%sて", info.stem);
                    break;
                case VERB_FORM_TA:
                    snprintf(result, sizeof(result), "%sた", info.stem);
                    break;
                case VERB_FORM_NAI:
                    snprintf(result, sizeof(result), "%sない", info.stem);
                    break;
                case VERB_FORM_PASSIVE:
                    snprintf(result, sizeof(result), "%sられる", info.stem);
                    break;
                case VERB_FORM_CAUSATIVE:
                    snprintf(result, sizeof(result), "%sさせる", info.stem);
                    break;
                case VERB_FORM_POTENTIAL:
                    snprintf(result, sizeof(result), "%sられる", info.stem);
                    break;
                case VERB_FORM_IMPERATIVE:
                    snprintf(result, sizeof(result), "%sろ", info.stem);
                    break;
                case VERB_FORM_VOLITIONAL:
                    snprintf(result, sizeof(result), "%sよう", info.stem);
                    break;
                default:
                    snprintf(result, sizeof(result), "%s", verb);
                    break;
            }
            break;
            
        case VERB_TYPE_GODAN:
            // 五段動詞の活用
            const char* ending = get_godan_ending(info.ending, form);
            if (ending) {
                snprintf(result, sizeof(result), "%s%s", info.stem, ending);
            } else {
                snprintf(result, sizeof(result), "%s", verb);
            }
            break;
            
        case VERB_TYPE_IRREGULAR:
            // 不規則動詞の活用
            if (strcmp(info.ending, "する") == 0) { // する
                switch (form) {
                    case VERB_FORM_BASIC:
                        strcpy(result, "する");
                        break;
                    case VERB_FORM_MASU:
                        strcpy(result, "します");
                        break;
                    case VERB_FORM_TE:
                        strcpy(result, "して");
                        break;
                    case VERB_FORM_TA:
                        strcpy(result, "した");
                        break;
                    case VERB_FORM_NAI:
                        strcpy(result, "しない");
                        break;
                    case VERB_FORM_PASSIVE:
                        strcpy(result, "される");
                        break;
                    case VERB_FORM_CAUSATIVE:
                        strcpy(result, "させる");
                        break;
                    case VERB_FORM_POTENTIAL:
                        strcpy(result, "できる");
                        break;
                    case VERB_FORM_IMPERATIVE:
                        strcpy(result, "しろ");
                        break;
                    case VERB_FORM_VOLITIONAL:
                        strcpy(result, "しよう");
                        break;
                    default:
                        strcpy(result, "する");
                        break;
                }
            } else if (strcmp(info.ending, "くる") == 0) { // くる/来る
                switch (form) {
                    case VERB_FORM_BASIC:
                        strcpy(result, "来る");
                        break;
                    case VERB_FORM_MASU:
                        strcpy(result, "来ます");
                        break;
                    case VERB_FORM_TE:
                        strcpy(result, "来て");
                        break;
                    case VERB_FORM_TA:
                        strcpy(result, "来た");
                        break;
                    case VERB_FORM_NAI:
                        strcpy(result, "来ない");
                        break;
                    case VERB_FORM_PASSIVE:
                        strcpy(result, "来られる");
                        break;
                    case VERB_FORM_CAUSATIVE:
                        strcpy(result, "来させる");
                        break;
                    case VERB_FORM_POTENTIAL:
                        strcpy(result, "来られる");
                        break;
                    case VERB_FORM_IMPERATIVE:
                        strcpy(result, "来い");
                        break;
                    case VERB_FORM_VOLITIONAL:
                        strcpy(result, "来よう");
                        break;
                    default:
                        strcpy(result, "来る");
                        break;
                }
            } else {
                snprintf(result, sizeof(result), "%s", verb);
            }
            break;
            
        default:
            // 不明な場合はそのまま
            snprintf(result, sizeof(result), "%s", verb);
            break;
    }
    
    return result;
}

// 動詞の種類を文字列で取得
const char* get_verb_type_name(VerbType type) {
    switch (type) {
        case VERB_TYPE_GODAN:
            return "五段動詞";
        case VERB_TYPE_ICHIDAN:
            return "一段動詞";
        case VERB_TYPE_IRREGULAR:
            return "不規則動詞";
        default:
            return "不明";
    }
}

// 活用形を文字列で取得
const char* get_verb_form_name(VerbForm form) {
    switch (form) {
        case VERB_FORM_BASIC:
            return "基本形";
        case VERB_FORM_MASU:
            return "ます形";
        case VERB_FORM_TE:
            return "て形";
        case VERB_FORM_TA:
            return "た形";
        case VERB_FORM_NAI:
            return "ない形";
        case VERB_FORM_PASSIVE:
            return "受身形";
        case VERB_FORM_CAUSATIVE:
            return "使役形";
        case VERB_FORM_POTENTIAL:
            return "可能形";
        case VERB_FORM_IMPERATIVE:
            return "命令形";
        case VERB_FORM_VOLITIONAL:
            return "意志形";
        default:
            return "不明";
    }
}

// テスト用のメイン関数
#ifdef TEST_UTF8_VERB_CONJUGATOR
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("使用法: %s <動詞>\n", argv[0]);
        printf("例: %s 食べる\n", argv[0]);
        return 1;
    }
    
    const char* verb = argv[1];
    VerbInfo info;
    
    analyze_verb(verb, &info);
    
    printf("動詞: %s\n", verb);
    printf("種類: %s\n", get_verb_type_name(info.type));
    printf("語幹: %s\n", info.stem);
    printf("語尾: %s\n", info.ending);
    printf("\n");
    
    printf("活用形一覧:\n");
    for (int i = 0; i <= VERB_FORM_VOLITIONAL; i++) {
        VerbForm form = (VerbForm)i;
        printf("  %s: %s\n", get_verb_form_name(form), conjugate_verb(verb, form));
    }
    
    return 0;
}
#endif