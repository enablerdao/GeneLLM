# GeneLLM - C言語による超軽量AI実装

![GeneLLM Logo](assets/genellm_logo.png)

GeneLLMは、C言語で実装された軽量な自然言語処理システムです。大規模な深層学習モデルとは異なり、形態素解析と独自のルールベースアプローチを組み合わせることで、最小限のリソースで動作する効率的なAIシステムを提供します。

## 📋 目次

- [インストール方法](#-インストール方法)
- [使い方](#-使い方)
- [プロジェクト概要](#-プロジェクト概要)
- [システムアーキテクチャ](#-システムアーキテクチャ)
- [ディレクトリ構造](#-ディレクトリ構造)
- [ベンチマーク](#-ベンチマーク)
- [ライセンス](#-ライセンス)
- [貢献](#-貢献)

## 🚀 インストール方法

### クイックスタート（推奨）

最も簡単な方法は、クイックスタートスクリプトを使用することです：

```bash
curl -sSL https://raw.githubusercontent.com/enablerdao/GeneLLM/main/quick_start.sh | bash
```

このスクリプトは以下の処理を自動的に行います：
1. 必要な依存関係のインストール（MeCab、libcurl）
2. プロジェクトのビルド
3. 最初のテスト実行

### 手動インストール

リポジトリをクローンして手動でインストールすることもできます：

```bash
# リポジトリのクローン
git clone https://github.com/enablerdao/GeneLLM.git
cd GeneLLM

# 依存関係のインストール（Ubuntu/Debian）
sudo apt-get update
sudo apt-get install -y libmecab-dev mecab mecab-ipadic-utf8 libcurl4-openssl-dev

# ビルド
gcc -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/include/word_loader.c -lmecab -lm -lcurl
```

### 依存関係

- **MeCab**: 日本語形態素解析エンジン
- **libcurl**: ウェブからのデータ取得（オプション）

各OSでのインストール方法は[依存関係インストールガイド](docs/dependencies.md)を参照してください。

## 💬 使い方

GeneLLMは以下の3つのモードで実行できます：

### 1. 単一質問モード

```bash
./gllm "C言語でHello Worldを表示するプログラムを教えてください"
```

### 2. デバッグモード

内部処理の詳細を表示するデバッグモードで実行：

```bash
./gllm -d "自然言語処理とは何ですか？"
```

### 3. 対話モード

継続的な会話を行う対話モードで実行：

```bash
./gllm -i
```

対話モードでは、`exit`と入力することで終了できます。

詳細な使用方法と高度なオプションについては[使用方法ガイド](docs/usage_guide.md)を参照してください。

## 📋 プロジェクト概要

GeneLLMは以下の特徴を持つ軽量AIシステムです：

- **C言語による実装**: 高速で効率的な処理を実現
- **最小限の依存関係**: 外部ライブラリへの依存を最小限に抑えた設計
- **形態素解析ベース**: MeCabを使用した日本語テキスト解析
- **ベクトル検索**: 効率的な類似度計算による関連情報の検索
- **知識ベース管理**: マークダウン形式の知識ファイルを活用
- **学習機能**: 過去の質問と回答を記憶し再利用
- **エージェントベースアーキテクチャ**: 専門化されたエージェントによる処理

GeneLLMは、大規模な計算リソースを必要とせずに基本的な自然言語処理タスクを実行できるように設計されています。教育目的や組み込みシステムでの利用に適しています。

## 🔍 システムアーキテクチャ

GeneLLMは以下の主要コンポーネントから構成されています：

### 1. メインプログラム (`src/main.c`)
- プログラムのエントリーポイント
- コマンドライン引数の解析
- 対話モードと単一クエリモードの処理
- [詳細な説明](docs/main_program.md)

### 2. 知識管理システム (`src/include/knowledge_manager.c`)
- 知識ベースの初期化と管理
- ドキュメントの追加、検索、保存
- ベクトル検索との連携
- [詳細な説明](docs/knowledge_system.md)

### 3. ベクトル検索エンジン (`src/vector_search/vector_search.c`)
- 単語や文のベクトル表現
- 類似度計算（コサイン類似度など）
- 高速な検索アルゴリズム
- [詳細な説明](docs/vector_search.md)

### 4. 学習モジュール (`src/include/learning_module.c`)
- 過去の質問と回答の保存
- 類似質問の検索
- [詳細な説明](docs/learning_module.md)

### 5. ルーターモデル
- 入力テキストの分析と適切なエージェントへのルーティング
- エージェントベースのアーキテクチャ
- [詳細な説明と改善案](docs/router_model.md)

システムの全体的なフロー図と詳細な説明は[アーキテクチャ概要](docs/architecture_overview.md)を参照してください。

## 📁 ディレクトリ構造

```
/
├── bin/           - コンパイル済みバイナリ
├── data/          - データファイル
│   ├── models/             - モデルデータ
│   ├── vector_db/          - ベクトルデータベース
│   ├── word_vectors.dat    - 単語ベクトルデータ
│   └── learning_db.txt     - 学習データベース
├── knowledge/     - 知識関連ファイル
│   ├── base/               - 知識ベース（カテゴリ別サブディレクトリを含む）
│   │   └── system/         - システム関連の知識ファイル
│   ├── docs/               - トピック別知識ファイル
│   └── text/               - テキストコーパス
│       └── wikipedia/      - Wikipediaデータ
├── logs/          - ログファイル（質問と回答の履歴）
└── src/           - ソースコード
    ├── analyzers/          - テキスト解析モジュール
    ├── compressors/        - データ圧縮モジュール
    ├── generators/         - 文生成モジュール
    ├── include/            - 共通ヘッダとユーティリティ
    │   ├── knowledge_manager.c    - 知識ベース管理
    │   ├── learning_module.c      - 学習モジュール
    │   └── word_loader.c          - 単語読み込み
    ├── routers/             - ルーティングモジュール
    └── vector_search/       - ベクトル検索モジュール
```

各ディレクトリとファイルの詳細な説明は[ファイル構造ガイド](docs/file_structure.md)を参照してください。

### 知識ベース

システムは`knowledge/base`ディレクトリ内のマークダウンファイルから知識を取得します。各ファイルは以下の形式に従います：

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

現在、以下のカテゴリの知識ベースが実装されています：
- AI: 人工知能に関する知識
- Culture: 各国の文化と歴史
- System: システム関連の知識

知識ベースの拡張方法については[知識ベース拡張ガイド](docs/knowledge_base_guide.md)を参照してください。

## 📊 ベンチマーク

GeneLLMの性能を評価するためのベンチマークツールが用意されています：

```bash
# ベンチマークを実行
./benchmark/run_benchmark.sh
```

ベンチマークでは以下の項目が測定されます：
- 応答速度
- メモリ使用量
- 応答品質（正確性）
- 知識検索の精度

詳細な結果と分析は[ベンチマークレポート](benchmark/README.md)を参照してください。

## 📝 ライセンス

このプロジェクトはMITライセンスの下で公開されています。詳細は[LICENSE](LICENSE)ファイルを参照してください。

## 👥 貢献

GeneLLMプロジェクトへの貢献を歓迎します！

- バグ報告や機能リクエストは[Issue](https://github.com/enablerdao/GeneLLM/issues)にて受け付けています
- コードの改善や新機能の追加は[Pull Request](https://github.com/enablerdao/GeneLLM/pulls)を通じて提案できます
- 貢献方法の詳細は[貢献ガイド](CONTRIBUTING.md)を参照してください

---

**GeneLLM** - 軽量で効率的なC言語AI実装 | [GitHub](https://github.com/enablerdao/GeneLLM)
