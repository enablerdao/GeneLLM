#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
ベンチマーク結果からグラフを生成するスクリプト
"""

import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.font_manager import FontProperties
import sys

# フォントの設定（日本語対応）
try:
    # Ubuntuの場合
    font_path = '/usr/share/fonts/truetype/takao-gothic/TakaoGothic.ttf'
    if os.path.exists(font_path):
        font_prop = FontProperties(fname=font_path)
        plt.rcParams['font.family'] = font_prop.get_name()
    # macOSの場合
    elif os.path.exists('/System/Library/Fonts/ヒラギノ角ゴシック W3.ttc'):
        font_prop = FontProperties(fname='/System/Library/Fonts/ヒラギノ角ゴシック W3.ttc')
        plt.rcParams['font.family'] = font_prop.get_name()
    else:
        print("警告: 日本語フォントが見つかりません。グラフのテキストが正しく表示されない可能性があります。")
except Exception as e:
    print(f"フォント設定エラー: {e}")
    print("警告: 日本語フォントの設定に失敗しました。グラフのテキストが正しく表示されない可能性があります。")

# 最新の結果ディレクトリを使用
latest_dir = 'benchmark/results/latest'

# CSVファイルのパス
csv_file = f'{latest_dir}/benchmark_results.csv'

# 出力ディレクトリ
output_dir = f'{latest_dir}/charts'
os.makedirs(output_dir, exist_ok=True)

# CSVファイルが存在するか確認
if not os.path.exists(csv_file):
    print(f"エラー: {csv_file} が見つかりません。先にベンチマークを実行してください。")
    sys.exit(1)

# CSVファイルを読み込む
try:
    df = pd.read_csv(csv_file)
    print(f"CSVファイルを読み込みました: {len(df)} 行のデータ")
except Exception as e:
    print(f"CSVファイルの読み込みエラー: {e}")
    sys.exit(1)

# 応答時間のグラフ
plt.figure(figsize=(12, 6))
plt.bar(df['質問番号'], df['応答時間(秒)'], color='skyblue')
plt.xlabel('質問番号')
plt.ylabel('応答時間 (秒)')
plt.title('質問ごとの応答時間')
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.xticks(df['質問番号'])
plt.tight_layout()
plt.savefig(f"{output_dir}/response_time.png", dpi=300)
print(f"応答時間のグラフを保存しました: {output_dir}/response_time.png")

# 評価スコアのグラフ
plt.figure(figsize=(12, 6))
plt.bar(df['質問番号'], df['評価スコア'], color='lightgreen')
plt.xlabel('質問番号')
plt.ylabel('評価スコア (0-10)')
plt.title('質問ごとの評価スコア')
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.xticks(df['質問番号'])
plt.ylim(0, 10)
plt.tight_layout()
plt.savefig(f"{output_dir}/evaluation_score.png", dpi=300)
print(f"評価スコアのグラフを保存しました: {output_dir}/evaluation_score.png")

# 応答文字数のグラフ
plt.figure(figsize=(12, 6))
plt.bar(df['質問番号'], df['応答文字数'], color='salmon')
plt.xlabel('質問番号')
plt.ylabel('応答文字数')
plt.title('質問ごとの応答文字数')
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.xticks(df['質問番号'])
plt.tight_layout()
plt.savefig(f"{output_dir}/response_length.png", dpi=300)
print(f"応答文字数のグラフを保存しました: {output_dir}/response_length.png")

# 散布図（応答時間 vs 評価スコア）
plt.figure(figsize=(10, 6))
plt.scatter(df['応答時間(秒)'], df['評価スコア'], c=df['質問番号'], cmap='viridis', s=100, alpha=0.7)
plt.colorbar(label='質問番号')
plt.xlabel('応答時間 (秒)')
plt.ylabel('評価スコア (0-10)')
plt.title('応答時間 vs 評価スコア')
plt.grid(linestyle='--', alpha=0.7)
plt.tight_layout()
plt.savefig(f"{output_dir}/time_vs_score.png", dpi=300)
print(f"散布図を保存しました: {output_dir}/time_vs_score.png")

# 散布図（応答文字数 vs 評価スコア）
plt.figure(figsize=(10, 6))
plt.scatter(df['応答文字数'], df['評価スコア'], c=df['質問番号'], cmap='plasma', s=100, alpha=0.7)
plt.colorbar(label='質問番号')
plt.xlabel('応答文字数')
plt.ylabel('評価スコア (0-10)')
plt.title('応答文字数 vs 評価スコア')
plt.grid(linestyle='--', alpha=0.7)
plt.tight_layout()
plt.savefig(f"{output_dir}/length_vs_score.png", dpi=300)
print(f"散布図を保存しました: {output_dir}/length_vs_score.png")

# サマリーレポートを生成
summary_file = f"{output_dir}/summary.md"
with open(summary_file, 'w') as f:
    f.write("# ベンチマーク結果サマリー\n\n")
    
    f.write("## 統計情報\n\n")
    f.write(f"- 質問数: {len(df)}\n")
    f.write(f"- 平均応答時間: {df['応答時間(秒)'].mean():.2f} 秒\n")
    f.write(f"- 最短応答時間: {df['応答時間(秒)'].min():.2f} 秒\n")
    f.write(f"- 最長応答時間: {df['応答時間(秒)'].max():.2f} 秒\n")
    f.write(f"- 平均評価スコア: {df['評価スコア'].mean():.1f}/10\n")
    f.write(f"- 最低評価スコア: {df['評価スコア'].min()}/10\n")
    f.write(f"- 最高評価スコア: {df['評価スコア'].max()}/10\n")
    f.write(f"- 平均応答文字数: {int(df['応答文字数'].mean())} 文字\n\n")
    
    f.write("## グラフ\n\n")
    f.write("### 応答時間\n")
    f.write("![応答時間](response_time.png)\n\n")
    
    f.write("### 評価スコア\n")
    f.write("![評価スコア](evaluation_score.png)\n\n")
    
    f.write("### 応答文字数\n")
    f.write("![応答文字数](response_length.png)\n\n")
    
    f.write("### 応答時間 vs 評価スコア\n")
    f.write("![応答時間 vs 評価スコア](time_vs_score.png)\n\n")
    
    f.write("### 応答文字数 vs 評価スコア\n")
    f.write("![応答文字数 vs 評価スコア](length_vs_score.png)\n\n")

print(f"サマリーレポートを生成しました: {summary_file}")
print("グラフ生成が完了しました！")