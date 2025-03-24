# GeneLLM 使用ガイド

このドキュメントでは、GeneLLMの使用方法について説明します。

## 基本的な使用方法

GeneLLMは、コマンドラインから質問を入力して応答を得ることができます。

```bash
./gllm "質問文"
```

例:
```bash
./gllm "C言語でHello Worldを表示するプログラムを教えてください"
```

## コマンドラインオプション

GeneLLMは以下のコマンドラインオプションをサポートしています:

- `-h, --help`: ヘルプメッセージを表示
- `-v, --version`: バージョン情報を表示
- `-d, --debug`: デバッグモードで実行
- `-i, --interactive`: 対話モードで実行

## 対話モード

対話モードでは、複数の質問を連続して入力することができます。

```bash
./gllm -i
```

対話モードを終了するには、`exit`または`quit`と入力します。

## デバッグモード

デバッグモードでは、処理の詳細情報が表示されます。

```bash
./gllm -d "質問文"
```

## 便利なスクリプト

GeneLLMには、使いやすくするための便利なスクリプトが用意されています。

### 対話モード実行スクリプト

```bash
./scripts/run_interactive.sh
```

### デバッグモード実行スクリプト

```bash
./scripts/run_debug.sh "質問文"
```

### テスト実行スクリプト

```bash
./scripts/run_tests.sh [basic|new|tools|build|all]
```

- `basic`: 基本的なテストを実行
- `new`: 新しいコードのテストを実行
- `tools`: ツールのテストを実行
- `build`: ビルドテストを実行
- `all`: すべてのテストを実行

## ツールの使用方法

GeneLLMには、いくつかの便利なツールが含まれています。

### DNAコンプレッサー

```bash
./tools/dna_compressor/dna_compressor <主語> <動詞> <結果>
```

例:
```bash
./tools/dna_compressor/dna_compressor 猫 食べる 嬉しい
```

### 動詞活用

```bash
./tools/verb_conjugator/verb_conjugator <動詞>
```

例:
```bash
./tools/verb_conjugator/verb_conjugator 食べる
```

## トラブルシューティング

### ビルドエラー

ビルドエラーが発生した場合は、以下を確認してください:

1. 必要な依存関係がインストールされているか
   ```bash
   # Ubuntuの場合
   sudo apt-get install -y libmecab-dev mecab mecab-ipadic-utf8 libcurl4-openssl-dev
   
   # macOSの場合
   brew install mecab mecab-ipadic curl
   ```

2. ビルドスクリプトが実行可能になっているか
   ```bash
   chmod +x build.sh
   ```

### 実行エラー

実行時にエラーが発生した場合は、以下を確認してください:

1. 実行ファイルが実行可能になっているか
   ```bash
   chmod +x gllm
   ```

2. 必要なディレクトリとファイルが存在するか
   ```bash
   mkdir -p data knowledge/text logs
   ```