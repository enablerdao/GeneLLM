# GeneLLM ベンチマーク

このディレクトリには、GeneLLMの性能を評価するためのベンチマークツールが含まれています。

## 概要

このベンチマークは以下の項目を測定します：

- 応答時間（秒）
- 応答の品質（0-10のスコア）
- 応答の文字数

## 使用方法

### ベンチマークの実行

```bash
# 実行権限を付与
chmod +x benchmark/run_benchmark.sh

# ベンチマークを実行
./benchmark/run_benchmark.sh
```

### グラフの生成

```bash
# 必要なPythonパッケージをインストール
pip install pandas matplotlib numpy

# グラフを生成
python benchmark/generate_charts.py
```

## ファイル構成

- `questions.txt` - ベンチマークに使用する20個の質問
- `run_benchmark.sh` - ベンチマークを実行するスクリプト
- `generate_charts.py` - 結果からグラフを生成するPythonスクリプト
- `results/` - ベンチマーク結果の出力ディレクトリ
  - `benchmark_results.md` - Markdown形式の結果
  - `benchmark_results.csv` - CSV形式の結果
  - `question_*.md` - 各質問の詳細結果
  - `charts/` - 生成されたグラフ

## 評価方法

応答の評価は以下の基準に基づいています：

1. **応答の長さ** (0-5点)
   - 50文字未満: 1点
   - 50-200文字: 2点
   - 200-500文字: 3点
   - 500-1000文字: 4点
   - 1000文字以上: 5点

2. **キーワードの存在** (0-5点)
   - 質問に関連する重要なキーワードが含まれているかどうか
   - 例: C言語のコード例では `#include`, `main`, `return` などの存在をチェック

合計スコアは10点満点で評価されます。