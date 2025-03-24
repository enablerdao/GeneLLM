#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    char ending;          // 語尾（五段動詞の場合）
} VerbInfo;

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
    
    // 一段動詞の判定（「〜る」で終わり、直前の文字が「い」段または「え」段）
    if (verb[len-1] == 'る') {
        char prev = verb[len-2];
        if (prev == 'い' || prev == 'き' || prev == 'し' || prev == 'ち' || prev == 'に' || 
            prev == 'ひ' || prev == 'み' || prev == 'り' || 
            prev == 'え' || prev == 'け' || prev == 'せ' || prev == 'て' || prev == 'ね' || 
            prev == 'へ' || prev == 'め' || prev == 'れ') {
            return VERB_TYPE_ICHIDAN;
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
    
    size_t len = strlen(verb);
    
    switch (info->type) {
        case VERB_TYPE_ICHIDAN:
            // 一段動詞: 語幹は「る」を除いた部分
            strncpy(info->stem, verb, len - 1);
            info->stem[len - 1] = '\0';
            info->ending = 'る';
            break;
            
        case VERB_TYPE_GODAN:
            // 五段動詞: 語幹は最後の1文字を除いた部分
            strncpy(info->stem, verb, len - 1);
            info->stem[len - 1] = '\0';
            info->ending = verb[len - 1];
            break;
            
        case VERB_TYPE_IRREGULAR:
            // 不規則動詞: 特殊処理
            if (strcmp(verb, "する") == 0) {
                strcpy(info->stem, "");
                info->ending = 's'; // 特殊マーカー
            } else if (strcmp(verb, "くる") == 0 || strcmp(verb, "来る") == 0) {
                strcpy(info->stem, "");
                info->ending = 'k'; // 特殊マーカー
            }
            break;
            
        default:
            // 不明な場合はそのまま
            strcpy(info->stem, verb);
            info->ending = '\0';
            break;
    }
}

// 五段動詞の活用語尾を取得
const char* get_godan_ending(char base, VerbForm form) {
    static const char* endings[10][10] = {
        // BASIC, MASU, TE, TA, NAI, PASSIVE, CAUSATIVE, POTENTIAL, IMPERATIVE, VOLITIONAL
        {"う", "います", "って", "った", "わない", "われる", "わせる", "える", "え", "おう"},     // う
        {"く", "きます", "いて", "いた", "かない", "かれる", "かせる", "ける", "け", "こう"},     // く
        {"ぐ", "ぎます", "いで", "いだ", "がない", "がれる", "がせる", "げる", "げ", "ごう"},     // ぐ
        {"す", "します", "して", "した", "さない", "される", "させる", "せる", "せ", "そう"},     // す
        {"つ", "ちます", "って", "った", "たない", "たれる", "たせる", "てる", "て", "とう"},     // つ
        {"ぬ", "にます", "んで", "んだ", "なない", "なれる", "なせる", "ねる", "ね", "のう"},     // ぬ
        {"ぶ", "びます", "んで", "んだ", "ばない", "ばれる", "ばせる", "べる", "べ", "ぼう"},     // ぶ
        {"む", "みます", "んで", "んだ", "まない", "まれる", "ませる", "める", "め", "もう"},     // む
        {"る", "ります", "って", "った", "らない", "られる", "らせる", "れる", "れ", "ろう"}      // る
    };
    
    int row;
    switch (base) {
        case 'う': row = 0; break;
        case 'く': row = 1; break;
        case 'ぐ': row = 2; break;
        case 'す': row = 3; break;
        case 'つ': row = 4; break;
        case 'ぬ': row = 5; break;
        case 'ぶ': row = 6; break;
        case 'む': row = 7; break;
        case 'る': row = 8; break;
        default: return NULL;
    }
    
    return endings[row][form];
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
            if (info.ending == 's') { // する
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
            } else if (info.ending == 'k') { // くる/来る
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
#ifdef TEST_VERB_CONJUGATOR
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
    printf("語尾: %c\n", info.ending);
    printf("\n");
    
    printf("活用形一覧:\n");
    for (int i = 0; i <= VERB_FORM_VOLITIONAL; i++) {
        VerbForm form = (VerbForm)i;
        printf("  %s: %s\n", get_verb_form_name(form), conjugate_verb(verb, form));
    }
    
    return 0;
}
#endif