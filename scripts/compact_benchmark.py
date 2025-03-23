#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import subprocess
import json
import os
import time
from datetime import datetime

# 各ジャンルから1問ずつ選んだ質問リスト（合計10問）
QUESTIONS = [
    {"genre": "システム", "question": "GeneLLMの仕組みについて教えてください"},
    {"genre": "AI", "question": "人工知能の歴史について教えてください"},
    {"genre": "プログラミング", "question": "C言語の特徴について教えてください"},
    {"genre": "自然言語処理", "question": "形態素解析とは何ですか"},
    {"genre": "量子コンピュータ", "question": "量子コンピュータとは何ですか"},
    {"genre": "数学", "question": "線形代数とは何ですか"},
    {"genre": "物理学", "question": "相対性理論について説明してください"},
    {"genre": "健康", "question": "健康的な食事の基本原則を教えてください"},
    {"genre": "料理", "question": "基本的な調理技術について教えてください"},
    {"genre": "旅行", "question": "効率的な旅行計画の立て方を教えてください"}
]

def run_benchmark():
    """コンパクトなベンチマークを実行する関数"""
    results = {}
    
    # 結果ディレクトリを作成
    os.makedirs("benchmark_results", exist_ok=True)
    
    # 現在の日時を取得してファイル名に使用
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    result_file = f"benchmark_results/compact_benchmark_{timestamp}.json"
    
    total_questions = len(QUESTIONS)
    
    print(f"コンパクトベンチマークを開始します。合計 {total_questions} 問")
    
    for i, item in enumerate(QUESTIONS):
        genre = item["genre"]
        question = item["question"]
        
        print(f"\n[{i+1}/{total_questions}] ジャンル: {genre}")
        print(f"質問: {question}")
        
        # GeneLLMを実行して回答を取得
        try:
            start_time = time.time()
            result = subprocess.run(
                ["./gllm", question],
                capture_output=True,
                text=True,
                check=True
            )
            end_time = time.time()
            
            answer = result.stdout.strip()
            response_time = end_time - start_time
            
            # 回答が長い場合は省略表示
            if len(answer) > 500:
                display_answer = answer[:500] + "..."
            else:
                display_answer = answer
                
            print(f"回答: {display_answer}")
            print(f"応答時間: {response_time:.2f}秒")
            
            # 結果を保存
            if genre not in results:
                results[genre] = {
                    "questions": [],
                    "avg_time": 0
                }
            
            results[genre]["questions"].append({
                "question": question,
                "answer": answer,
                "response_time": response_time
            })
            
            # 平均応答時間を更新
            total_time = sum(q["response_time"] for q in results[genre]["questions"])
            results[genre]["avg_time"] = total_time / len(results[genre]["questions"])
            
            # 結果を保存
            with open(result_file, "w", encoding="utf-8") as f:
                json.dump(results, f, ensure_ascii=False, indent=2)
            
            # 少し待機して次の質問へ
            time.sleep(1)
            
        except subprocess.CalledProcessError as e:
            print(f"エラー: {e}")
            print(f"標準出力: {e.stdout}")
            print(f"標準エラー: {e.stderr}")
    
    # 最終結果を表示
    print("\n===== ベンチマーク結果 =====")
    for genre, data in results.items():
        print(f"\nジャンル: {genre}")
        print(f"質問数: {len(data['questions'])}")
        print(f"平均応答時間: {data['avg_time']:.2f}秒")
    
    print(f"\n結果は {result_file} に保存されました。")

if __name__ == "__main__":
    run_benchmark()