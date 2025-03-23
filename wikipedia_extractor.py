#!/usr/bin/env python3
"""
Wikipediaの記事から冒頭のイントロ部分を抽出し、プレーンテキストとして保存するスクリプト
"""

import wikipediaapi
import requests
from bs4 import BeautifulSoup
import time
import os
import random
import re
import argparse

def clean_text(text):
    """テキストをクリーンアップする"""
    # 余分な空白を削除
    text = re.sub(r'\s+', ' ', text)
    # 括弧内の参照情報を削除 [1], [2] など
    text = re.sub(r'\[\d+\]', '', text)
    # 先頭と末尾の空白を削除
    text = text.strip()
    return text

def get_first_paragraph(page):
    """Wikipediaページから最初の段落を抽出する"""
    # WikipediaAPIを使用して記事の全文を取得
    content = page.text
    
    # 最初の段落を抽出（== で始まるセクションの前まで）
    paragraphs = content.split('\n\n')
    
    # 空でない最初の段落を見つける
    for para in paragraphs:
        if para and not para.startswith('=='):
            return clean_text(para)
    
    return ""

def get_random_wikipedia_articles(language='ja', count=100):
    """指定された言語のランダムなWikipedia記事を取得する"""
    wiki = wikipediaapi.Wikipedia(
        language=language,
        extract_format=wikipediaapi.ExtractFormat.WIKI,
        user_agent='WikipediaExtractor/1.0 (your-email@example.com)'
    )
    
    articles = []
    
    # ランダム記事のURLを構築
    random_url = f"https://{language}.wikipedia.org/wiki/Special:Random"
    
    for i in range(count):
        try:
            # ランダム記事にリダイレクト
            response = requests.get(random_url, allow_redirects=True)
            
            if response.status_code == 200:
                # 最終的なURLからページタイトルを抽出
                final_url = response.url
                title = final_url.split('/')[-1]
                
                # URLデコード
                title = requests.utils.unquote(title)
                
                # WikipediaAPIでページを取得
                page = wiki.page(title)
                
                if page.exists():
                    # 最初の段落を抽出
                    first_para = get_first_paragraph(page)
                    
                    if first_para:
                        print(f"[{i+1}/{count}] 取得: {page.title}")
                        articles.append({
                            'title': page.title,
                            'intro': first_para,
                            'url': page.fullurl
                        })
                    else:
                        print(f"[{i+1}/{count}] スキップ: {page.title} (イントロなし)")
                        i -= 1  # カウントを減らして再試行
            else:
                print(f"[{i+1}/{count}] エラー: HTTPステータス {response.status_code}")
                i -= 1  # カウントを減らして再試行
                
        except Exception as e:
            print(f"エラー: {e}")
            i -= 1  # カウントを減らして再試行
        
        # サーバーに負荷をかけないよう少し待機
        time.sleep(1)
    
    return articles

def save_to_file(articles, output_file):
    """記事をファイルに保存する"""
    with open(output_file, 'w', encoding='utf-8') as f:
        for article in articles:
            f.write(f"# {article['title']}\n")
            f.write(f"{article['intro']}\n")
            f.write(f"URL: {article['url']}\n")
            f.write("\n---\n\n")
    
    print(f"{len(articles)}件の記事を {output_file} に保存しました。")

def save_plain_text(articles, output_file):
    """記事のイントロ部分だけをプレーンテキストとして保存する"""
    with open(output_file, 'w', encoding='utf-8') as f:
        for article in articles:
            f.write(f"{article['intro']}\n\n")
    
    print(f"{len(articles)}件の記事のイントロを {output_file} に保存しました。")

def main():
    parser = argparse.ArgumentParser(description='Wikipediaの記事から冒頭のイントロ部分を抽出する')
    parser.add_argument('--count', type=int, default=100, help='取得する記事の数 (デフォルト: 100)')
    parser.add_argument('--lang', type=str, default='ja', help='Wikipediaの言語 (デフォルト: ja)')
    parser.add_argument('--output', type=str, default='wikipedia_intros.txt', help='出力ファイル名 (デフォルト: wikipedia_intros.txt)')
    parser.add_argument('--plain', type=str, default='wikipedia_plain.txt', help='プレーンテキスト出力ファイル名 (デフォルト: wikipedia_plain.txt)')
    
    args = parser.parse_args()
    
    print(f"{args.lang}版Wikipediaから{args.count}件の記事を取得します...")
    articles = get_random_wikipedia_articles(language=args.lang, count=args.count)
    
    # 詳細情報付きで保存
    save_to_file(articles, args.output)
    
    # プレーンテキストとして保存
    save_plain_text(articles, args.plain)

if __name__ == "__main__":
    main()