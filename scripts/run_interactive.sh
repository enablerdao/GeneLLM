#!/bin/bash

# GeneLLM対話モード実行スクリプト
# このスクリプトは、GeneLLMを対話モードで実行します

# エラーが発生したら停止
set -e

echo "💬 GeneLLM 対話モード"
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

# 対話モードで実行
echo "🤖 GeneLLMを対話モードで起動します..."
echo "終了するには 'exit' または 'quit' と入力してください"
echo "====================="

./gllm -i

# 元のディレクトリに戻る
cd "$CURRENT_DIR"

echo "✅ 対話モードを終了しました"