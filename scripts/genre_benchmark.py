#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import subprocess
import json
import os
import time
import random
from datetime import datetime

# ジャンル別の質問リスト
QUESTIONS = {
    "システム": [
        "GeneLLMの仕組みについて教えてください",
        "GeneLLMのアーキテクチャはどうなっていますか",
        "GeneLLMの特徴は何ですか",
        "GeneLLMはどのように動作しますか",
        "GeneLLMの制約と限界は何ですか",
        "GeneLLMの実装上の特徴を教えてください",
        "GeneLLMの動作原理について説明してください",
        "GeneLLMの推論フェーズはどのように機能しますか",
        "GeneLLMの生成フェーズについて教えてください",
        "GeneLLMの入力処理フェーズはどのように機能しますか"
    ],
    "AI": [
        "人工知能の歴史について教えてください",
        "機械学習とディープラーニングの違いは何ですか",
        "ニューラルネットワークとは何ですか",
        "自然言語処理の基本概念を説明してください",
        "強化学習とは何ですか",
        "教師あり学習と教師なし学習の違いは何ですか",
        "AIの倫理的課題について教えてください",
        "AIの将来展望はどうなっていますか",
        "転移学習とは何ですか",
        "AIの主要なアプローチにはどのようなものがありますか"
    ],
    "プログラミング": [
        "C言語の特徴について教えてください",
        "オブジェクト指向プログラミングとは何ですか",
        "関数型プログラミングの特徴は何ですか",
        "データ構造とアルゴリズムの重要性について教えてください",
        "メモリ管理の基本概念を説明してください",
        "ポインタとは何ですか",
        "コンパイラとインタプリタの違いは何ですか",
        "デバッグの効果的な方法を教えてください",
        "バージョン管理システムの役割は何ですか",
        "プログラミングパラダイムの種類について教えてください"
    ],
    "自然言語処理": [
        "形態素解析とは何ですか",
        "単語埋め込みについて説明してください",
        "トランスフォーマーアーキテクチャとは何ですか",
        "BERTモデルの特徴は何ですか",
        "日本語NLPの課題について教えてください",
        "感情分析の手法について説明してください",
        "機械翻訳の歴史と発展について教えてください",
        "質問応答システムの仕組みを説明してください",
        "テキスト要約の手法にはどのようなものがありますか",
        "自然言語生成の評価指標について教えてください"
    ],
    "量子コンピュータ": [
        "量子コンピュータとは何ですか",
        "量子ビット（キュービット）について説明してください",
        "量子重ね合わせとは何ですか",
        "量子もつれについて教えてください",
        "ショアのアルゴリズムとは何ですか",
        "グローバーのアルゴリズムについて説明してください",
        "量子コンピュータの現状と課題は何ですか",
        "量子エラー訂正について教えてください",
        "量子コンピュータの応用分野にはどのようなものがありますか",
        "量子優位性とは何ですか"
    ],
    "数学": [
        "微分積分学の基本概念を説明してください",
        "線形代数とは何ですか",
        "確率論の基礎について教えてください",
        "統計学の重要性について説明してください",
        "フーリエ変換とは何ですか",
        "集合論の基本概念を教えてください",
        "数学的帰納法について説明してください",
        "複素数とは何ですか",
        "グラフ理論の応用例を教えてください",
        "暗号理論の基礎について説明してください"
    ],
    "物理学": [
        "相対性理論について説明してください",
        "量子力学の基本原理を教えてください",
        "熱力学の法則について説明してください",
        "電磁気学の基礎概念を教えてください",
        "素粒子物理学とは何ですか",
        "宇宙論の現代的な理解について教えてください",
        "流体力学の応用例を説明してください",
        "光学の基本原理について教えてください",
        "音響学の基礎を説明してください",
        "物理学における対称性の重要性について教えてください"
    ],
    "健康": [
        "健康的な食事の基本原則を教えてください",
        "効果的な運動方法について説明してください",
        "睡眠の質を向上させる方法を教えてください",
        "ストレス管理のテクニックについて説明してください",
        "心身の健康を維持するためのバランスについて教えてください",
        "免疫システムを強化する方法を説明してください",
        "健康的な体重管理の方法について教えてください",
        "メンタルヘルスケアの重要性について説明してください",
        "予防医学の基本概念を教えてください",
        "健康的な生活習慣の形成方法について説明してください"
    ],
    "料理": [
        "基本的な調理技術について教えてください",
        "和食の特徴と基本的な料理法を説明してください",
        "イタリア料理の基本について教えてください",
        "フランス料理の特徴と基本技術を説明してください",
        "中華料理の基本的な調理法について教えてください",
        "調味料の適切な使い方を説明してください",
        "食材の保存方法について教えてください",
        "健康的な食事の準備方法を説明してください",
        "ベジタリアン料理の基本について教えてください",
        "季節の食材を活かした料理について説明してください"
    ],
    "旅行": [
        "効率的な旅行計画の立て方を教えてください",
        "バックパッキングの基本について説明してください",
        "海外旅行の準備リストを教えてください",
        "文化的に敏感な旅行者になる方法について説明してください",
        "予算内で旅行する方法を教えてください",
        "持続可能な旅行の実践方法について説明してください",
        "ソロ旅行のコツを教えてください",
        "家族旅行を成功させるためのヒントを説明してください",
        "旅行保険の重要性について教えてください",
        "現地の文化を尊重しながら旅行する方法を説明してください"
    ]
}

def run_benchmark():
    """ベンチマークを実行する関数"""
    results = {}
    all_questions = []
    
    # すべての質問をリストに追加
    for genre, questions in QUESTIONS.items():
        for question in questions:
            all_questions.append({"genre": genre, "question": question})
    
    # 質問をランダムに並べ替え
    random.shuffle(all_questions)
    
    # 結果ディレクトリを作成
    os.makedirs("benchmark_results", exist_ok=True)
    
    # 現在の日時を取得してファイル名に使用
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    result_file = f"benchmark_results/genre_benchmark_{timestamp}.json"
    
    total_questions = len(all_questions)
    correct_answers = 0
    
    print(f"ジャンルベンチマークを開始します。合計 {total_questions} 問")
    
    for i, item in enumerate(all_questions):
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
            
            print(f"回答: {answer}")
            print(f"応答時間: {response_time:.2f}秒")
            
            # 結果を保存
            if genre not in results:
                results[genre] = {
                    "questions": [],
                    "correct": 0,
                    "total": 0,
                    "avg_time": 0
                }
            
            results[genre]["questions"].append({
                "question": question,
                "answer": answer,
                "response_time": response_time
            })
            
            results[genre]["total"] += 1
            results[genre]["avg_time"] = (results[genre]["avg_time"] * (results[genre]["total"] - 1) + response_time) / results[genre]["total"]
            
            # 一時的に結果を保存
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
        print(f"質問数: {data['total']}")
        print(f"平均応答時間: {data['avg_time']:.2f}秒")
    
    print(f"\n結果は {result_file} に保存されました。")

if __name__ == "__main__":
    run_benchmark()