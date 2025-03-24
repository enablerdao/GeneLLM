# GeneLLM ディレクトリ構造

このドキュメントでは、GeneLLMプロジェクトのディレクトリ構造と各ファイルの役割について説明します。

## トップレベルディレクトリ

```
/
├── bin/                  # 実行可能バイナリファイル
├── data/                 # データファイル（ベクトルDB、学習データなど）
├── docs/                 # ドキュメント
├── knowledge/            # 知識ベース
│   ├── base/             # 基本的な知識
│   └── text/             # テキストデータ
├── logs/                 # ログファイル
├── scripts/              # スクリプト
│   └── build/            # ビルド関連スクリプト
├── src/                  # ソースコード（従来版）
├── src_new/              # ソースコード（新版）
└── tools/                # ツール
    ├── dna_compressor/   # DNAコンプレッサー
    ├── parser/           # パーサー
    ├── router/           # ルーター
    └── verb_conjugator/  # 動詞活用
```

## 主要ファイル

- `build.sh` - ビルドスクリプト
- `quick_start.sh` - クイックスタートスクリプト
- `gllm` - メインの実行可能ファイル
- `Makefile` - ビルド設定
- `README.md` - プロジェクト概要
- `README_simple.md` - シンプル版の概要

## ディレクトリの詳細

### bin/

実行可能バイナリファイルを格納します。ビルドされたプログラムはここに配置されます。

- `gllm` - メインプログラム
- `gllm_simple` - シンプル版のメインプログラム
- `test_new` - 新しいテストプログラム
- その他のツール

### data/

データファイルを格納します。

- `vector_db/` - ベクトルデータベース
- `models/` - モデルデータ
- `learning_db.txt` - 学習データベース

### docs/

プロジェクトのドキュメントを格納します。

### knowledge/

知識ベースを格納します。

- `base/` - 基本的な知識
  - `languages/` - 言語に関する知識
- `text/` - テキストデータ
  - `japanese_words.txt` - 日本語単語リスト

### logs/

ログファイルを格納します。

### scripts/

スクリプトファイルを格納します。

- `build/` - ビルド関連スクリプト
  - `build_test/` - ビルドテスト

### src/

従来版のソースコードを格納します。

- `main.c` - メインプログラム
- `vector_search/` - ベクトル検索
- `include/` - ヘッダーファイル

### src_new/

新版のソースコードを格納します。

- `main_simple.c` - シンプル版のメインプログラム
- `include/` - ヘッダーファイル
- `vector_search/` - ベクトル検索

### tools/

各種ツールを格納します。

- `dna_compressor/` - DNAコンプレッサー関連
- `parser/` - パーサー関連
- `router/` - ルーター関連
- `verb_conjugator/` - 動詞活用関連

## ビルドファイル

- `.github/workflows/` - GitHub Actions設定
  - `build.yml` - ビルドワークフロー

## その他のファイル

- `DEBUG_GUIDE.md` - デバッグガイド
- `benchmark/` - ベンチマーク
- `benchmark_results/` - ベンチマーク結果