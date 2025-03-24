# メインプログラム詳細

`src/main.c` はGeneLLMのエントリーポイントであり、プログラムの実行フローを制御します。

## 主要機能

### 1. 初期化処理

```c
int main(int argc, char* argv[]) {
    // ロケールを設定（日本語対応）
    setlocale(LC_ALL, "");
    
    // 乱数の初期化
    srand((unsigned int)time(NULL));
    
    // エージェントとトピックを初期化
    init_agents();
    init_topics();
    init_topic_relations();
    init_inference_rules();
    
    // ベクトルデータベースを初期化
    init_global_vector_db_impl();
    
    // 学習データベースを初期化
    learning_db = learning_db_init("data/learning_db.txt");
    
    // 知識ベースを初期化
    knowledge_base = knowledge_base_init("logs");
    
    // ...
}
```

### 2. コマンドライン引数の解析

```c
// コマンドライン引数のチェック
if (i < argc && strcmp(argv[i], "-d") == 0) {
    debug_mode = true;
    printf("デバッグモードが有効になりました\n");
    i++;
}

// モードを判定
if (strcmp(argv[i], "-f") == 0) {
    // ファイルモード
    // ...
} else if (strcmp(argv[i], "-i") == 0) {
    // 対話モード
    // ...
} else {
    // 単一テキストモード
    // ...
}
```

### 3. テキスト処理フロー

```c
// テキストをルーティング
char response[MAX_RESPONSE_LEN];
route_text(text_buffer, response);

// 応答を表示
printf("%s\n", response);
```

### 4. 対話モード

```c
// 対話モード
printf("対話モードを開始します。終了するには 'exit' と入力してください。\n");

while (1) {
    // 入力を受け取る
    printf("> ");
    if (!fgets(text_buffer, MAX_TEXT_LEN, stdin)) {
        break;
    }
    
    // 改行を削除
    size_t len = strlen(text_buffer);
    if (len > 0 && text_buffer[len-1] == '\n') {
        text_buffer[len-1] = '\0';
    }
    
    // 終了コマンドをチェック
    if (strcmp(text_buffer, "exit") == 0) {
        break;
    }
    
    // テキストをルーティング
    char response[MAX_RESPONSE_LEN];
    route_text(text_buffer, response);
    
    // 応答を表示
    printf("%s\n", response);
}
```

### 5. 終了処理

```c
// 終了処理
if (learning_db) {
    learning_db_free(learning_db);
}

if (knowledge_base) {
    knowledge_base_free(knowledge_base);
}

return 0;
```

## 実行フロー

1. プログラムが起動され、初期化処理が実行されます
2. コマンドライン引数が解析され、実行モードが決定されます
3. 指定されたモード（対話モード、ファイルモード、単一テキストモード）に応じた処理が実行されます
4. テキスト入力が `route_text()` 関数に渡され、適切な応答が生成されます
5. 応答がユーザーに表示されます
6. 対話モードの場合は、ユーザーが「exit」と入力するまで処理が繰り返されます
7. 終了処理が実行され、リソースが解放されます

## デバッグモード

デバッグモード (`-d` オプション) を有効にすると、以下の追加情報が表示されます：

- トークン解析結果
- エージェント評価スコア
- トピック評価スコア
- ベクトル検索結果
- 推論ルールの適用状況

## 拡張ポイント

メインプログラムは以下の点で拡張可能です：

1. 新しいコマンドラインオプションの追加
2. 追加の実行モードの実装
3. 初期化処理の拡張（追加のモジュールやリソースの読み込み）
4. 応答生成プロセスのカスタマイズ
5. 外部サービスとの連携機能の追加