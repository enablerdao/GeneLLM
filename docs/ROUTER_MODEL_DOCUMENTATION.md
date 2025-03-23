# 拡張型ルーターモデル（Improved Router Model）詳細ドキュメント

このドキュメントでは、`improved_router_model.c`の詳細な仕組みと使い方について説明します。

## 1. システム概要

拡張型ルーターモデルは、C言語で実装された軽量な自然言語処理システムです。以下の主要機能を備えています：

1. **エージェントルーティング**: 入力テキストを分析し、適切なエージェントに振り分け
2. **トピック認識**: 入力テキストからトピック（料理、健康、投資など）を識別
3. **推論エンジン**: 条件に基づいて適切な回答を生成
4. **知識ベース**: 各トピックに関する基本的な知識を保持
5. **外部LLM連携**: 内部知識で対応できない質問に対して外部のLLMに問い合わせ

## 2. アーキテクチャ詳細

### 2.1 エージェントシステム

システムは以下の6種類のエージェントを持っています：

| エージェント | 役割 | 例 |
|------------|------|-----|
| 挨拶エージェント | 挨拶や礼儀的な表現に対応 | 「こんにちは」「ありがとう」 |
| 質問エージェント | 質問に回答 | 「料理の基本は何ですか？」 |
| 命令エージェント | 指示や依頼に対応 | 「料理のコツを教えて」 |
| ステートメントエージェント | 事実や意見の表明に対応 | 「料理は楽しいです」 |
| 感情エージェント | 感情表現に対応 | 「料理が上手くできて嬉しい」 |
| フォールバックエージェント | 他のエージェントが対応できない場合 | その他の入力 |

各エージェントは入力テキストに対するスコアを計算し、最も高いスコアを持つエージェントが応答を生成します。

### 2.2 トピック認識システム

システムは以下のトピックを認識できます：

1. 幸せ（happiness）
2. 愛（love）
3. 健康（health）
4. 仕事（work）
5. 学習（learning）
6. 数学（math）
7. 物理（physics）
8. プログラミング（programming）
9. ギター（guitar）
10. サバイバル（survival）
11. 量子コンピュータ（quantum）
12. 料理（cooking）
13. フィットネス（fitness）
14. 旅行（travel）
15. 金融・投資（finance）
16. エンターテイメント（entertainment）

各トピックには関連するキーワードとその重みが設定されており、入力テキストに含まれるキーワードに基づいてスコアが計算されます。

### 2.3 推論エンジン

推論エンジンは条件と結論のペアで構成されるルールを持っています。条件は「AND」で結合された2つのキーワードで、入力テキストにこれらのキーワードが含まれている場合、対応する結論が応答として生成されます。

例：
- 条件: "料理 AND 基本"
- 結論: "料理の基本は、食材の特性を理解し、適切な調理法と調味料を選ぶことです。包丁の扱いや火加減の調整も重要なスキルです。"

各ルールには確信度（0.0〜1.0）が設定されており、複数のルールが適用可能な場合は、最も確信度の高いルールが選択されます。

### 2.4 外部LLM連携

システムは内部知識で対応できない質問に対して、外部のLLM（大規模言語モデル）に問い合わせる機能を持っています。この機能は以下のコンポーネントで構成されています：

#### 2.4.1 C言語インターフェース（llm_interface.c）

```c
int query_external_llm(const char* query, char* response, size_t max_length);
```

この関数は、外部コマンドを実行し、その結果を取得するためのインターフェースを提供します。

#### 2.4.2 Pythonスクリプト（llm_query.py）

```python
def query_llm(prompt: str, model: str = "gpt-3.5-turbo") -> Optional[str]:
    # LLM APIと通信して回答を取得
    
def simulate_llm_response(prompt: str) -> str:
    # API キーがない場合に簡易的な回答を生成
```

このスクリプトは、実際にLLM APIと通信するためのものです。

## 3. 処理フロー

システムの処理フローは以下の通りです：

```
入力テキスト
    ↓
形態素解析（MeCab）
    ↓
エージェント選択
    ↓
トピック選択
    ↓
エージェントハンドラ実行
    ↓
  ┌─────────────────┐
  │ 推論ルール適用  │
  │   成功 → 応答  │
  │   失敗 ↓      │
  ├─────────────────┤
  │ 知識ベース検索  │
  │   成功 → 応答  │
  │   失敗 ↓      │
  ├─────────────────┤
  │ 外部LLM問い合わせ │
  │   成功 → 応答  │
  │   失敗 ↓      │
  ├─────────────────┤
  │ フォールバック応答 │
  └─────────────────┘
    ↓
応答テキスト
```

## 4. 外部LLM連携の詳細

### 4.1 連携フロー

```
C言語コード
    ↓
query_external_llm() 関数呼び出し
    ↓
コマンド構築: python3 llm_query.py "質問文"
    ↓
popen() でコマンド実行
    ↓
llm_query.py 実行
    ↓
  ┌─────────────────┐
  │ APIキー確認    │
  │   あり → API呼び出し │
  │   なし → シミュレーション │
  └─────────────────┘
    ↓
応答テキスト
    ↓
C言語コードに戻る
```

### 4.2 シミュレーションモード

APIキーが設定されていない場合、`simulate_llm_response()` 関数が呼び出され、質問のキーワードに基づいて簡易的な応答が生成されます。

例：
- 「料理」を含む質問 → 料理に関する一般的な回答
- 「投資」を含む質問 → 投資に関する一般的な回答

## 5. 使用方法

### 5.1 コンパイル

```bash
gcc -Wall -Wextra -std=c99 -o improved_router_model improved_router_model.c -lmecab
```

### 5.2 実行

```bash
./improved_router_model "あなたの質問や文章をここに入力"
```

### 5.3 外部LLM連携の設定

外部LLMを使用するには、以下のいずれかの方法でAPIキーを設定します：

1. コマンドライン引数で指定：
```bash
./improved_router_model --api-key "your_api_key_here" "質問文"
```

2. 環境変数で指定：
```bash
export OPENAI_API_KEY="your_api_key_here"
./improved_router_model "質問文"
```

3. 対話的に入力：
プログラム実行時にAPIキーの入力を促されます。

4. 対話モード中に設定：
```
> /api-key your_api_key_here
```

対話モードでは以下のAPIキー関連コマンドが使用できます：
- `/api-key <APIキー>` - APIキーを設定します
- `/api-status` - APIキーの設定状態を確認します
- `/api-help` - APIキー関連コマンドのヘルプを表示します

APIキーが設定されていない場合は、シミュレーションモードで動作します。

## 6. コードの主要部分

### 6.1 エージェント選択

```c
// エージェントを選択
int select_agent(const char* text, Agent* agents, int agent_count) {
    int best_agent = -1;
    float best_score = -1.0f;
    
    for (int i = 0; i < agent_count; i++) {
        float score = calculate_agent_score(text, &agents[i]);
        printf("エージェント '%s' のスコア: %.4f (しきい値: %.4f)\n", 
               agents[i].name, score, agents[i].threshold);
        
        if (score >= agents[i].threshold && score > best_score) {
            best_score = score;
            best_agent = i;
        }
    }
    
    return best_agent;
}
```

### 6.2 トピック選択

```c
// トピックを選択
int select_topic(const char* text, Topic* topics, int topic_count) {
    int best_topic = -1;
    float best_score = -1.0f;
    
    for (int i = 0; i < topic_count; i++) {
        float score = calculate_topic_score(text, &topics[i]);
        printf("トピック '%s' のスコア: %.4f\n", topics[i].name, score);
        
        if (score > best_score) {
            best_score = score;
            best_topic = i;
        }
    }
    
    return best_topic;
}
```

### 6.3 推論ルール適用

```c
// 推論ルールを適用
char apply_inference_rules(const char* text, char* response) {
    printf("入力テキスト: '%s'\n", text);
    printf("推論ルール数: %d\n", rule_count);
    
    int best_rule = -1;
    float best_confidence = 0.0f;
    
    for (int i = 0; i < rule_count; i++) {
        // ルールの条件をチェック
        if (check_rule_condition(text, rules[i].condition)) {
            // より高い確信度のルールを選択
            if (rules[i].confidence > best_confidence) {
                best_rule = i;
                best_confidence = rules[i].confidence;
            }
        }
    }
    
    if (best_rule >= 0) {
        printf("最終的に適用されたルールの確信度: %.2f\n", best_confidence);
        printf("推論ルールが適用されました: %s\n", rules[best_rule].conclusion);
        strcpy(response, rules[best_rule].conclusion);
        return 1;  // 成功
    }
    
    return 0;  // 失敗
}
```

### 6.4 外部LLM問い合わせ

```c
// 外部LLMを使用する場合
if (USE_EXTERNAL_LLM) {
    printf("外部LLMに問い合わせます...\n");
    char llm_response[MAX_RESPONSE_LEN];
    if (query_external_llm(text, llm_response, MAX_RESPONSE_LEN)) {
        strcpy(response, llm_response);
        return 1;
    }
    printf("外部LLMからの応答取得に失敗しました\n");
}
```

## 7. 拡張方法

### 7.1 新しいトピックの追加

1. `init_topics()` 関数内に新しいトピックを追加

```c
// 新しいトピック
add_topic("new_topic");
add_topic_pattern(topic_id, "キーワード1", "品詞", ENTITY, 1.0f);
add_topic_pattern(topic_id, "キーワード2", "品詞", ENTITY, 0.9f);
// ...

// 知識ファイルを読み込む
load_knowledge_from_file("new_topic_corpus.txt", topic_id);
```

2. 知識コーパスファイルを作成（例: `new_topic_corpus.txt`）

### 7.2 新しい推論ルールの追加

`init_inference_rules()` 関数内に新しい推論ルールを追加

```c
// 新しい推論ルール
add_inference_rule("キーワード1 AND キーワード2", "この質問に対する回答です。", 0.9f);
```

## 8. 制限事項

- 形態素解析には MeCab を使用しているため、日本語テキストに最適化されています
- 単純なキーワードマッチングに基づくトピック認識のため、複雑な文脈理解は限定的です
- 推論ルールは静的に定義されており、動的な学習機能はありません
- 外部LLM連携は基本的な実装のため、高度なプロンプト設計やストリーミング応答には対応していません

## 9. 今後の展望

- 文脈理解の改善（会話履歴の保持と参照）
- 動的な学習機能の追加（ユーザーフィードバックに基づくルールの調整）
- 多言語対応の強化
- より高度な外部LLM連携（ストリーミング応答、複数のLLMの使い分けなど）
- 音声入出力インターフェースの追加