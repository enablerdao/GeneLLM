# GeneLLM

C言語による超軽量AI実装 - 統合コマンドインターフェース

## 概要

GeneLLMは、C言語で実装された超軽量なAIシステムです。大規模言語モデル（LLM）の基本的な機能を、最小限のリソースで実現することを目指しています。新しい統合コマンド `./genellm`（または短縮形の `./gllm`、`./gl`、`./gm`）により、すべての機能を簡単に利用できるようになりました。

## 特徴

- **超軽量**: 最小限のメモリと計算リソースで動作
- **C言語実装**: 高速で効率的な処理
- **ベクトル検索**: 効率的な知識検索機能
- **日本語対応**: 日本語テキストの処理に対応
- **拡張可能**: モジュール式の設計で機能拡張が容易
- **フォールバック機能**: 知識ベースにない質問に対しても適切な回答を生成
- **統合コマンド**: すべての機能を一つのコマンドで操作可能（`genellm`、`gllm`、`gl`、`gm`）

## インストール

### 依存関係

- C コンパイラ (gcc/clang)
- MeCab (日本語形態素解析)
- libcurl (オプション: 外部APIとの連携用)
- Bash 4.0以上

### Ubuntuでのインストール

```bash
# 必要なパッケージのインストール
sudo apt-get update
sudo apt-get install -y build-essential libmecab-dev mecab mecab-ipadic-utf8 libcurl4-openssl-dev

# リポジトリのクローン
git clone https://github.com/enablerdao/GeneLLM.git
cd GeneLLM

# コンパイル
./genellm compile
```

### macOSでのインストール

```bash
# Homebrewを使用して必要なパッケージをインストール
brew install gcc mecab mecab-ipadic curl

# リポジトリのクローン
git clone https://github.com/enablerdao/GeneLLM.git
cd GeneLLM

# コンパイル
./genellm compile
```

## 使用方法

### 統合コマンド

GeneLLMは統合コマンド `./genellm` で操作します。短縮形として `./gllm`、`./gl`、`./gm` も使用できます。

```bash
# ヘルプを表示
./genellm help
# または
./gllm help
# または
./gl help
# または
./gm help

# バージョン情報を表示
./genellm version
```

### 質問に回答する

```bash
# 単一の質問に回答
./genellm ask "日本の首都は？"

# デバッグモードで質問
./genellm ask "C言語のポインタとは？" --debug
```

### 対話モード

```bash
# 対話モードを開始
./genellm chat

# デバッグモードで対話
./genellm chat --debug
```

### サーバーの起動

```bash
# APIサーバーを起動（バックグラウンド）
./genellm server

# 特定のポートでサーバーを起動
./genellm server --port=8080

# フォアグラウンドでサーバーを起動
./genellm server --foreground
```

### Webインターフェース

```bash
# Webインターフェースを起動
./genellm web

# ブラウザで http://localhost:50065/ にアクセス
```

### 知識ベースの更新

```bash
# 知識ベースを更新
./genellm update

# すべてのファイルを強制的に再処理
./genellm update --force
```

### サーバーの管理

```bash
# サーバーの状態を確認
./genellm status

# サーバーを停止
./genellm stop
```

### 設定の管理

```bash
# 現在の設定を表示
./genellm config

# 設定を変更
./genellm config port=8080 debug=true
```

## 設定オプション

| 設定項目 | 説明 | デフォルト値 |
|---------|------|------------|
| port | サーバーのポート番号 | 50065 |
| debug | デバッグモードの有効/無効 | false |
| fallback | フォールバック機能の有効/無効 | true |
| auto_update | 自動更新の有効/無効 | true |
| log_level | ログレベル (debug, info, warn, error) | info |

## 知識ベースの拡張

GeneLLMの知識ベースは以下のディレクトリに格納されています：

- `knowledge/base/`: 基本的な質問と回答のペア
- `knowledge/topics/`: トピック別の知識
- `docs/`: ドキュメントから抽出される知識

新しい知識を追加するには、これらのディレクトリに以下の形式でファイルを追加します：

1. 質問と回答のペア形式:
   ```
   質問1|回答1
   質問2|回答2
   ```

2. マークダウン形式:
   ```markdown
   # 質問1
   回答1の内容

   # 質問2
   回答2の内容
   ```

知識を追加した後は、`./genellm update` コマンドを実行して知識ベースを更新してください。

### ベクトルデータとトークンの確認

単語とそのベクトル表現を確認するには、`vectors` コマンドを使用します：

```bash
# デフォルトで10件表示
./vectors

# 表示件数を指定（5件）
./vectors 5

# 開始位置とともに指定（位置100から5件）
./vectors 5 100

# ヘルプを表示
./vectors --help
```

単語リストとテキストのトークン化結果を確認するには、`tokens` コマンドを使用します：

```bash
# 単語リストを表示（デフォルト10件）
./tokens words

# 単語リストを指定件数表示（5件）
./tokens words 5

# 単語リストを指定位置から表示（位置100から5件）
./tokens words 5 100

# テキストをトークナイズして表示
./tokens tokenize "こんにちは世界"
```

すべての機能は統合コマンドインターフェース（`genellm`、`gl`、`gllm`、`gm`）で利用できます：

```bash
# ヘルプを表示
./genellm help

# ベクトルデータを表示
./gl vectors 5

# 単語リストを表示
./gllm words 5

# テキストをトークナイズして表示
./gm tokenize "GeneLLMはC言語で実装された軽量AIです"

# テキストをDNAコードに圧縮
./genellm dna-compress "猫が魚を食べる"

# DNAコードからテキストを復元
./gl dna-decompress "E00C00R00"

# 動詞の活用形と種類を表示
./gllm verb-info "食べる"

# 類似単語を検索
./gm similar "猫" 5

# 単語間の類似度を計算
./genellm compare "猫" "犬"

# DNA辞書を表示
./gl dna-dict show

# DNA辞書にエントリを追加
./gllm dna-dict add E 人間
```

各機能は個別のコマンドでも利用できます：

```bash
# DNAコマンド
./dna compress "猫が魚を食べる"
./dna decompress "E00C00R00"
./dna verb-info "食べる"

# トークン表示コマンド
./tokens words 5
./tokens tokenize "こんにちは世界"

# ベクトル表示コマンド
./vectors 5

# 類似度検索コマンド
./similar find "猫" 5
./similar compare "猫" "犬"

# DNA辞書管理コマンド
./dna-dict show
./dna-dict add E 人間
```

## 機能

### ベクトル検索

テキストをベクトル化し、類似度に基づいて検索を行います。最適化されたベクトルデータベースにより、高速な検索が可能です。

### 形態素解析

MeCabを使用して日本語テキストを形態素解析し、単語単位での処理を行います。シングルトンパターンの導入により、初期化コストを削減しています。

### 知識ベース

テキストファイルから知識を抽出し、ベクトルデータベースに格納します。増分更新機能により、変更されたファイルのみを効率的に処理します。

### フォールバック機能

知識ベースに適切な回答がない場合、フォールバックジェネレーターが代替の回答を生成します。詳細は[フォールバックシステムのドキュメント](docs/fallback_system.md)を参照してください。

## ドキュメント

詳細なドキュメントは以下のファイルを参照してください：

- [使用ガイド](docs/readme/README_USAGE.md): 詳細な使用方法
- [フォールバック機能](docs/readme/README_FALLBACK.md): フォールバック機能の使い方
- [ディレクトリ構造](docs/readme/README_DIRECTORY_STRUCTURE.md): プロジェクトの構成
- [デバッグガイド](docs/DEBUG_GUIDE.md): デバッグ方法
- [最適化ガイド](docs/OPTIMIZATION_GUIDE.md): パフォーマンス最適化の詳細
- [改良機能のドキュメント](docs/IMPROVED_FEATURES.md): 最近の改良点

## 最近の改良点

GeneLLMには以下の改良が加えられました：

- **統合コマンドインターフェース**: すべての機能を一つのコマンドで操作可能に（`genellm`、`gllm`、`gl`、`gm`）
- **パフォーマンス最適化**: キーワードインデックス、MeCabシングルトン、効率的な文字列処理などによる高速化
- **拡張された回答データベース**: キーワードベースのマッチング強化、関連性スコアによる回答の品質向上
- **多様なトピックへの対応**: プログラミング関連だけでなく、様々なトピックに対応
- **フィードバックによる学習**: ユーザーからのフィードバックに基づいて関連性スコアを更新する機能
- **類似度計算アルゴリズムの改善**: 数字で始まる質問と人名を含む質問の誤マッチを防止
- **ベクトルデータ表示機能**: 単語とそのベクトル表現を確認できる `vectors` コマンドを追加
- **トークン表示機能**: 単語リストとテキストのトークン化結果を表示する `tokens` コマンドを追加
- **DNAベースの概念圧縮機能**: テキストを主語(E)、動詞(C)、結果(R)の形式で圧縮する `dna` コマンドを追加（動詞の活用形と種類の表示機能も含む）
- **ベクトル類似度検索機能**: 単語間の類似度計算や類似単語検索を行う `similar` コマンドを追加
- **DNA辞書管理機能**: DNA辞書の表示、追加、削除、エクスポート、インポート、検索を行う `dna-dict` コマンドを追加
- **統合ツールコマンド**: 開発・デバッグ用の各種ツールを一つのコマンド `genellm-tools` で操作可能に

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
