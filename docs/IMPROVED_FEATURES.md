# GeneLLM 改良機能

このドキュメントでは、GeneLLMの改良された機能について説明します。

## 拡張された回答データベース

### 概要

回答データベースは、質問と回答のペアを管理し、ユーザーの質問に最も適した回答を見つけるための機能です。改良版では以下の機能が追加されています：

- キーワードベースのマッチング強化
- 関連性スコアによる回答の品質向上
- アクセス頻度に基づく回答の優先順位付け
- 多様なトピックへの対応

### 主要コンポーネント

#### データ構造

```c
typedef struct {
    char question[MAX_QUESTION_LENGTH];
    char answer[MAX_ANSWER_LENGTH];
    double relevance_score;  // 関連性スコア（0.0〜1.0）
    time_t last_accessed;    // 最後にアクセスされた時間
    int access_count;        // アクセス回数
} QAPair;

typedef struct {
    char keyword[MAX_KEYWORD_LENGTH];
    double match_score;
} KeywordMatch;
```

#### キーワードマッチング

特定のキーワードに基づいて質問をマッチングする機能が強化されました。プログラミング関連のキーワード（ポインタ、構造体、配列など）だけでなく、一般的なトピック（柔術、サッカー、料理、音楽など）にも対応しています。

```c
// キーワードを初期化する
void init_keywords() {
    // プログラミング関連キーワード
    strcpy(keywords[keyword_count].keyword, "ポインタ");
    keywords[keyword_count++].match_score = 0.95;
    
    // 一般的なトピックのキーワード
    strcpy(keywords[keyword_count].keyword, "柔術");
    keywords[keyword_count++].match_score = 0.95;
    
    // 他のキーワードも同様に追加...
}
```

#### 類似度計算の改良

単語と文章の類似度計算アルゴリズムが改良され、より正確なマッチングが可能になりました：

- 単語の位置を考慮した類似度計算
- 単語の長さに応じた重み付け
- 先頭文字一致によるボーナス
- 部分文字列マッチングの強化

```c
// 単語の類似度を計算する簡易関数（改良版）
double word_similarity(const char* word1, const char* word2) {
    // 同じ単語なら最高スコア
    if (strcasecmp(word1, word2) == 0) {
        return 1.0;
    }
    
    // 長さが大きく異なる場合は類似度を下げる
    double len_ratio = (double)len1 / len2;
    if (len_ratio > 1.0) len_ratio = 1.0 / len_ratio;
    
    // 共通の文字数をカウント（位置も考慮）
    // ...
    
    // 先頭文字が一致する場合はボーナス
    double prefix_bonus = (tolower(word1[0]) == tolower(word2[0])) ? 0.2 : 0.0;
    
    // 類似度を計算（共通文字の割合、位置の一致、長さの比率を考慮）
    // ...
}
```

#### 関連性スコアとアクセス頻度

回答の関連性スコアとアクセス頻度を考慮することで、より適切な回答を優先的に提供します：

- 関連性スコア：回答の品質や適切さを表す値（0.5〜1.0）
- アクセス頻度：よく使われる回答に対するボーナス
- 最終アクセス時間：最近使われた回答の追跡

```c
// アクセス情報を更新する
void update_access_info(int index) {
    if (index >= 0 && index < answer_db_size) {
        answer_db[index].last_accessed = time(NULL);
        answer_db[index].access_count++;
    }
}

// 質問に対する回答を検索し、類似度スコアと選択された質問も返す
const char* find_answer_with_score(const char* question, double* score) {
    // ...
    
    // 類似度に基づく検索
    for (i = 0; i < answer_db_size; i++) {
        double current_score = sentence_similarity(question, answer_db[i].question);
        
        // 関連性スコアを考慮
        current_score *= answer_db[i].relevance_score;
        
        // アクセス頻度によるボーナス（よく使われる回答を優先）
        double access_bonus = 0.05 * (1.0 - exp(-0.1 * answer_db[i].access_count));
        current_score += access_bonus;
        
        // ...
    }
    
    // ...
}
```

#### フィードバックによる学習

ユーザーからのフィードバックに基づいて関連性スコアを更新する機能が追加されました：

```c
// 関連性スコアを更新する
void update_relevance_score(const char* question, double feedback_score) {
    // 質問に最も類似した回答を見つける
    double sim_score;
    find_answer_with_score(question, &sim_score);
    
    // マッチした質問がある場合、その関連性スコアを更新
    if (matched_question[0] != '\0') {
        for (int i = 0; i < answer_db_size; i++) {
            if (strcmp(answer_db[i].question, matched_question) == 0) {
                // 現在のスコアと新しいフィードバックを組み合わせて更新
                answer_db[i].relevance_score = 
                    answer_db[i].relevance_score * 0.8 + feedback_score * 0.2;
                
                // スコアの範囲を0.5〜1.0に制限
                // ...
                
                break;
            }
        }
    }
}
```

### 新しい機能

#### 回答データベースの永続化

回答データベースを保存および読み込む機能が追加されました：

```c
// 回答データベースを保存する
bool save_answer_db(const char* filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        return false;
    }
    
    for (int i = 0; i < answer_db_size; i++) {
        fprintf(fp, "%s|%s\n", answer_db[i].question, answer_db[i].answer);
    }
    
    fclose(fp);
    return true;
}
```

#### 回答の動的管理

回答の追加と削除を行う機能が追加されました：

```c
// 回答データベースに新しい質問と回答を追加する
bool add_answer(const char* question, const char* answer) {
    // ...
}

// 回答データベースから質問と回答を削除する
bool remove_answer(const char* question) {
    // ...
}
```

## 多様なトピックへの対応

GeneLLMは、プログラミング関連の質問だけでなく、以下のような多様なトピックにも対応できるようになりました：

- 柔術やスポーツなどの格闘技・スポーツ関連
- 料理のレシピ
- 音楽の歴史
- 健康的な生活習慣
- 宇宙の起源
- 人工知能の基本

これらのトピックに関する質問に対して、適切な回答を提供することができます。

## 使用例

```bash
# プログラミング関連の質問
./gllm "ポインタとは何ですか"

# スポーツ関連の質問
./gllm "柔術とは何ですか"

# 料理関連の質問
./gllm "料理のレシピを教えてください"

# 音楽関連の質問
./gllm "音楽の歴史について教えてください"

# 健康関連の質問
./gllm "健康的な生活習慣について教えてください"

# 宇宙関連の質問
./gllm "宇宙の起源について教えてください"

# AI関連の質問
./gllm "人工知能の基本について教えてください"
```

## 今後の改善点

- より多くのトピックへの対応
- 回答の品質向上のための機械学習アルゴリズムの導入
- 複数の言語への対応
- ユーザーインターフェースの改善
- 外部APIとの連携強化
