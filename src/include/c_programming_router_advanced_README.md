# 拡張C言語プログラミングルーター

このモジュールは、C言語プログラミングに関する質問や要求を処理し、適切な応答やコード例を生成するための拡張ルーターシステムです。コンパイラ検証機能と連携して、生成されたコードの品質を確保するとともに、難易度別のコンテンツ提供や検索機能を備えています。

## 主な機能

1. **難易度別コンテンツ**：基本、中級、上級の3段階の難易度でコンテンツを提供
2. **タグベースの検索**：関連タグによるコンテンツの検索と分類
3. **コード生成と検証**：要求に応じたC言語コードの自動生成とコンパイラによる検証
4. **コード実行結果表示**：生成されたコードの実行結果を表示
5. **対話型インターフェース**：コマンドベースの対話型インターフェースを提供

## 難易度レベル

### 基本レベル
初心者向けの基本的なC言語の概念とコード例：
- 配列、ポインタ、構造体などの基本概念
- Hello Worldプログラム
- 基本的なアルゴリズム（バブルソート、線形検索など）
- ファイル入出力の基本
- 動的メモリ管理の基本

### 中級レベル
中級者向けの発展的な概念とアルゴリズム：
- 関数ポインタ
- ビット操作
- 再帰関数
- 効率的なアルゴリズム（クイックソート、二分探索など）
- 連結リストなどのデータ構造

### 上級レベル
上級者向けの高度なシステムプログラミング概念：
- マルチスレッドプログラミング
- ソケットプログラミング
- メモリアライメント
- シグナル処理
- 最適化テクニック

## 対話コマンド

以下のコマンドを使用して、ルーターと対話できます：

- `help` - コマンド一覧を表示
- `search` - キーワード検索モード
- `basic` - 基本レベルのトピックを表示
- `intermediate` - 中級レベルのトピックを表示
- `advanced` - 上級レベルのトピックを表示
- `exit`/`quit` - プログラムを終了

## 使用例

```
$ ./c_router_test_advanced

C言語プログラミングルーター - 拡張対話モード
コマンド一覧を表示するには 'help' と入力してください
終了するには 'exit' または 'quit' と入力してください

質問またはコマンドを入力してください: basic

=== 難易度: 基本 のトピック ===
- 配列
タグ: 配列,基本,データ構造

- ポインタ
タグ: ポインタ,基本,メモリ

...

質問またはコマンドを入力してください: クイックソート

=== 応答 ===
以下はクイックソートアルゴリズムを実装したC言語のコード例です：

```c
#include <stdio.h>

// 配列の2要素を交換する関数
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}
...
```

## 拡張機能

1. **タグベースのナビゲーション**：関連タグを使用して関連コンテンツを探索
2. **難易度フィルタリング**：ユーザーのスキルレベルに合わせたコンテンツ提供
3. **コード実行結果**：生成されたコードの実行結果を表示（可能な場合）
4. **補足情報**：難易度に応じた追加の説明や注意点を提供

## 依存関係

- `compiler_validator.h`: コンパイラ検証機能を提供
- `c_programming_router.h`: 基本的なルーティング機能を提供

## 今後の展望

1. **インタラクティブなコード編集**：生成されたコードをその場で編集して再実行
2. **プロジェクトテンプレート**：完全なプロジェクト構造の生成
3. **パフォーマンス分析**：生成されたコードのパフォーマンス分析
4. **コード最適化提案**：より効率的なコードへの改善提案
5. **マルチ言語対応**：C++、Rustなど他の言語への拡張