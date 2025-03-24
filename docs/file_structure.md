# ファイル構造ガイド

このドキュメントでは、GeneLLMプロジェクトのディレクトリ構造と各ファイルの役割について詳細に説明します。

## ルートディレクトリ

```
/
├── bin/           - コンパイル済みバイナリ
├── data/          - データファイル
├── knowledge/     - 知識関連ファイル
├── logs/          - ログファイル
├── src/           - ソースコード
├── benchmark/     - ベンチマークツール
├── docs/          - ドキュメント
├── LICENSE        - ライセンスファイル
├── README.md      - プロジェクト概要
├── README_simple.md - 簡易版README
├── quick_start.sh - クイックスタートスクリプト
└── .gitignore     - Gitの無視ファイル設定
```

## データディレクトリ (`/data`)

```
data/
├── models/             - モデルデータ
│   └── embeddings/     - 単語埋め込みモデル
├── vector_db/          - ベクトルデータベース
│   ├── index/          - 検索インデックス
│   └── cache/          - キャッシュデータ
├── word_vectors.dat    - 単語ベクトルデータ
└── learning_db.txt     - 学習データベース
```

### 主要ファイル

- **word_vectors.dat**: 単語のベクトル表現を格納したバイナリファイル。各単語は固定長の浮動小数点数配列として表現されています。
- **learning_db.txt**: 過去の質問と回答のペアを保存するテキストファイル。各行は「質問|回答|確信度」の形式で記録されています。

## 知識ディレクトリ (`/knowledge`)

```
knowledge/
├── base/               - 知識ベース
│   ├── ai/             - AI関連の知識
│   ├── culture/        - 文化関連の知識
│   └── system/         - システム関連の知識
├── docs/               - トピック別知識ファイル
│   ├── programming_corpus.txt  - プログラミング関連の知識
│   ├── math_corpus.txt         - 数学関連の知識
│   └── ...                     - その他のトピック
└── text/               - テキストコーパス
    ├── japanese_words.txt      - 日本語単語リスト
    └── wikipedia/              - Wikipediaデータ
        ├── wikipedia_plain.txt     - プレーンテキスト
        └── wikipedia_intros.txt    - 記事導入部
```

### 知識ベースファイル形式

`knowledge/base`ディレクトリ内のファイルはマークダウン形式で、以下の構造を持ちます：

```markdown
---
category: カテゴリ名
tags: タグ1, タグ2, タグ3
created_at: 2023-03-23 12:00:00
updated_at: 2023-03-23 12:00:00
---

# タイトル

コンテンツ本文...
```

## ソースコードディレクトリ (`/src`)

```
src/
├── analyzers/          - テキスト解析モジュール
│   ├── simple_analyzer.c       - 基本的なテキスト解析
│   └── morphological.c         - 形態素解析
├── compressors/        - データ圧縮モジュール
│   └── dna_compressor.c        - DNA風圧縮アルゴリズム
├── generators/         - 文生成モジュール
│   └── graph_generator.c       - グラフベース生成器
├── include/            - 共通ヘッダとユーティリティ
│   ├── knowledge_manager.c     - 知識ベース管理
│   ├── knowledge_manager.h     - 知識ベース管理ヘッダ
│   ├── learning_module.c       - 学習モジュール
│   ├── learning_module.h       - 学習モジュールヘッダ
│   ├── word_loader.c           - 単語読み込み
│   ├── word_loader.h           - 単語読み込みヘッダ
│   ├── duckduckgo_search.h     - 検索API連携
│   ├── llm_interface.c         - 外部LLM連携
│   ├── improved_response_generator.c - 応答生成器
│   ├── improved_response_generator.h - 応答生成器ヘッダ
│   └── vector_db.c             - ベクトルDB操作
├── routers/             - ルーティングモジュール
│   └── router_model.c          - ルーターモデル
├── vector_search/       - ベクトル検索モジュール
│   ├── vector_search.c         - ベクトル検索実装
│   └── vector_search.h         - ベクトル検索ヘッダ
├── improved_router_model.c     - 改良版ルーターモデル
└── main.c                      - メインプログラム
```

### 主要ソースファイル

- **main.c**: プログラムのエントリーポイント。コマンドライン引数の解析、モード選択、全体の制御フローを担当します。
- **knowledge_manager.c**: 知識ベースの管理を担当。ドキュメントの読み込み、検索、保存機能を提供します。
- **vector_search.c**: ベクトル検索エンジンの実装。テキストのベクトル表現と類似度計算を行います。
- **learning_module.c**: 過去の質問と回答を記録し、類似質問の検索機能を提供します。
- **improved_router_model.c**: 入力テキストを分析し、適切なエージェントに処理を振り分けるルーターモデルの改良版です。

## バイナリディレクトリ (`/bin`)

```
bin/
├── main                - メインプログラム（gllmのコピー）
├── simple_analyzer     - 単体テスト用の解析ツール
├── dna_compressor      - DNA圧縮ツール
├── vector_search       - ベクトル検索ツール
├── graph_generator     - グラフ生成ツール
└── router_model        - ルーターモデルテスト
```

これらのバイナリは、対応するソースファイルからビルドされたツールです。単体テストや特定の機能のデモに使用できます。

## ログディレクトリ (`/logs`)

```
logs/
├── questions/          - 質問ログ
├── responses/          - 応答ログ
└── errors/             - エラーログ
```

このディレクトリには、システムの実行中に生成されるログファイルが保存されます。質問と応答の履歴、エラー情報などが含まれます。

## ベンチマークディレクトリ (`/benchmark`)

```
benchmark/
├── run_benchmark.sh    - ベンチマーク実行スクリプト
├── test_cases/         - テストケース
├── results/            - ベンチマーク結果
└── README.md           - ベンチマーク説明
```

ベンチマークツールは、GeneLLMの性能を評価するためのスクリプトとテストケースを提供します。

## ドキュメントディレクトリ (`/docs`)

```
docs/
├── main_program.md         - メインプログラム詳細
├── knowledge_system.md     - 知識管理システム詳細
├── vector_search.md        - ベクトル検索エンジン詳細
├── learning_module.md      - 学習モジュール詳細
├── router_model.md         - ルーターモデル詳細
├── dependencies.md         - 依存関係インストールガイド
├── usage_guide.md          - 使用方法ガイド
├── file_structure.md       - ファイル構造ガイド（このファイル）
├── architecture_overview.md - アーキテクチャ概要
└── knowledge_base_guide.md - 知識ベース拡張ガイド
```

これらのドキュメントファイルは、GeneLLMの各コンポーネントと使用方法に関する詳細な情報を提供します。

## ファイル依存関係

主要なファイル間の依存関係は以下の通りです：

```
main.c
 ├── include/knowledge_manager.c
 │    └── vector_search/vector_search.c
 ├── include/learning_module.c
 ├── include/word_loader.c
 ├── include/llm_interface.c
 ├── include/duckduckgo_search.h
 └── improved_router_model.c
      └── include/improved_response_generator.c
```

## ファイル拡張のガイドライン

新しいファイルを追加する場合は、以下のガイドラインに従ってください：

1. **モジュール分割**: 機能ごとに適切なディレクトリに配置します
2. **命名規則**: スネークケース（例: `word_loader.c`）を使用します
3. **ヘッダファイル**: 公開APIにはヘッダファイルを作成します
4. **ドキュメント**: 新しいモジュールには対応するドキュメントを作成します

## カスタマイズのヒント

GeneLLMのファイル構造をカスタマイズする際のヒント：

1. **知識ベースの拡張**: `knowledge/base`ディレクトリに新しいカテゴリやファイルを追加します
2. **新しいエージェントの追加**: `src/agents`ディレクトリを作成し、専門エージェントを実装します
3. **ベクトル検索の改良**: `src/vector_search`ディレクトリに新しいアルゴリズムを追加します
4. **UI拡張**: `src/ui`ディレクトリを作成し、ユーザーインターフェースを実装します