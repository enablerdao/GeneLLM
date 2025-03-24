#!/bin/bash

# GeneLLMデバッグモード実行スクリプト
# このスクリプトは、GeneLLMをデバッグモードで実行します

# エラーが発生したら停止
set -e

echo "🔍 GeneLLM デバッグモード"
echo "====================="

# 現在のディレクトリを保存
CURRENT_DIR=$(pwd)
# スクリプトのディレクトリを取得
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# プロジェクトのルートディレクトリを設定
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

# ルートディレクトリに移動
cd "$ROOT_DIR"

# メインプログラムが存在するか確認
if [ ! -f "gllm" ]; then
  echo "❌ エラー: gllmが見つかりません"
  exit 1
fi

# 引数をチェック
if [ $# -eq 0 ]; then
  echo "❌ エラー: 質問文を指定してください"
  echo "使用法: $0 \"質問文\""
  exit 1
fi

# デバッグモードで実行
echo "🔍 GeneLLMをデバッグモードで実行します..."
echo "質問: $1"
echo "====================="

# 引数をダブルクォートで囲んで渡す
./gllm -d "$1"

# 元のディレクトリに戻る
cd "$CURRENT_DIR"

echo "✅ デバッグモードを終了しました"