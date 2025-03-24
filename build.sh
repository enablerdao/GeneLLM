#!/bin/bash
set -e

# ディレクトリの作成
mkdir -p bin data knowledge/text logs

# 必要なファイルが存在しない場合は作成
if [ ! -f "knowledge/text/japanese_words.txt" ]; then
    echo "日本語単語ファイルを作成しています..."
    echo "こんにちは" > knowledge/text/japanese_words.txt
    echo "世界" >> knowledge/text/japanese_words.txt
    echo "プログラミング" >> knowledge/text/japanese_words.txt
    echo "コンピュータ" >> knowledge/text/japanese_words.txt
    echo "人工知能" >> knowledge/text/japanese_words.txt
fi

# ソースコードのコンパイル
echo "GeneLLMをビルドしています..."

# src_newディレクトリが存在する場合は新しいソースを使用
if [ -d "src_new" ]; then
    echo "新しいソースコードを使用してビルドします..."
    gcc -Wall -Wextra -std=c99 -o gllm src_new/main_simple.c src_new/include/vector_db.c src_new/vector_search/vector_search.c src_new/vector_search/vector_search_global.c src_new/include/word_loader/word_loader.c -lmecab -lm -lcurl
else
    # 従来のソースコードを使用
    echo "従来のソースコードを使用してビルドします..."
    # vector_db.cが存在する場合は含める
    if [ -f "src/include/vector_db.c" ]; then
        gcc -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/include/word_loader.c src/include/vector_db.c -lmecab -lm -lcurl
    else
        # vector_search_global.cが存在する場合は含める
        if [ -f "src/vector_search/vector_search_global.c" ]; then
            gcc -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/vector_search/vector_search_global.c src/include/word_loader.c -lmecab -lm -lcurl
        else
            # 基本的なビルド
            gcc -Wall -Wextra -std=c99 -o gllm src/main.c src/vector_search/vector_search.c src/include/word_loader.c -lmecab -lm -lcurl
        fi
    fi
fi

# 実行権限の付与
chmod +x gllm

echo "ビルドが完了しました。"
echo "実行方法: ./gllm \"質問文\""