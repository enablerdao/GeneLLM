#!/bin/bash

# GeneLLMクイックスタートスクリプト
# このスクリプトは、GeneLLMを素早く始めるためのものです

# エラーが発生したら停止
set -e

echo "🚀 GeneLLM クイックスタート"
echo "=========================="

# 現在のディレクトリを保存
CURRENT_DIR=$(pwd)
# スクリプトのディレクトリを取得
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# プロジェクトのルートディレクトリを設定
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

# ルートディレクトリに移動
cd "$ROOT_DIR"

# OSの検出
OS="$(uname -s)"
echo "検出されたOS: $OS"

# 依存関係のインストール
install_dependencies() {
    echo "依存関係をインストールしています..."
    
    case "$OS" in
        Linux*)
            echo "Linuxの依存関係をインストールしています..."
            if [ -x "$(command -v apt-get)" ]; then
                sudo apt-get update
                sudo apt-get install -y build-essential libmecab-dev mecab mecab-ipadic-utf8 libcurl4-openssl-dev
            elif [ -x "$(command -v yum)" ]; then
                sudo yum install -y gcc make mecab mecab-devel curl-devel
            else
                echo "サポートされていないLinuxディストリビューションです。手動で依存関係をインストールしてください。"
                exit 1
            fi
            ;;
        Darwin*)
            echo "macOSの依存関係をインストールしています..."
            if [ -x "$(command -v brew)" ]; then
                brew install gcc mecab mecab-ipadic curl
            else
                echo "Homebrewがインストールされていません。Homebrewをインストールしてから再試行してください。"
                echo "Homebrewのインストール方法: /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
                exit 1
            fi
            ;;
        CYGWIN*|MINGW*|MSYS*)
            echo "Windowsの依存関係をインストールしています..."
            echo "Windowsでは手動で依存関係をインストールする必要があります。"
            echo "1. MinGW-w64をインストール: http://mingw-w64.org/doku.php/download"
            echo "2. MeCabをインストール: https://taku910.github.io/mecab/"
            echo "3. libcurlをインストール"
            exit 1
            ;;
        *)
            echo "不明なOSです: $OS"
            echo "手動で依存関係をインストールしてください。"
            exit 1
            ;;
    esac
    
    echo "依存関係のインストールが完了しました。"
}

# ビルド
build_genellm() {
    echo "GeneLLMをビルドしています..."
    ./scripts/build.sh
}

# 質問の実行
run_query() {
    echo "GeneLLMを実行しています..."
    ./gllm "C言語でHello Worldを表示するプログラムを教えてください"
}

# メイン処理
echo "GeneLLMクイックスタートを開始します..."

# 依存関係のインストールを確認
read -p "依存関係をインストールしますか？ (y/n): " install_deps
if [[ "$install_deps" =~ ^[Yy]$ ]]; then
    install_dependencies
fi

# ビルド
build_genellm

# 質問の実行
echo "サンプル質問を実行します..."
run_query

echo "🎉 クイックスタートが完了しました！"
echo "詳細な使用方法については、README.mdを参照してください。"

# 対話モードの提案
read -p "対話モードを開始しますか？ (y/n): " start_interactive
if [[ "$start_interactive" =~ ^[Yy]$ ]]; then
    echo "対話モードを開始します..."
    ./scripts/run_interactive.sh
fi

# 元のディレクトリに戻る
cd "$CURRENT_DIR"