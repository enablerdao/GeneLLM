#!/bin/bash

# GeneLLMのC言語版統合スクリプト
# 使用方法: ./genellm_c.sh [update|tokenize|all]

WORKSPACE_DIR="/workspace"
KNOWLEDGE_MANAGER="$WORKSPACE_DIR/bin/knowledge_manager"

# コマンドライン引数の確認
if [ $# -lt 1 ]; then
    echo "使用方法: $0 [update|tokenize|all]"
    exit 1
fi

# コマンドを実行
case "$1" in
    update)
        # 知識ベースの更新
        echo "知識ベースを更新しています..."
        time "$KNOWLEDGE_MANAGER" update
        ;;
    tokenize)
        # トークナイズ処理
        echo "ナレッジとドキュメントをトークナイズしています..."
        time "$KNOWLEDGE_MANAGER" tokenize
        ;;
    all)
        # 全処理を実行
        echo "知識ベースの更新とトークナイズを実行しています..."
        time "$KNOWLEDGE_MANAGER" all
        ;;
    *)
        echo "不明なコマンド: $1"
        echo "使用方法: $0 [update|tokenize|all]"
        exit 1
        ;;
esac

exit $?