#!/bin/bash

# 拡張DNA辞書を生成するスクリプト
# 使用方法: ./generate_enhanced_dictionary.sh

WORKSPACE_DIR="/workspace"
JAPANESE_WORDS_FILE="$WORKSPACE_DIR/knowledge/text/japanese_words.txt"
ENHANCED_DNA_DICT_FILE="$WORKSPACE_DIR/data/enhanced_dna_dictionary.txt"
TEMP_DIR="/tmp/genellm_dna"

# 必要なディレクトリを作成
mkdir -p "$TEMP_DIR"

# 既存の辞書をバックアップ
if [ -f "$ENHANCED_DNA_DICT_FILE" ]; then
    cp "$ENHANCED_DNA_DICT_FILE" "${ENHANCED_DNA_DICT_FILE}.bak"
    echo "既存の辞書をバックアップしました: ${ENHANCED_DNA_DICT_FILE}.bak"
fi

# 新しい辞書ファイルを作成
> "$ENHANCED_DNA_DICT_FILE"

echo "拡張DNA辞書を生成しています..."

# 基本的なエントリを追加
echo "基本エントリを追加しています..."

# 主語（Entity）を追加
echo "E0|GeneLLM" >> "$ENHANCED_DNA_DICT_FILE"
echo "E1|シェルスクリプト" >> "$ENHANCED_DNA_DICT_FILE"
echo "E2|並列処理" >> "$ENHANCED_DNA_DICT_FILE"
echo "E3|トークナイザー" >> "$ENHANCED_DNA_DICT_FILE"
echo "E4|ベクトル検索" >> "$ENHANCED_DNA_DICT_FILE"
echo "E5|知識グラフ" >> "$ENHANCED_DNA_DICT_FILE"
echo "E6|自然言語処理" >> "$ENHANCED_DNA_DICT_FILE"
echo "E7|機械学習" >> "$ENHANCED_DNA_DICT_FILE"
echo "E8|深層学習" >> "$ENHANCED_DNA_DICT_FILE"
echo "E9|強化学習" >> "$ENHANCED_DNA_DICT_FILE"
echo "E10|遺伝的アルゴリズム" >> "$ENHANCED_DNA_DICT_FILE"
echo "E11|自然言語" >> "$ENHANCED_DNA_DICT_FILE"

# 動詞（Concept）を追加
echo "C0|する" >> "$ENHANCED_DNA_DICT_FILE"
echo "C1|実装する" >> "$ENHANCED_DNA_DICT_FILE"
echo "C2|高速化する" >> "$ENHANCED_DNA_DICT_FILE"
echo "C3|解析する" >> "$ENHANCED_DNA_DICT_FILE"
echo "C4|生成する" >> "$ENHANCED_DNA_DICT_FILE"
echo "C5|最適化する" >> "$ENHANCED_DNA_DICT_FILE"
echo "C6|変換する" >> "$ENHANCED_DNA_DICT_FILE"
echo "C7|抽出する" >> "$ENHANCED_DNA_DICT_FILE"
echo "C8|分類する" >> "$ENHANCED_DNA_DICT_FILE"
echo "C9|予測する" >> "$ENHANCED_DNA_DICT_FILE"
echo "C10|学習する" >> "$ENHANCED_DNA_DICT_FILE"
echo "C11|処理する" >> "$ENHANCED_DNA_DICT_FILE"
echo "C12|処理する" >> "$ENHANCED_DNA_DICT_FILE"

# 目的語（Result）を追加
echo "R0|C言語" >> "$ENHANCED_DNA_DICT_FILE"
echo "R1|知識ベース" >> "$ENHANCED_DNA_DICT_FILE"
echo "R2|更新処理" >> "$ENHANCED_DNA_DICT_FILE"
echo "R3|テキストデータ" >> "$ENHANCED_DNA_DICT_FILE"
echo "R4|バイナリデータ" >> "$ENHANCED_DNA_DICT_FILE"
echo "R5|構造化データ" >> "$ENHANCED_DNA_DICT_FILE"
echo "R6|非構造化データ" >> "$ENHANCED_DNA_DICT_FILE"
echo "R7|メタデータ" >> "$ENHANCED_DNA_DICT_FILE"
echo "R8|特徴量" >> "$ENHANCED_DNA_DICT_FILE"
echo "R9|パターン" >> "$ENHANCED_DNA_DICT_FILE"
echo "R10|モデル" >> "$ENHANCED_DNA_DICT_FILE"
echo "R11|テキスト" >> "$ENHANCED_DNA_DICT_FILE"
echo "R12|言語" >> "$ENHANCED_DNA_DICT_FILE"

# 属性（Attribute）を追加
echo "A0|高速" >> "$ENHANCED_DNA_DICT_FILE"
echo "A1|効率的" >> "$ENHANCED_DNA_DICT_FILE"
echo "A2|安定" >> "$ENHANCED_DNA_DICT_FILE"
echo "A3|信頼性の高い" >> "$ENHANCED_DNA_DICT_FILE"
echo "A4|スケーラブルな" >> "$ENHANCED_DNA_DICT_FILE"
echo "A5|柔軟な" >> "$ENHANCED_DNA_DICT_FILE"
echo "A6|堅牢な" >> "$ENHANCED_DNA_DICT_FILE"
echo "A7|最適化された" >> "$ENHANCED_DNA_DICT_FILE"
echo "A8|先進的な" >> "$ENHANCED_DNA_DICT_FILE"
echo "A9|革新的な" >> "$ENHANCED_DNA_DICT_FILE"
echo "A10|使いやすい" >> "$ENHANCED_DNA_DICT_FILE"

# 時間（Time）を追加
echo "T0|今日" >> "$ENHANCED_DNA_DICT_FILE"
echo "T1|毎日" >> "$ENHANCED_DNA_DICT_FILE"
echo "T2|常時" >> "$ENHANCED_DNA_DICT_FILE"
echo "T3|リアルタイムで" >> "$ENHANCED_DNA_DICT_FILE"
echo "T4|定期的に" >> "$ENHANCED_DNA_DICT_FILE"
echo "T5|週次で" >> "$ENHANCED_DNA_DICT_FILE"
echo "T6|月次で" >> "$ENHANCED_DNA_DICT_FILE"
echo "T7|年次で" >> "$ENHANCED_DNA_DICT_FILE"
echo "T8|将来的に" >> "$ENHANCED_DNA_DICT_FILE"
echo "T9|過去に" >> "$ENHANCED_DNA_DICT_FILE"
echo "T10|現在" >> "$ENHANCED_DNA_DICT_FILE"

# 場所（Location）を追加
echo "L0|メモリ上" >> "$ENHANCED_DNA_DICT_FILE"
echo "L1|ディスク" >> "$ENHANCED_DNA_DICT_FILE"
echo "L2|クラウド" >> "$ENHANCED_DNA_DICT_FILE"
echo "L3|ローカル環境" >> "$ENHANCED_DNA_DICT_FILE"
echo "L4|サーバー上" >> "$ENHANCED_DNA_DICT_FILE"
echo "L5|データベース内" >> "$ENHANCED_DNA_DICT_FILE"
echo "L6|ネットワーク上" >> "$ENHANCED_DNA_DICT_FILE"
echo "L7|コンテナ内" >> "$ENHANCED_DNA_DICT_FILE"
echo "L8|仮想環境" >> "$ENHANCED_DNA_DICT_FILE"
echo "L9|分散システム" >> "$ENHANCED_DNA_DICT_FILE"
echo "L10|エッジデバイス" >> "$ENHANCED_DNA_DICT_FILE"

# 様態（Manner）を追加
echo "M0|効率的に" >> "$ENHANCED_DNA_DICT_FILE"
echo "M1|安全に" >> "$ENHANCED_DNA_DICT_FILE"
echo "M2|自動的に" >> "$ENHANCED_DNA_DICT_FILE"
echo "M3|迅速に" >> "$ENHANCED_DNA_DICT_FILE"
echo "M4|正確に" >> "$ENHANCED_DNA_DICT_FILE"
echo "M5|柔軟に" >> "$ENHANCED_DNA_DICT_FILE"
echo "M6|堅牢に" >> "$ENHANCED_DNA_DICT_FILE"
echo "M7|スケーラブルに" >> "$ENHANCED_DNA_DICT_FILE"
echo "M8|透過的に" >> "$ENHANCED_DNA_DICT_FILE"
echo "M9|継続的に" >> "$ENHANCED_DNA_DICT_FILE"
echo "M10|段階的に" >> "$ENHANCED_DNA_DICT_FILE"
echo "M11|上" >> "$ENHANCED_DNA_DICT_FILE"

# 数量（Quantity）を追加
echo "Q0|多数" >> "$ENHANCED_DNA_DICT_FILE"
echo "Q1|数百" >> "$ENHANCED_DNA_DICT_FILE"
echo "Q2|数千" >> "$ENHANCED_DNA_DICT_FILE"
echo "Q3|数万" >> "$ENHANCED_DNA_DICT_FILE"
echo "Q4|数十万" >> "$ENHANCED_DNA_DICT_FILE"
echo "Q5|数百万" >> "$ENHANCED_DNA_DICT_FILE"
echo "Q6|数千万" >> "$ENHANCED_DNA_DICT_FILE"
echo "Q7|数億" >> "$ENHANCED_DNA_DICT_FILE"
echo "Q8|数十億" >> "$ENHANCED_DNA_DICT_FILE"
echo "Q9|無数" >> "$ENHANCED_DNA_DICT_FILE"
echo "Q10|少数" >> "$ENHANCED_DNA_DICT_FILE"

# japanese_words.txtから追加のエントリを生成
echo "japanese_words.txtから追加エントリを生成しています..."

# 名詞を主語（Entity）として追加
echo "名詞を主語として追加しています..."
NOUN_COUNT=0
ENTITY_START=12  # 既存のエントリ数

# 最初の500個の単語を主語として追加
head -n 500 "$JAPANESE_WORDS_FILE" | while read -r word; do
    # 既存のエントリと重複しないか確認
    if ! grep -q "|$word$" "$ENHANCED_DNA_DICT_FILE"; then
        echo "E$((ENTITY_START + NOUN_COUNT))|$word" >> "$ENHANCED_DNA_DICT_FILE"
        NOUN_COUNT=$((NOUN_COUNT + 1))
    fi
done

# 名詞を目的語（Result）として追加
echo "名詞を目的語として追加しています..."
RESULT_COUNT=0
RESULT_START=13  # 既存のエントリ数

# 501-1000番目の単語を目的語として追加
sed -n '501,1000p' "$JAPANESE_WORDS_FILE" | while read -r word; do
    # 既存のエントリと重複しないか確認
    if ! grep -q "|$word$" "$ENHANCED_DNA_DICT_FILE"; then
        echo "R$((RESULT_START + RESULT_COUNT))|$word" >> "$ENHANCED_DNA_DICT_FILE"
        RESULT_COUNT=$((RESULT_COUNT + 1))
    fi
done

# 動詞（Concept）を追加
echo "動詞を追加しています..."
VERB_COUNT=0
VERB_START=13  # 既存のエントリ数

# 動詞リストを作成
cat > "$TEMP_DIR/verbs.txt" << EOF
読む
書く
話す
聞く
見る
食べる
飲む
走る
歩く
泳ぐ
飛ぶ
寝る
起きる
働く
休む
遊ぶ
学ぶ
教える
考える
思う
感じる
笑う
泣く
怒る
喜ぶ
悲しむ
恐れる
驚く
愛する
憎む
欲する
望む
期待する
信じる
疑う
知る
忘れる
覚える
理解する
誤解する
説明する
質問する
回答する
調査する
研究する
開発する
設計する
構築する
テストする
デバッグする
リファクタリングする
最適化する
拡張する
縮小する
統合する
分離する
抽象化する
具体化する
一般化する
特殊化する
初期化する
終了する
開始する
停止する
再開する
保存する
読み込む
書き出す
送信する
受信する
接続する
切断する
登録する
削除する
追加する
更新する
検索する
並べ替える
フィルタリングする
グループ化する
集計する
計算する
測定する
評価する
比較する
選択する
決定する
実行する
中止する
再試行する
スキップする
EOF

# 動詞リストから動詞を追加
cat "$TEMP_DIR/verbs.txt" | while read -r verb; do
    # 既存のエントリと重複しないか確認
    if ! grep -q "|$verb$" "$ENHANCED_DNA_DICT_FILE"; then
        echo "C$((VERB_START + VERB_COUNT))|$verb" >> "$ENHANCED_DNA_DICT_FILE"
        VERB_COUNT=$((VERB_COUNT + 1))
    fi
done

# 形容詞（Attribute）を追加
echo "形容詞を追加しています..."
ATTR_COUNT=0
ATTR_START=11  # 既存のエントリ数

# 形容詞リストを作成
cat > "$TEMP_DIR/adjectives.txt" << EOF
美しい
醜い
大きい
小さい
高い
低い
長い
短い
広い
狭い
厚い
薄い
重い
軽い
強い
弱い
硬い
柔らかい
熱い
冷たい
暖かい
涼しい
明るい
暗い
新しい
古い
若い
老いた
速い
遅い
早い
遅い
良い
悪い
正しい
間違った
真実の
偽りの
本物の
偽物の
簡単な
難しい
複雑な
単純な
便利な
不便な
快適な
不快な
安全な
危険な
健康的な
不健康な
幸せな
悲しい
楽しい
つまらない
面白い
退屈な
静かな
騒がしい
きれいな
汚い
整然とした
乱雑な
豊かな
貧しい
豊富な
乏しい
十分な
不十分な
完全な
不完全な
正確な
不正確な
明確な
不明確な
詳細な
大まかな
具体的な
抽象的な
論理的な
非論理的な
合理的な
非合理的な
効率的な
非効率的な
生産的な
非生産的な
創造的な
破壊的な
積極的な
消極的な
肯定的な
否定的な
楽観的な
悲観的な
現実的な
非現実的な
客観的な
主観的な
公平な
不公平な
公正な
不公正な
誠実な
不誠実な
正直な
不正直な
親切な
不親切な
思いやりのある
冷淡な
寛大な
けちな
勇敢な
臆病な
謙虚な
傲慢な
忍耐強い
短気な
慎重な
軽率な
賢い
愚かな
知的な
無知な
教養のある
無教養な
経験豊かな
未経験の
熟練した
未熟な
専門的な
素人の
有能な
無能な
有用な
無用な
必要な
不要な
重要な
些細な
緊急な
非緊急な
永続的な
一時的な
安定した
不安定な
一貫した
矛盾した
互換性のある
互換性のない
柔軟な
硬直した
拡張可能な
制限された
スケーラブルな
スケーラブルでない
移植可能な
移植不可能な
再利用可能な
使い捨ての
持続可能な
持続不可能な
環境に優しい
環境に有害な
エネルギー効率の良い
エネルギー効率の悪い
コスト効率の良い
コスト効率の悪い
時間効率の良い
時間効率の悪い
EOF

# 形容詞リストから形容詞を追加
cat "$TEMP_DIR/adjectives.txt" | while read -r adj; do
    # 既存のエントリと重複しないか確認
    if ! grep -q "|$adj$" "$ENHANCED_DNA_DICT_FILE"; then
        echo "A$((ATTR_START + ATTR_COUNT))|$adj" >> "$ENHANCED_DNA_DICT_FILE"
        ATTR_COUNT=$((ATTR_COUNT + 1))
    fi
done

# 副詞（Manner）を追加
echo "副詞を追加しています..."
MANNER_COUNT=0
MANNER_START=12  # 既存のエントリ数

# 副詞リストを作成
cat > "$TEMP_DIR/adverbs.txt" << EOF
非常に
とても
かなり
少し
ほとんど
全く
完全に
部分的に
徐々に
突然
常に
しばしば
時々
めったに
決して
絶対に
確かに
おそらく
もしかすると
実際に
本当に
明らかに
意外にも
当然
必然的に
偶然に
自然に
人為的に
意図的に
無意識に
自動的に
手動で
直接
間接的に
個別に
一緒に
同時に
順番に
前もって
事後に
早く
遅く
速く
ゆっくりと
静かに
大声で
明確に
曖昧に
詳細に
簡潔に
正確に
おおよそ
完璧に
不完全に
効率的に
非効率的に
生産的に
無駄に
創造的に
型通りに
積極的に
消極的に
肯定的に
否定的に
楽観的に
悲観的に
現実的に
理想的に
客観的に
主観的に
公平に
不公平に
誠実に
不誠実に
正直に
不正直に
親切に
不親切に
思いやりを持って
冷淡に
寛大に
けちに
勇敢に
臆病に
謙虚に
傲慢に
忍耐強く
短気に
慎重に
軽率に
賢く
愚かに
知的に
無知に
専門的に
素人っぽく
有能に
無能に
有用に
無用に
必要に
不要に
重要に
些細に
緊急に
非緊急に
永続的に
一時的に
安定して
不安定に
一貫して
矛盾して
互換性を持って
互換性なく
柔軟に
硬直して
拡張可能に
制限されて
スケーラブルに
スケーラブルでなく
移植可能に
移植不可能に
再利用可能に
使い捨てに
持続可能に
持続不可能に
環境に優しく
環境に有害に
エネルギー効率良く
エネルギー効率悪く
コスト効率良く
コスト効率悪く
時間効率良く
時間効率悪く
EOF

# 副詞リストから副詞を追加
cat "$TEMP_DIR/adverbs.txt" | while read -r adv; do
    # 既存のエントリと重複しないか確認
    if ! grep -q "|$adv$" "$ENHANCED_DNA_DICT_FILE"; then
        echo "M$((MANNER_START + MANNER_COUNT))|$adv" >> "$ENHANCED_DNA_DICT_FILE"
        MANNER_COUNT=$((MANNER_COUNT + 1))
    fi
done

# 時間表現（Time）を追加
echo "時間表現を追加しています..."
TIME_COUNT=0
TIME_START=11  # 既存のエントリ数

# 時間表現リストを作成
cat > "$TEMP_DIR/time_expressions.txt" << EOF
昨日
明日
今週
先週
来週
今月
先月
来月
今年
去年
来年
朝
昼
夕方
夜
深夜
未明
早朝
正午
真夜中
春
夏
秋
冬
平日
週末
休日
祝日
年始
年末
四半期
半期
年度
世紀
millennium
瞬間
一瞬
しばらく
永遠に
一時的に
永続的に
周期的に
不定期に
頻繁に
稀に
常に
決して
いつか
いつも
今すぐ
後で
以前
以後
同時に
交互に
連続して
断続的に
定期的に
不規則に
突然
徐々に
即座に
遅延して
予定通りに
予想外に
早期に
後期に
初期に
最終的に
中間で
開始時に
終了時に
実行中に
待機中に
停止中に
再開後に
中断前に
更新時に
読み込み時に
書き込み時に
処理中に
計算後に
評価前に
比較時に
選択後に
決定前に
実行中に
中止後に
再試行前に
スキップ時に
EOF

# 時間表現リストから時間表現を追加
cat "$TEMP_DIR/time_expressions.txt" | while read -r time_expr; do
    # 既存のエントリと重複しないか確認
    if ! grep -q "|$time_expr$" "$ENHANCED_DNA_DICT_FILE"; then
        echo "T$((TIME_START + TIME_COUNT))|$time_expr" >> "$ENHANCED_DNA_DICT_FILE"
        TIME_COUNT=$((TIME_COUNT + 1))
    fi
done

# 場所表現（Location）を追加
echo "場所表現を追加しています..."
LOCATION_COUNT=0
LOCATION_START=11  # 既存のエントリ数

# 場所表現リストを作成
cat > "$TEMP_DIR/location_expressions.txt" << EOF
ここ
そこ
あそこ
どこか
どこでも
内部
外部
上部
下部
前方
後方
左側
右側
中央
周辺
境界
端
隅
表面
裏面
内側
外側
近く
遠く
国内
海外
オンライン
オフライン
仮想空間
現実世界
フロントエンド
バックエンド
クライアント側
サーバー側
ローカル
リモート
グローバル
ローカルスコープ
グローバルスコープ
プライベート領域
パブリック領域
保護領域
共有メモリ
専用メモリ
キャッシュ
バッファ
スタック
ヒープ
レジスタ
ディスク
SSD
HDD
フラッシュメモリ
RAM
ROM
クラウドストレージ
ローカルストレージ
データベース
ファイルシステム
ネットワーク
インターネット
イントラネット
VPN
コンテナ
仮想マシン
物理マシン
クラスタ
ノード
エッジ
センター
分散環境
集中環境
開発環境
テスト環境
ステージング環境
本番環境
障害復旧環境
バックアップサイト
プライマリサイト
セカンダリサイト
データセンター
オフィス
自宅
モバイル
デスクトップ
ラップトップ
サーバーラック
クラウドリージョン
アベイラビリティゾーン
CDN
エッジロケーション
EOF

# 場所表現リストから場所表現を追加
cat "$TEMP_DIR/location_expressions.txt" | while read -r loc_expr; do
    # 既存のエントリと重複しないか確認
    if ! grep -q "|$loc_expr$" "$ENHANCED_DNA_DICT_FILE"; then
        echo "L$((LOCATION_START + LOCATION_COUNT))|$loc_expr" >> "$ENHANCED_DNA_DICT_FILE"
        LOCATION_COUNT=$((LOCATION_COUNT + 1))
    fi
done

# 数量表現（Quantity）を追加
echo "数量表現を追加しています..."
QUANTITY_COUNT=0
QUANTITY_START=11  # 既存のエントリ数

# 数量表現リストを作成
cat > "$TEMP_DIR/quantity_expressions.txt" << EOF
ゼロ
一つ
二つ
三つ
四つ
五つ
十
百
千
万
億
兆
京
全て
なし
いくつか
多く
少し
ほとんど
わずか
十分
不十分
過剰
不足
一部
全部
半分
四分の一
三分の一
八分の一
パーセント
割合
比率
倍
半分
数倍
数十倍
数百倍
数千倍
バイト
キロバイト
メガバイト
ギガバイト
テラバイト
ペタバイト
ビット
キロビット
メガビット
ギガビット
テラビット
ヘルツ
キロヘルツ
メガヘルツ
ギガヘルツ
テラヘルツ
ワット
キロワット
メガワット
ギガワット
テラワット
アンペア
ボルト
オーム
ジュール
カロリー
グラム
キログラム
トン
ミリグラム
マイクログラム
リットル
ミリリットル
立方メートル
平方メートル
メートル
キロメートル
センチメートル
ミリメートル
マイクロメートル
ナノメートル
インチ
フィート
ヤード
マイル
秒
分
時間
日
週間
ヶ月
年
世紀
ミリ秒
マイクロ秒
ナノ秒
ピコ秒
フェムト秒
EOF

# 数量表現リストから数量表現を追加
cat "$TEMP_DIR/quantity_expressions.txt" | while read -r quant_expr; do
    # 既存のエントリと重複しないか確認
    if ! grep -q "|$quant_expr$" "$ENHANCED_DNA_DICT_FILE"; then
        echo "Q$((QUANTITY_START + QUANTITY_COUNT))|$quant_expr" >> "$ENHANCED_DNA_DICT_FILE"
        QUANTITY_COUNT=$((QUANTITY_COUNT + 1))
    fi
done

# 辞書のエントリ数を表示
TOTAL_ENTRIES=$(wc -l < "$ENHANCED_DNA_DICT_FILE")
echo "拡張DNA辞書の生成が完了しました。"
echo "合計エントリ数: $TOTAL_ENTRIES"
echo "- 主語 (Entity): $((ENTITY_START + NOUN_COUNT)) エントリ"
echo "- 動詞 (Concept): $((VERB_START + VERB_COUNT)) エントリ"
echo "- 目的語 (Result): $((RESULT_START + RESULT_COUNT)) エントリ"
echo "- 属性 (Attribute): $((ATTR_START + ATTR_COUNT)) エントリ"
echo "- 時間 (Time): $((TIME_START + TIME_COUNT)) エントリ"
echo "- 場所 (Location): $((LOCATION_START + LOCATION_COUNT)) エントリ"
echo "- 様態 (Manner): $((MANNER_START + MANNER_COUNT)) エントリ"
echo "- 数量 (Quantity): $((QUANTITY_START + QUANTITY_COUNT)) エントリ"
echo "辞書ファイル: $ENHANCED_DNA_DICT_FILE"