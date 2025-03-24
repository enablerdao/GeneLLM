# C言語による軽量AI実装プロジェクト

このプロジェクトは、C言語を使用して軽量なAIシステムを実装することを目的としています。スピードと効率性を最重視し、最小限のライブラリで自前実装する方針です。

## ⚡ クイックスタート（1行で実行）

```bash
curl -sSL https://raw.githubusercontent.com/enablerdao/GeneLLM/main/quick_start.sh | bash
```

このコマンドは必要な依存関係をインストールし、プロジェクトをビルドして最初の質問を実行します。

## 📦 ビルド済みバイナリ

各プラットフォーム向けのビルド済みバイナリは[リリースページ](https://github.com/enablerdao/GeneLLM/releases)からダウンロードできます：

- [Linux版](https://github.com/enablerdao/GeneLLM/releases/latest/download/gllm-linux.tar.gz)
- [macOS版](https://github.com/enablerdao/GeneLLM/releases/latest/download/gllm-macos.tar.gz)
- [Windows版](https://github.com/enablerdao/GeneLLM/releases/latest/download/gllm-windows.zip)

## 📊 ベンチマーク

GeneLLMの性能を評価するためのベンチマークツールが用意されています：

```bash
# ベンチマークを実行し、グラフを生成
./benchmark/run_all.sh
```

## 実装コンポーネント

以下の要素を全てC言語で実装します：

- 自然言語の簡易解析（概念抽出）
- ベクトル化（シンプルなベクトル演算）
- DNA風の概念圧縮・展開
- 超軽量のベクトル検索機構
- 非連続トークンを使った意味ノードの生成器
- グラフ構造管理（隣接リストなど）
- 簡易的な高速ルーターモデル（ルールベース）

## 開発順序

1. 意味概念抽出（簡易構文解析）
2. DNAベース圧縮
3. 圧縮概念からの文再構築
4. 超高速ベクトル検索
5. 非連続トークン生成（グラフ生成）
6. 簡易ルーターモデル
7. （オプション）P2P分散通信
8. （高度オプション）LLVMによるJIT高速化

## プロジェクト状況

現在、プロジェクトは以下の段階にあります：

- [x] 意味概念抽出モジュール
- [x] DNAベース圧縮モジュール
- [x] 圧縮概念からの文再構築
- [x] ベクトル検索モジュール
- [ ] 非連続トークン生成器
- [ ] ルーターモデル
- [ ] P2P通信モジュール
- [ ] JITコンパイル高速化

## 使用方法

```bash
# コンパイル
gcc -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/include/word_loader.c -lmecab -lm -lcurl

# 実行
./gllm "あなたの質問文をここに入力"

# デバッグモードで実行
./gllm -d "あなたの質問文をここに入力"
DEBUG=1 ./gllm "あなたの質問文をここに入力"
```

## ナレッジベース

システムは`data/knowledge_base`ディレクトリ内のマークダウンファイルからナレッジを取得します。各ファイルは以下の形式に従います：

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

現在、以下のカテゴリのナレッジベースが実装されています：

- AI: 人工知能に関する知識
- Culture: 各国の文化と歴史（日本、フランス、イタリアなど）

## 依存ライブラリ

- MeCab: 形態素解析エンジン
- libcurl: ウェブからのデータ取得（オプション）

## ライセンス

このプロジェクトはMITライセンスの下で公開されています。詳細はLICENSEファイルを参照してください。