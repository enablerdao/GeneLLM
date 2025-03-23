#!/usr/bin/env python3
"""
LLMベンチマークスクリプト
異なる質問に対するルーターモデルの応答時間と正確性を測定します。
"""

import subprocess
import time
import json
import os
import argparse
from typing import List, Dict, Any, Tuple

# ベンチマーク用の質問リスト
DEFAULT_QUESTIONS = [
    "Pythonでウェブスクレイピングをする方法を教えてください",
    "Javaでファイル操作をする方法を教えてください",
    "C言語で電卓プログラムを作るには？",
    "JavaScriptでTodoリストアプリを作成する方法は？",
    "量子コンピュータの仕組みについて教えてください",
    "機械学習の基本的な概念を説明してください",
    "効率的な学習方法はありますか？",
    "健康的な食事について教えてください",
    "投資の始め方を教えてください",
    "旅行の計画を立てるコツはありますか？"
]

# 期待される応答キーワード（簡易評価用）
EXPECTED_KEYWORDS = {
    "Pythonでウェブスクレイピングをする方法を教えてください": ["requests", "BeautifulSoup", "スクレイピング", "ウェブページ", "HTML"],
    "Javaでファイル操作をする方法を教えてください": ["java.io", "File", "BufferedReader", "FileWriter", "Path"],
    "C言語で電卓プログラムを作るには？": ["stdio.h", "main", "scanf", "printf", "switch"],
    "JavaScriptでTodoリストアプリを作成する方法は？": ["function", "addEventListener", "localStorage", "DOM", "HTML"],
    "量子コンピュータの仕組みについて教えてください": ["量子ビット", "キュービット", "重ね合わせ", "量子力学", "計算"],
    "機械学習の基本的な概念を説明してください": ["学習", "モデル", "データ", "アルゴリズム", "予測"],
    "効率的な学習方法はありますか？": ["集中", "復習", "計画", "実践", "理解"],
    "健康的な食事について教えてください": ["バランス", "野菜", "タンパク質", "栄養", "食事"],
    "投資の始め方を教えてください": ["資金", "リスク", "分散", "株式", "計画"],
    "旅行の計画を立てるコツはありますか？": ["予約", "スケジュール", "予算", "情報", "準備"]
}

def run_router_model(question: str, debug: bool = False) -> Tuple[str, float]:
    """
    ルーターモデルを実行して応答と実行時間を取得します。
    
    Args:
        question: 質問文
        debug: デバッグモードを有効にするかどうか
    
    Returns:
        応答文と実行時間（秒）のタプル
    """
    start_time = time.time()
    
    cmd = ["./main", "router"]
    if debug:
        cmd.append("-d")
    cmd.append(question)
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=False, check=True)
        response = result.stdout.decode('utf-8', errors='replace').strip()
    except subprocess.CalledProcessError as e:
        response = f"エラー: {e.stderr.decode('utf-8', errors='replace') if e.stderr else 'Unknown error'}"
    except UnicodeDecodeError:
        response = "エラー: 出力のデコードに失敗しました"
    
    end_time = time.time()
    execution_time = end_time - start_time
    
    return response, execution_time

def extract_actual_response(response: str) -> str:
    """
    デバッグ情報を含む応答から実際の回答部分を抽出します
    
    Args:
        response: モデルからの応答（デバッグ情報を含む）
    
    Returns:
        実際の回答部分
    """
    # 推論ルールが適用された場合、その結果を抽出
    if "推論ルールが適用されました:" in response:
        parts = response.split("推論ルールが適用されました:", 1)
        if len(parts) > 1:
            return parts[1].strip()
    
    # 最終的な応答を探す（デバッグ情報の後）
    lines = response.split('\n')
    for i in range(len(lines) - 1, -1, -1):
        if lines[i].strip() and not lines[i].startswith("ルール") and not "一致:" in lines[i] and not "不一致:" in lines[i]:
            return lines[i].strip()
    
    # 見つからない場合は元の応答を返す
    return response

def evaluate_response(response: str, expected_keywords: List[str]) -> float:
    """
    応答の質を評価します（キーワードの一致率）
    
    Args:
        response: モデルからの応答
        expected_keywords: 期待されるキーワードのリスト
    
    Returns:
        0.0〜1.0の評価スコア
    """
    if not expected_keywords:
        return 0.0
    
    # 実際の回答部分を抽出
    actual_response = extract_actual_response(response)
    
    matched_keywords = 0
    for keyword in expected_keywords:
        if keyword in actual_response or keyword in response:
            matched_keywords += 1
    
    return matched_keywords / len(expected_keywords)

def run_benchmark(questions: List[str] = None, debug: bool = False, output_file: str = None) -> Dict[str, Any]:
    """
    ベンチマークを実行します。
    
    Args:
        questions: 質問のリスト（Noneの場合はデフォルトの質問を使用）
        debug: デバッグモードを有効にするかどうか
        output_file: 結果を保存するJSONファイルのパス
    
    Returns:
        ベンチマーク結果の辞書
    """
    if questions is None:
        questions = DEFAULT_QUESTIONS
    
    results = {
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
        "total_questions": len(questions),
        "debug_mode": debug,
        "questions": []
    }
    
    total_time = 0
    total_score = 0
    
    print(f"ベンチマークを開始します（質問数: {len(questions)}）...")
    
    for i, question in enumerate(questions, 1):
        print(f"\n[{i}/{len(questions)}] 質問: {question}")
        
        response, exec_time = run_router_model(question, debug)
        total_time += exec_time
        
        # 応答の評価
        expected_kw = EXPECTED_KEYWORDS.get(question, [])
        score = evaluate_response(response, expected_kw)
        total_score += score
        
        # デバッグ情報を含む応答から実際の回答部分を抽出
        actual_response = extract_actual_response(response)
        
        # 応答の一部を表示（長すぎる場合は省略）
        display_response = actual_response[:200] + "..." if len(actual_response) > 200 else actual_response
        print(f"応答: {display_response}")
        print(f"実行時間: {exec_time:.2f}秒")
        print(f"評価スコア: {score:.2f}")
        
        # 結果を記録
        results["questions"].append({
            "question": question,
            "response": response,
            "actual_response": actual_response,
            "execution_time": exec_time,
            "score": score,
            "expected_keywords": expected_kw
        })
    
    # 集計結果
    results["total_execution_time"] = total_time
    results["average_execution_time"] = total_time / len(questions)
    results["average_score"] = total_score / len(questions)
    
    print("\n===== ベンチマーク結果 =====")
    print(f"合計実行時間: {total_time:.2f}秒")
    print(f"平均実行時間: {results['average_execution_time']:.2f}秒/質問")
    print(f"平均評価スコア: {results['average_score']:.2f}")
    
    # 結果をファイルに保存
    if output_file:
        with open(output_file, 'w', encoding='utf-8') as f:
            json.dump(results, f, ensure_ascii=False, indent=2)
        print(f"\n結果を {output_file} に保存しました。")
    
    return results

def main():
    parser = argparse.ArgumentParser(description='LLMベンチマークツール')
    parser.add_argument('-d', '--debug', action='store_true', help='デバッグモードを有効にする')
    parser.add_argument('-o', '--output', type=str, default='benchmark_results.json', help='結果を保存するJSONファイル')
    parser.add_argument('-q', '--questions', type=str, help='質問リストを含むJSONファイル')
    
    args = parser.parse_args()
    
    # カスタム質問リストの読み込み
    questions = None
    if args.questions and os.path.exists(args.questions):
        try:
            with open(args.questions, 'r', encoding='utf-8') as f:
                questions_data = json.load(f)
                if isinstance(questions_data, list):
                    questions = questions_data
                elif isinstance(questions_data, dict) and 'questions' in questions_data:
                    questions = questions_data['questions']
        except Exception as e:
            print(f"質問ファイルの読み込みエラー: {e}")
            print("デフォルトの質問リストを使用します。")
    
    run_benchmark(questions, args.debug, args.output)

if __name__ == "__main__":
    main()