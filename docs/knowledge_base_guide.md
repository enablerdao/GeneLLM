# 知識ベース拡張ガイド

このガイドでは、GeneLLMの知識ベースを拡張する方法について説明します。知識ベースを充実させることで、システムの応答品質を向上させることができます。

## 知識ベースの概要

GeneLLMの知識ベースは、`knowledge/base`ディレクトリに格納されたマークダウン形式のファイルで構成されています。各ファイルは特定のトピックに関する情報を含み、カテゴリとタグで分類されています。

知識ベースは以下のカテゴリに分かれています：

- **AI**: 人工知能に関する知識
- **Culture**: 各国の文化と歴史
- **System**: システム関連の知識
- **Programming**: プログラミング言語や技術
- **Science**: 科学的知識
- **General**: 一般的な知識

## 知識ファイルの形式

各知識ファイルは以下の形式に従います：

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

### メタデータ

ファイルの先頭にあるYAMLフロントマターには、以下のメタデータを指定します：

- **category**: ファイルのカテゴリ（必須）
- **tags**: 関連するキーワード（カンマ区切り、最大10個）
- **created_at**: 作成日時（YYYY-MM-DD HH:MM:SS形式）
- **updated_at**: 更新日時（YYYY-MM-DD HH:MM:SS形式）

### 本文

メタデータの後に、マークダウン形式で本文を記述します。本文は以下の要素を含むことができます：

- 見出し（`#`, `##`, `###`など）
- 段落テキスト
- リスト（箇条書き、番号付き）
- コードブロック
- 表
- 引用
- リンク

## 知識ファイルの追加方法

### 1. 新しいファイルの作成

適切なカテゴリディレクトリ内に新しいマークダウンファイルを作成します：

```bash
# AIカテゴリに新しいファイルを作成
mkdir -p knowledge/base/ai
touch knowledge/base/ai/machine_learning.md
```

### 2. メタデータと内容の記述

エディタでファイルを開き、メタデータと内容を記述します：

```markdown
---
category: AI
tags: 機械学習, アルゴリズム, データサイエンス
created_at: 2023-03-23 12:00:00
updated_at: 2023-03-23 12:00:00
---

# 機械学習の基礎

機械学習は、コンピュータシステムが明示的にプログラムされることなく、データから学習し改善する能力を提供する人工知能の一分野です。

## 主要なアプローチ

機械学習には以下の主要なアプローチがあります：

1. **教師あり学習**: ラベル付きデータを使用して予測モデルを構築
2. **教師なし学習**: ラベルなしデータからパターンを発見
3. **強化学習**: 環境との相互作用を通じて最適な行動を学習

## 一般的なアルゴリズム

- 線形回帰
- ロジスティック回帰
- 決定木
- ランダムフォレスト
- サポートベクターマシン
- ニューラルネットワーク
```

### 3. ファイル名の命名規則

ファイル名は以下の規則に従うことをお勧めします：

- 英数字とアンダースコアのみを使用
- 小文字を使用
- 内容を簡潔に表す名前
- `.md`拡張子を使用

例：`machine_learning.md`, `neural_networks.md`, `deep_learning_intro.md`

## 多言語サポート

GeneLLMは多言語の知識ファイルをサポートしています。言語固有のファイルを作成するには、ファイル名に言語コードを追加します：

```
japan.md       # デフォルト（日本語）
japan_en.md    # 英語版
japan_fr.md    # フランス語版
```

## 知識ベースの構造化

大規模な知識ベースを効率的に管理するために、以下の構造化アプローチを推奨します：

```
knowledge/base/
├── ai/                     # AIカテゴリ
│   ├── machine_learning/   # 機械学習サブカテゴリ
│   │   ├── basics.md
│   │   ├── algorithms.md
│   │   └── applications.md
│   └── neural_networks/    # ニューラルネットワークサブカテゴリ
│       ├── cnn.md
│       └── rnn.md
├── programming/            # プログラミングカテゴリ
│   ├── c/                  # C言語サブカテゴリ
│   ├── python/             # Pythonサブカテゴリ
│   └── javascript/         # JavaScriptサブカテゴリ
└── science/                # 科学カテゴリ
    ├── physics/            # 物理学サブカテゴリ
    └── chemistry/          # 化学サブカテゴリ
```

## 知識ファイルの最適化

### 1. 適切な長さ

知識ファイルは以下のガイドラインに従うことをお勧めします：

- 最小: 100単語以上
- 最適: 500〜2000単語
- 最大: 16KB（約8000単語）

長すぎる内容は複数のファイルに分割してください。

### 2. 構造化された内容

内容は以下のように構造化することをお勧めします：

- 明確な見出し階層
- 短い段落（3〜5文）
- 箇条書きリストの活用
- 重要な用語の強調（**太字**）
- 関連する概念へのリンク

### 3. SEO最適化

検索性能を向上させるために、以下の点に注意してください：

- 重要なキーワードをタイトルに含める
- 関連するタグを追加する
- 最初の段落に主要な概念を含める
- 同義語や関連用語を本文に含める

## 知識ベースの検証

新しい知識ファイルを追加した後、以下のコマンドで検証できます：

```bash
# 知識ベースの検証
./gllm --validate-knowledge

# 特定のファイルのテスト
./gllm --test-knowledge knowledge/base/ai/machine_learning.md
```

## 知識ベースの更新

既存の知識ファイルを更新する場合は、以下の点に注意してください：

1. `updated_at`フィールドを現在の日時に更新
2. 変更内容を簡潔に記録（オプション）
3. 関連するタグを必要に応じて更新

例：

```markdown
---
category: AI
tags: 機械学習, アルゴリズム, データサイエンス, ディープラーニング
created_at: 2023-03-23 12:00:00
updated_at: 2023-04-15 09:30:00
changes: ディープラーニングセクションを追加
---

# 機械学習の基礎

...
```

## サンプル知識ファイル

### プログラミング言語（C言語）

```markdown
---
category: Programming
tags: C言語, プログラミング, 入門, コンパイル
created_at: 2023-03-23 12:00:00
updated_at: 2023-03-23 12:00:00
---

# C言語入門

C言語は、1972年にAT&Tベル研究所のデニス・リッチーによって開発された汎用プログラミング言語です。システムプログラミングやアプリケーション開発に広く使用されています。

## 特徴

- 低レベル操作と高レベル機能のバランス
- 効率的なメモリ管理
- 移植性の高いコード
- 豊富なライブラリ

## Hello Worldプログラム

```c
#include <stdio.h>

int main() {
    printf("Hello, World!\n");
    return 0;
}
```

## コンパイル方法

```bash
gcc -o hello hello.c
./hello
```

## 基本的なデータ型

- `int`: 整数
- `float`: 単精度浮動小数点数
- `double`: 倍精度浮動小数点数
- `char`: 文字
- `void`: 型なし

## 制御構造

### 条件分岐

```c
if (条件) {
    // 条件が真の場合の処理
} else {
    // 条件が偽の場合の処理
}
```

### ループ

```c
// forループ
for (初期化; 条件; 更新) {
    // 繰り返し処理
}

// whileループ
while (条件) {
    // 繰り返し処理
}
```

## 関数

```c
戻り値の型 関数名(引数の型 引数名, ...) {
    // 処理
    return 戻り値;
}
```

例：
```c
int add(int a, int b) {
    return a + b;
}
```
```

## まとめ

知識ベースの拡張は、GeneLLMの応答品質を向上させる重要な要素です。このガイドに従って、構造化された高品質な知識ファイルを追加することで、システムの能力を大幅に向上させることができます。

質問や提案がある場合は、[Issue](https://github.com/enablerdao/GeneLLM/issues)にて報告してください。