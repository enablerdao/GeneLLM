# GeneLLM

C言語による超軽量AI実装

## 概要

GeneLLMは、C言語で実装された超軽量なAIシステムです。大規模言語モデル（LLM）の基本的な機能を、最小限のリソースで実現することを目指しています。

## 特徴

- **超軽量**: 最小限のメモリと計算リソースで動作
- **C言語実装**: 高速で効率的な処理
- **ベクトル検索**: 効率的な知識検索機能
- **日本語対応**: 日本語テキストの処理に対応
- **拡張可能**: モジュール式の設計で機能拡張が容易

## インストール

### 依存関係

- C コンパイラ (gcc/clang)
- MeCab (日本語形態素解析)
- libcurl (オプション: 外部APIとの連携用)

### Ubuntuでのインストール

```bash
# 必要なパッケージのインストール
sudo apt-get update
sudo apt-get install -y build-essential libmecab-dev mecab mecab-ipadic-utf8 libcurl4-openssl-dev

# リポジトリのクローン
git clone https://github.com/enablerdao/GeneLLM.git
cd GeneLLM

# ビルド
./scripts/build.sh
```

### macOSでのインストール

```bash
# Homebrewを使用して必要なパッケージをインストール
brew install gcc mecab mecab-ipadic curl

# リポジトリのクローン
git clone https://github.com/enablerdao/GeneLLM.git
cd GeneLLM

# ビルド
./scripts/build.sh
```

## 使用方法

### 基本的な使い方

```bash
./gllm "質問文"
```

例:
```bash
./gllm "C言語でHello Worldを表示するプログラムを教えてください"
```

### オプション

- `-h, --help`: ヘルプメッセージを表示
- `-v, --version`: バージョン情報を表示
- `-d, --debug`: デバッグモードで実行
- `-i, --interactive`: 対話モードで実行

### クイックスタート

クイックスタートスクリプトを使用すると、依存関係のインストールからビルド、最初の質問までを自動的に行います。

```bash
./scripts/quick_start.sh
```

### 便利なスクリプト

- `./scripts/run_interactive.sh`: 対話モードで実行
- `./scripts/run_debug.sh "質問文"`: デバッグモードで実行
- `./scripts/run_tests.sh [basic|new|tools|build|all]`: テストを実行

## ドキュメント

詳細なドキュメントは以下のファイルを参照してください：

- [使用ガイド](docs/readme/README_USAGE.md): 詳細な使用方法
- [ディレクトリ構造](docs/readme/README_DIRECTORY_STRUCTURE.md): プロジェクトの構成
- [デバッグガイド](docs/DEBUG_GUIDE.md): デバッグ方法
- [シンプル版の説明](docs/readme/README_simple.md): シンプル実装の概要

## 機能

### ベクトル検索

テキストをベクトル化し、類似度に基づいて検索を行います。

### 形態素解析

MeCabを使用して日本語テキストを形態素解析し、単語単位での処理を行います。

### 知識ベース

テキストファイルから知識を抽出し、ベクトルデータベースに格納します。

## 貢献

プロジェクトへの貢献を歓迎します。以下の方法で貢献できます：

1. イシューの報告
2. 機能リクエスト
3. プルリクエスト

## ライセンス

このプロジェクトはMITライセンスの下で公開されています。詳細は[LICENSE](LICENSE)ファイルを参照してください。

## 謝辞

- MeCab開発チーム
- 貢献者の皆様
