#!/bin/bash

# GeneLLMのテスト実行スクリプト
# このスクリプトは、GeneLLMの各種テストを実行します

# エラーが発生したら停止
set -e

echo "🧪 GeneLLM テスト実行スクリプト"
echo "================================="

# 現在のディレクトリを保存
CURRENT_DIR=$(pwd)
# スクリプトのディレクトリを取得
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# プロジェクトのルートディレクトリを設定
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

# ルートディレクトリに移動
cd "$ROOT_DIR"

# 必要なディレクトリが存在するか確認
if [ ! -d "bin" ]; then
  echo "❌ エラー: binディレクトリが見つかりません"
  exit 1
fi

# 基本的なテスト
run_basic_tests() {
  echo "🔍 基本的なテストを実行しています..."
  
  # メインプログラムのテスト
  if [ -f "gllm" ]; then
    echo "✓ メインプログラムのテスト"
    ./gllm -v
    ./gllm -h
    echo "✓ 簡単な質問テスト"
    ./gllm "こんにちは"
  else
    echo "❌ エラー: gllmが見つかりません"
  fi
  
  echo "基本的なテスト完了"
  echo
}

# 新しいコードのテスト
run_new_code_tests() {
  echo "🔍 新しいコードのテストを実行しています..."
  
  if [ -f "bin/test_new" ]; then
    echo "✓ 新コードのテスト"
    ./bin/test_new
  else
    echo "❌ エラー: bin/test_newが見つかりません"
  fi
  
  if [ -f "bin/gllm_simple" ]; then
    echo "✓ シンプル版のテスト"
    ./bin/gllm_simple -h
  else
    echo "❌ エラー: bin/gllm_simpleが見つかりません"
  fi
  
  echo "新しいコードのテスト完了"
  echo
}

# ツールのテスト
run_tool_tests() {
  echo "🔍 ツールのテストを実行しています..."
  
  # DNAコンプレッサーのテスト
  if [ -f "tools/dna_compressor/dna_compressor" ]; then
    echo "✓ DNAコンプレッサーのテスト"
    cd tools/dna_compressor
    ./dna_compressor
    cd "$ROOT_DIR"
  else
    echo "❌ エラー: tools/dna_compressor/dna_compressorが見つかりません"
  fi
  
  # 動詞活用のテスト
  if [ -f "tools/verb_conjugator/verb_conjugator" ]; then
    echo "✓ 動詞活用のテスト"
    cd tools/verb_conjugator
    ./verb_conjugator || echo "動詞活用テストは使用法を表示しました"
    cd "$ROOT_DIR"
  else
    echo "❌ エラー: tools/verb_conjugator/verb_conjugatorが見つかりません"
  fi
  
  echo "ツールのテスト完了"
  echo
}

# ビルドテスト
run_build_tests() {
  echo "🔍 ビルドテストを実行しています..."
  
  if [ -d "scripts/build/build_test" ]; then
    echo "✓ ビルドテスト"
    cd scripts/build/build_test
    if [ -f "test" ]; then
      ./test
    else
      echo "❌ エラー: scripts/build/build_test/testが見つかりません"
    fi
    cd "$ROOT_DIR"
  else
    echo "❌ エラー: scripts/build/build_testディレクトリが見つかりません"
  fi
  
  echo "ビルドテスト完了"
  echo
}

# 全テストの実行
run_all_tests() {
  run_basic_tests
  run_new_code_tests
  run_tool_tests
  run_build_tests
  
  echo "🎉 全テスト完了"
}

# コマンドライン引数の処理
if [ $# -eq 0 ]; then
  # 引数がない場合は全テストを実行
  run_all_tests
else
  # 引数に応じたテストを実行
  for arg in "$@"; do
    case $arg in
      basic)
        run_basic_tests
        ;;
      new)
        run_new_code_tests
        ;;
      tools)
        run_tool_tests
        ;;
      build)
        run_build_tests
        ;;
      all)
        run_all_tests
        ;;
      *)
        echo "❌ エラー: 不明な引数 '$arg'"
        echo "使用法: $0 [basic|new|tools|build|all]"
        exit 1
        ;;
    esac
  done
fi

# 元のディレクトリに戻る
cd "$CURRENT_DIR"

echo "✅ テスト実行が完了しました"