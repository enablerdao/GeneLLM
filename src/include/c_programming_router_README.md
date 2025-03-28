# C言語プログラミングルーター

このモジュールは、C言語プログラミングに関する質問や要求を処理し、適切な応答やコード例を生成するためのルーターシステムです。コンパイラ検証機能と連携して、生成されたコードの品質を確保します。

## 主な機能

1. **パターンマッチング**: 入力テキストを解析し、最適な応答パターンを選択
2. **コード生成**: 要求に応じたC言語コードの自動生成
3. **コンパイラ検証**: 生成されたコードをコンパイラでチェックし、警告やエラーを検出
4. **カスタマイズ**: 入力に基づいてコードテンプレートをカスタマイズ

## 使用例

```c
// ルーターの初期化
CRouter* router = init_c_programming_router();

// 入力に対する応答を生成
char* response = c_router_generate_response(router, "動的メモリ確保の方法を教えて");
printf("%s\n", response);

// ルーターの解放
c_router_free(router);
```

## 対応トピック

現在、以下のC言語トピックに対応しています：

1. **配列操作**: 配列の基本、ソートアルゴリズムなど
2. **ポインタ**: ポインタの基本概念と使用方法
3. **メモリ管理**: 動的メモリ確保と解放
4. **構造体**: 構造体の定義と使用方法
5. **ファイル操作**: ファイルの読み書き

## コード生成機能

以下のコード生成機能を提供しています：

1. **配列のソート**: バブルソートアルゴリズムの実装
2. **線形検索**: 配列内の要素を検索する関数
3. **構造体の定義と使用**: 構造体を定義して操作するコード
4. **ファイル読み込み**: ファイルからデータを読み込むコード
5. **動的メモリ管理**: mallocとfreeを使用したメモリ管理

## コンパイラ検証

生成されたコードは自動的にコンパイラでチェックされ、以下の情報が提供されます：

- コンパイル成功/失敗の状態
- 警告やエラーの詳細
- 問題がある場合の修正案

## 拡張方法

新しいパターンを追加するには、`c_router_add_pattern`関数を使用します：

```c
c_router_add_pattern(router, "パターン", 0.9f, 
    "応答テキスト", 1, "コードテンプレート");
```

## 依存関係

- `compiler_validator.h`: コンパイラ検証機能を提供