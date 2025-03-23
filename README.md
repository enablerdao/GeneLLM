# GeneLLM - C言語による自然言語処理ツール

このプロジェクトは、C言語を使用して自然言語処理の基本的な機能を実装したものです。外部ライブラリの依存を最小限に抑えつつ、効率的な処理を目指しています。

## 機能

1. **意味概念抽出（簡易構文解析器）**
   - 日本語の文から主語・動詞・結果を抽出
   - MeCabを使用した形態素解析

2. **文生成機能**
   - 収集した単語から自然な日本語文を生成
   - 様々なパターンの文型をサポート

3. **DNAベースの概念圧縮（シンボル化）**
   - 主語(E)、動詞(C)、結果(R)を1文字コードと番号で圧縮
   - 例: 「猫が魚を食べる」→ `E00C00`
   - 圧縮されたDNAコードから元の文を再構築

## ファイル構成

- `c_implementation/simple_generator.c`: 簡易的な文生成プログラム（自前トークナイザー版）
- `c_implementation/mecab_generator.c`: MeCabを使用した形態素解析と文生成プログラム
- `c_implementation/dna_compressor.c`: DNAベースの概念圧縮プログラム
- `c_implementation/simple_test.txt`: 簡易的なテスト用テキストファイル

## 必要条件

- GCC コンパイラ
- MeCab（日本語形態素解析エンジン）
- libmecab-dev（MeCabの開発ライブラリ）

## インストール方法

```bash
# MeCabとその開発ライブラリをインストール
apt-get install -y mecab libmecab-dev mecab-ipadic-utf8

# コンパイル
cd c_implementation
gcc -Wall -Wextra -std=c99 -o mecab_generator mecab_generator.c -lmecab
gcc -Wall -Wextra -std=c99 -o simple_generator simple_generator.c -lm
gcc -Wall -Wextra -std=c99 -o dna_compressor dna_compressor.c -lmecab
```

## 使用方法

```bash
# 簡易文生成（自前トークナイザー版）
./simple_generator simple_test.txt

# MeCabを使用した形態素解析と文生成
./mecab_generator simple_test.txt

# DNAベースの概念圧縮
# 圧縮モード
./dna_compressor compress simple_test.txt dna_output.txt
# 展開モード
./dna_compressor decompress dna_output.txt reconstructed.txt
```

## 今後の改善点

1. **超軽量ベクトル検索機構**
   - 256次元程度のfloatベクトルを大量に格納し、高速に最も近いベクトルを検索

2. **非連続トークンを使った意味ノードの生成器**
   - 意味単位のノードを非連続的に探索して組み合わせ、自然な文を作る仕組み

3. **グラフ構造管理（隣接リストなど）**
   - 意味単位をノードとした簡易な隣接リスト形式のグラフを作成

4. **簡易的な高速ルーターモデル（ルールベース）**
   - 入力された文の内容によって、呼び出すべき専門エージェントを決定する小型のルーターモデル

## 注意点

- 日本語の処理は簡易的なものであり、より高度な処理を行うためには形態素解析器などの専門的なツールが必要です。
- C言語での日本語処理には制限があり、完全な処理は難しい場合があります。
- MeCabを使用するには、事前にインストールが必要です（`apt-get install -y mecab libmecab-dev mecab-ipadic-utf8`）。

## ライセンス

MIT