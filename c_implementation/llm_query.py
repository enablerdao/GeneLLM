#!/usr/bin/env python3
import sys
import json
import os
import requests
from typing import Dict, Any, Optional

# 環境変数からAPIキーを取得
API_URL = "https://api.openai.com/v1/chat/completions"

def get_api_key() -> str:
    """
    APIキーを取得する。環境変数になければ引数から取得し、それもなければユーザーに入力を促す
    
    Returns:
        APIキー
    """
    # 環境変数からAPIキーを取得
    api_key = os.environ.get("OPENAI_API_KEY")
    
    # 環境変数になければコマンドライン引数から取得
    if not api_key and len(sys.argv) > 2:
        api_key = sys.argv[2]
    
    # それもなければユーザーに入力を促す
    if not api_key:
        print("OpenAI APIキーが必要です。")
        print("APIキーを入力してください（入力しない場合はシミュレーションモードで動作します）:")
        api_key = input().strip()
    
    return api_key if api_key else "dummy_key"

def query_llm(prompt: str, model: str = "gpt-3.5-turbo") -> Optional[str]:
    """
    外部LLMに質問を送信し、回答を取得する
    
    Args:
        prompt: 質問文
        model: 使用するモデル名
        
    Returns:
        回答テキスト、エラー時はNone
    """
    # APIキーを取得
    API_KEY = get_api_key()
    
    # APIキーが設定されていない場合はシミュレーションモード
    if API_KEY == "dummy_key":
        print("APIキーが設定されていないため、シミュレーションモードで動作します")
        return simulate_llm_response(prompt)
    
    headers = {
        "Content-Type": "application/json",
        "Authorization": f"Bearer {API_KEY}"
    }
    
    data = {
        "model": model,
        "messages": [{"role": "user", "content": prompt}],
        "temperature": 0.7,
        "max_tokens": 500
    }
    
    try:
        response = requests.post(API_URL, headers=headers, json=data, timeout=30)
        response.raise_for_status()
        result = response.json()
        return result["choices"][0]["message"]["content"].strip()
    except Exception as e:
        print(f"LLM APIへのリクエスト中にエラーが発生しました: {e}", file=sys.stderr)
        return None

def simulate_llm_response(prompt: str) -> str:
    """
    APIキーがない場合のシミュレーション回答
    
    Args:
        prompt: 質問文
        
    Returns:
        シミュレーションされた回答
    """
    # 質問に基づいて簡単な回答を生成
    if "料理" in prompt:
        return "料理の基本は、新鮮な食材を選び、適切な調理法を使うことです。初心者は簡単なレシピから始め、徐々に技術を磨いていくことをお勧めします。"
    elif "投資" in prompt:
        return "投資を始める際は、まず緊急資金を確保し、リスク許容度を理解した上で、分散投資を心がけることが重要です。長期的な視点で投資し、定期的に見直しを行いましょう。"
    elif "旅行" in prompt:
        return "旅行計画では、目的地の情報収集、予算設定、交通手段と宿泊先の予約が重要です。現地の文化や習慣を尊重し、柔軟なスケジュールを立てることで、より充実した旅行体験ができます。"
    elif "健康" in prompt or "フィットネス" in prompt:
        return "健康維持には、バランスの良い食事、定期的な運動、十分な睡眠が重要です。自分の体力レベルに合った運動から始め、徐々に強度を上げていくことをお勧めします。"
    else:
        return "申し訳ありませんが、その質問に対する具体的な情報を持っていません。もう少し詳しく教えていただけますか？"

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("使用方法: python llm_query.py '質問文'", file=sys.stderr)
        sys.exit(1)
    
    prompt = sys.argv[1]
    response = query_llm(prompt)
    
    if response:
        print(response)
    else:
        print("回答を取得できませんでした。")