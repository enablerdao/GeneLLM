#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
複数のベンチマーク結果を比較するスクリプト
"""

import os
import sys
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import argparse
from matplotlib.font_manager import FontProperties
import glob
from datetime import datetime

# 引数の解析
parser = argparse.ArgumentParser(description='複数のベンチマーク結果を比較します')
parser.add_argument('--dirs', nargs='+', help='比較するベンチマーク結果ディレクトリのリスト')
parser.add_argument('--labels', nargs='+', help='各ディレクトリのラベル')
parser.add_argument('--output', default='benchmark/results/comparison', help='出力ディレクトリ')
args = parser.parse_args()

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

# ディレクトリが指定されていない場合は、最新の2つのベンチマーク結果を使用
if not args.dirs:
    result_dirs = sorted(glob.glob('benchmark/results/20*'))
    if len(result_dirs) < 2:
        print("エラー: 比較するベンチマーク結果が2つ以上必要です")
        print("使用方法: python benchmark/compare_benchmarks.py --dirs dir1 dir2 [dir3 ...] --labels label1 label2 [label3 ...]")
        sys.exit(1)
    args.dirs = result_dirs[-2:]  # 最新の2つを使用
    
    # ラベルが指定されていない場合はディレクトリ名を使用
    if not args.labels:
        args.labels = [os.path.basename(d) for d in args.dirs]

# ラベルの数がディレクトリの数と一致しない場合
if args.labels and len(args.labels) != len(args.dirs):
    print(f"警告: ラベルの数 ({len(args.labels)}) がディレクトリの数 ({len(args.dirs)}) と一致しません")
    # ラベルが足りない場合は、ディレクトリ名を使用
    if len(args.labels) < len(args.dirs):
        args.labels.extend([os.path.basename(d) for d in args.dirs[len(args.labels):]])
    # ラベルが多い場合は、余分なラベルを削除
    else:
        args.labels = args.labels[:len(args.dirs)]

# 出力ディレクトリを作成
os.makedirs(args.output, exist_ok=True)
print(f"出力ディレクトリを作成しました: {args.output}")

# 各ディレクトリからCSVファイルを読み込む
dataframes = []
for i, directory in enumerate(args.dirs):
    csv_file = os.path.join(directory, 'benchmark_results.csv')
    if not os.path.exists(csv_file):
        print(f"警告: {csv_file} が見つかりません。スキップします。")
        continue
    
    try:
        df = pd.read_csv(csv_file)
        df['ベンチマーク'] = args.labels[i]
        dataframes.append(df)
        print(f"CSVファイルを読み込みました: {csv_file}")
    except Exception as e:
        print(f"CSVファイルの読み込みエラー: {e}")
        continue

if not dataframes:
    print("エラー: 有効なCSVファイルが見つかりませんでした")
    sys.exit(1)

# データフレームを結合
combined_df = pd.concat(dataframes, ignore_index=True)

# 応答時間の比較グラフ
plt.figure(figsize=(14, 8))
for label in combined_df['ベンチマーク'].unique():
    subset = combined_df[combined_df['ベンチマーク'] == label]
    plt.plot(subset['質問番号'], subset['応答時間(秒)'], marker='o', label=label)

plt.xlabel('質問番号')
plt.ylabel('応答時間 (秒)')
plt.title('ベンチマーク間の応答時間比較')
plt.grid(linestyle='--', alpha=0.7)
plt.xticks(combined_df['質問番号'].unique())
plt.legend()
plt.tight_layout()
plt.savefig(f"{args.output}/response_time_comparison.png", dpi=300)
print(f"応答時間比較グラフを保存しました: {args.output}/response_time_comparison.png")

# 評価スコアの比較グラフ
plt.figure(figsize=(14, 8))
for label in combined_df['ベンチマーク'].unique():
    subset = combined_df[combined_df['ベンチマーク'] == label]
    plt.plot(subset['質問番号'], subset['評価スコア'], marker='o', label=label)

plt.xlabel('質問番号')
plt.ylabel('評価スコア (0-10)')
plt.title('ベンチマーク間の評価スコア比較')
plt.grid(linestyle='--', alpha=0.7)
plt.xticks(combined_df['質問番号'].unique())
plt.ylim(0, 10)
plt.legend()
plt.tight_layout()
plt.savefig(f"{args.output}/evaluation_score_comparison.png", dpi=300)
print(f"評価スコア比較グラフを保存しました: {args.output}/evaluation_score_comparison.png")

# 応答文字数の比較グラフ
plt.figure(figsize=(14, 8))
for label in combined_df['ベンチマーク'].unique():
    subset = combined_df[combined_df['ベンチマーク'] == label]
    plt.plot(subset['質問番号'], subset['応答文字数'], marker='o', label=label)

plt.xlabel('質問番号')
plt.ylabel('応答文字数')
plt.title('ベンチマーク間の応答文字数比較')
plt.grid(linestyle='--', alpha=0.7)
plt.xticks(combined_df['質問番号'].unique())
plt.legend()
plt.tight_layout()
plt.savefig(f"{args.output}/response_length_comparison.png", dpi=300)
print(f"応答文字数比較グラフを保存しました: {args.output}/response_length_comparison.png")

# 平均値の比較（棒グラフ）
plt.figure(figsize=(12, 8))
avg_data = combined_df.groupby('ベンチマーク').agg({
    '応答時間(秒)': 'mean',
    '評価スコア': 'mean',
    '応答文字数': 'mean'
}).reset_index()

# 応答時間の平均
plt.subplot(3, 1, 1)
bars = plt.bar(avg_data['ベンチマーク'], avg_data['応答時間(秒)'], color='skyblue')
plt.title('平均応答時間の比較')
plt.ylabel('秒')
plt.grid(axis='y', linestyle='--', alpha=0.7)
for bar in bars:
    height = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2., height + 0.05,
             f'{height:.2f}', ha='center', va='bottom')

# 評価スコアの平均
plt.subplot(3, 1, 2)
bars = plt.bar(avg_data['ベンチマーク'], avg_data['評価スコア'], color='lightgreen')
plt.title('平均評価スコアの比較')
plt.ylabel('スコア (0-10)')
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.ylim(0, 10)
for bar in bars:
    height = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2., height + 0.05,
             f'{height:.1f}', ha='center', va='bottom')

# 応答文字数の平均
plt.subplot(3, 1, 3)
bars = plt.bar(avg_data['ベンチマーク'], avg_data['応答文字数'], color='salmon')
plt.title('平均応答文字数の比較')
plt.ylabel('文字数')
plt.grid(axis='y', linestyle='--', alpha=0.7)
for bar in bars:
    height = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2., height + 0.05,
             f'{int(height)}', ha='center', va='bottom')

plt.tight_layout()
plt.savefig(f"{args.output}/averages_comparison.png", dpi=300)
print(f"平均値比較グラフを保存しました: {args.output}/averages_comparison.png")

# サマリーレポートを生成
summary_file = f"{args.output}/comparison_summary.md"
with open(summary_file, 'w') as f:
    f.write("# ベンチマーク比較結果\n\n")
    f.write(f"生成日時: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
    
    f.write("## 比較対象\n\n")
    for i, (directory, label) in enumerate(zip(args.dirs, args.labels)):
        f.write(f"{i+1}. **{label}**: `{directory}`\n")
    f.write("\n")
    
    f.write("## 統計情報\n\n")
    f.write("| ベンチマーク | 平均応答時間 | 平均評価スコア | 平均応答文字数 |\n")
    f.write("|------------|------------|--------------|------------|\n")
    
    for _, row in avg_data.iterrows():
        f.write(f"| {row['ベンチマーク']} | {row['応答時間(秒)']:.2f}秒 | {row['評価スコア']:.1f}/10 | {int(row['応答文字数'])}文字 |\n")
    
    f.write("\n## 比較グラフ\n\n")
    f.write("### 応答時間の比較\n")
    f.write("![応答時間比較](response_time_comparison.png)\n\n")
    
    f.write("### 評価スコアの比較\n")
    f.write("![評価スコア比較](evaluation_score_comparison.png)\n\n")
    
    f.write("### 応答文字数の比較\n")
    f.write("![応答文字数比較](response_length_comparison.png)\n\n")
    
    f.write("### 平均値の比較\n")
    f.write("![平均値比較](averages_comparison.png)\n\n")
    
    # 改善点や差異の分析
    f.write("## 分析\n\n")
    
    # 応答時間の分析
    fastest = avg_data.loc[avg_data['応答時間(秒)'].idxmin()]
    slowest = avg_data.loc[avg_data['応答時間(秒)'].idxmax()]
    time_diff = slowest['応答時間(秒)'] - fastest['応答時間(秒)']
    time_diff_percent = (time_diff / slowest['応答時間(秒)']) * 100
    
    f.write("### 応答時間\n\n")
    f.write(f"- 最速: **{fastest['ベンチマーク']}** ({fastest['応答時間(秒)']:.2f}秒)\n")
    f.write(f"- 最遅: **{slowest['ベンチマーク']}** ({slowest['応答時間(秒)']:.2f}秒)\n")
    f.write(f"- 差異: {time_diff:.2f}秒 ({time_diff_percent:.1f}%)\n\n")
    
    # 評価スコアの分析
    best = avg_data.loc[avg_data['評価スコア'].idxmax()]
    worst = avg_data.loc[avg_data['評価スコア'].idxmin()]
    score_diff = best['評価スコア'] - worst['評価スコア']
    score_diff_percent = (score_diff / best['評価スコア']) * 100
    
    f.write("### 評価スコア\n\n")
    f.write(f"- 最高: **{best['ベンチマーク']}** ({best['評価スコア']:.1f}/10)\n")
    f.write(f"- 最低: **{worst['ベンチマーク']}** ({worst['評価スコア']:.1f}/10)\n")
    f.write(f"- 差異: {score_diff:.1f}ポイント ({score_diff_percent:.1f}%)\n\n")
    
    # 応答文字数の分析
    longest = avg_data.loc[avg_data['応答文字数'].idxmax()]
    shortest = avg_data.loc[avg_data['応答文字数'].idxmin()]
    length_diff = longest['応答文字数'] - shortest['応答文字数']
    length_diff_percent = (length_diff / longest['応答文字数']) * 100
    
    f.write("### 応答文字数\n\n")
    f.write(f"- 最長: **{longest['ベンチマーク']}** ({int(longest['応答文字数'])}文字)\n")
    f.write(f"- 最短: **{shortest['ベンチマーク']}** ({int(shortest['応答文字数'])}文字)\n")
    f.write(f"- 差異: {int(length_diff)}文字 ({length_diff_percent:.1f}%)\n\n")
    
    f.write("### 総合評価\n\n")
    f.write("各ベンチマークの特徴:\n\n")
    
    for _, row in avg_data.iterrows():
        f.write(f"**{row['ベンチマーク']}**:\n")
        
        # 応答時間の特徴
        if row['応答時間(秒)'] == fastest['応答時間(秒)']:
            f.write("- 応答時間が最も速い\n")
        elif row['応答時間(秒)'] == slowest['応答時間(秒)']:
            f.write("- 応答時間が最も遅い\n")
        else:
            f.write(f"- 応答時間は平均的 ({row['応答時間(秒)']:.2f}秒)\n")
        
        # 評価スコアの特徴
        if row['評価スコア'] == best['評価スコア']:
            f.write("- 評価スコアが最も高い\n")
        elif row['評価スコア'] == worst['評価スコア']:
            f.write("- 評価スコアが最も低い\n")
        else:
            f.write(f"- 評価スコアは平均的 ({row['評価スコア']:.1f}/10)\n")
        
        # 応答文字数の特徴
        if row['応答文字数'] == longest['応答文字数']:
            f.write("- 応答文字数が最も多い\n")
        elif row['応答文字数'] == shortest['応答文字数']:
            f.write("- 応答文字数が最も少ない\n")
        else:
            f.write(f"- 応答文字数は平均的 ({int(row['応答文字数'])}文字)\n")
        
        f.write("\n")

print(f"比較サマリーレポートを生成しました: {summary_file}")
print("比較グラフ生成が完了しました！")