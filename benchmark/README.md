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
# すべてのステップを一度に実行（ベンチマーク実行 + グラフ生成）
./benchmark/run_all.sh
```

### 過去のベンチマーク結果との比較

```bash
# 最新の2つのベンチマーク結果を比較
./benchmark/run_comparison.sh

# 特定のベンチマーク結果を比較
./benchmark/run_comparison.sh --dir benchmark/results/20230101_120000 benchmark/results/20230102_120000 --label "バージョン1.0" "バージョン1.1"

# ヘルプを表示
./benchmark/run_comparison.sh --help
```

## ファイル構成

- `questions.txt` - ベンチマークに使用する20個の質問
- `run_benchmark.sh` - ベンチマークを実行するスクリプト
- `generate_charts.py` - 結果からグラフを生成するPythonスクリプト
- `run_all.sh` - ベンチマークの実行とグラフ生成を一度に行うスクリプト
- `compare_benchmarks.py` - 複数のベンチマーク結果を比較するPythonスクリプト
- `run_comparison.sh` - ベンチマーク比較を実行するスクリプト
- `results/` - ベンチマーク結果の出力ディレクトリ
  - `YYYYMMDD_HHMMSS/` - タイムスタンプ付きの結果ディレクトリ
    - `benchmark_results.md` - Markdown形式の結果
    - `benchmark_results.csv` - CSV形式の結果
    - `question_*.md` - 各質問の詳細結果
    - `charts/` - 生成されたグラフ
  - `latest/` - 最新の結果へのシンボリックリンク
  - `comparison/` - ベンチマーク比較結果

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