# C言語による知識ルーターモデル - コンパイル手順

このドキュメントでは、C言語で実装された知識ルーターモデルをコンパイルして実行する方法を説明します。

## 必要なもの

- C99対応のCコンパイラ（GCCなど）
- MeCabライブラリ（日本語形態素解析用）
- 数学ライブラリ（libm）

## 環境別セットアップ手順

### Ubuntu/Debian系

```bash
# 必要なパッケージのインストール
sudo apt-get update
sudo apt-get install -y gcc make mecab libmecab-dev mecab-ipadic-utf8 build-essential

# プロジェクトディレクトリに移動
cd /path/to/project

# binディレクトリの作成（存在しない場合）
mkdir -p bin

# コンパイル
gcc -Wall -Wextra -std=c99 -o bin/improved_router_model src/improved_router_model.c -lmecab -lm

# 実行ファイルをルートディレクトリにコピー（オプション）
cp bin/improved_router_model main

# 実行権限の付与
chmod +x main
```

### CentOS/RHEL系

```bash
# 必要なパッケージのインストール
sudo yum update
sudo yum install -y gcc make mecab mecab-devel mecab-ipadic

# プロジェクトディレクトリに移動
cd /path/to/project

# binディレクトリの作成（存在しない場合）
mkdir -p bin

# コンパイル
gcc -Wall -Wextra -std=c99 -o bin/improved_router_model src/improved_router_model.c -lmecab -lm

# 実行ファイルをルートディレクトリにコピー（オプション）
cp bin/improved_router_model main

# 実行権限の付与
chmod +x main
```

### macOS

```bash
# Homebrewがインストールされていない場合はインストール
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 必要なパッケージのインストール
brew install gcc mecab mecab-ipadic

# プロジェクトディレクトリに移動
cd /path/to/project

# binディレクトリの作成（存在しない場合）
mkdir -p bin

# コンパイル
gcc -Wall -Wextra -std=c99 -o bin/improved_router_model src/improved_router_model.c -lmecab -lm

# 実行ファイルをルートディレクトリにコピー（オプション）
cp bin/improved_router_model main

# 実行権限の付与
chmod +x main
```

## プロジェクト構造の確認

コンパイルする前に、以下のようなプロジェクト構造になっていることを確認してください：

```
.
├── bin/                  # コンパイルされたバイナリファイル
├── data/                 # データファイル
│   ├── knowledge_base/   # 知識ベース（Markdownファイル）
│   └── learning_db.txt   # 学習データベース
├── src/                  # ソースコード
│   ├── include/          # インクルードファイル
│   │   ├── knowledge_manager.c  # 知識ベース管理
│   │   ├── knowledge_manager.h  # 知識ベース管理ヘッダ
│   │   ├── learning_module.c    # 学習モジュール
│   │   └── llm_interface.c      # 外部LLMインターフェース
│   └── improved_router_model.c  # メインプログラム
└── main                  # 実行ファイル
```

## コンパイルオプションの説明

- `-Wall -Wextra`: すべての警告を表示
- `-std=c99`: C99標準を使用
- `-o bin/improved_router_model`: 出力ファイル名を指定
- `-lmecab`: MeCabライブラリをリンク
- `-lm`: 数学ライブラリをリンク

## 実行方法

コンパイルが成功したら、以下のコマンドでプログラムを実行できます：

```bash
# 単一の質問に対する応答
./main "質問文をここに入力"

# ルーターモードで実行
./main router "質問文をここに入力"

# デバッグモードで実行
./main -d router "質問文をここに入力"

# 対話モード
./main -i
```

## トラブルシューティング

### MeCabライブラリが見つからない場合

```bash
# Ubuntu/Debian
sudo apt-get install -y libmecab-dev mecab mecab-ipadic-utf8

# CentOS/RHEL
sudo yum install -y mecab mecab-devel mecab-ipadic

# macOS
brew install mecab mecab-ipadic
```

### 数学ライブラリが見つからない場合

```bash
# Ubuntu/Debian
sudo apt-get install -y libm-dev

# CentOS/RHEL
sudo yum install -y glibc-devel

# macOS
# 通常はデフォルトでインストールされています
```

### コンパイル時の警告について

コンパイル時に警告が表示されることがありますが、プログラムの実行には影響しません。警告を無視するには、コンパイルコマンドから `-Wall -Wextra` オプションを削除してください。

## データディレクトリの準備

プログラムを実行する前に、データディレクトリが存在することを確認してください：

```bash
# データディレクトリの作成
mkdir -p data/knowledge_base
```

## 知識ベースの拡張

知識ベースを拡張するには、`data/knowledge_base/` ディレクトリに新しいMarkdownファイルを追加します。ファイルは以下の形式に従ってください：

```markdown
---
title: ファイルのタイトル
category: カテゴリ
tags: タグ1, タグ2, タグ3
created_at: 2025-03-23 16:30:00
updated_at: 2025-03-23 16:30:00
---

# タイトル

## サブタイトル

コンテンツをここに記述...
```