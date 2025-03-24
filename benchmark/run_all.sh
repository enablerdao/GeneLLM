#!/bin/bash

# GeneLLM ベンチマーク実行スクリプト
# ベンチマークを実行し、グラフを生成します

echo "🔍 GeneLLM ベンチマーク開始"
echo "========================================"

# 必要なディレクトリを作成
mkdir -p benchmark/results/charts

# 実行権限を付与
chmod +x benchmark/run_benchmark.sh

# ベンチマークを実行
echo "📊 ベンチマークを実行中..."
./benchmark/run_benchmark.sh

# Pythonの依存関係をチェック
echo "🐍 Pythonの依存関係をチェック中..."
pip install pandas matplotlib numpy -q

# グラフを生成
echo "📈 グラフを生成中..."
python benchmark/generate_charts.py

echo "========================================"
echo "✅ ベンチマーク完了！"
echo "結果は benchmark/results/latest ディレクトリに保存されました"
echo "グラフは benchmark/results/latest/charts ディレクトリに保存されました"
echo ""
echo "結果を確認するには:"
echo "- Markdown形式: benchmark/results/latest/benchmark_results.md"
echo "- グラフサマリー: benchmark/results/latest/charts/summary.md"
echo ""
echo "過去のベンチマーク結果と比較するには:"
echo "- ./benchmark/run_comparison.sh --help"
echo "========================================"