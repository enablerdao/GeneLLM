# 🧠 C言語による超軽量AI実装プロジェクト

<div align="center">
  <img src="https://img.shields.io/badge/language-C-blue.svg" alt="Language: C">
  <img src="https://img.shields.io/badge/platform-Linux%20%7C%20macOS-lightgrey.svg" alt="Platform: Linux | macOS">
  <img src="https://img.shields.io/badge/license-MIT-green.svg" alt="License: MIT">
</div>

<p align="center">
  <i>軽量・高速・シンプルなAI実装</i>
</p>

---

## 📋 概要

このプロジェクトは、C言語を主体として軽量なAI機能を実装することを目的としています。大規模な深層学習モデルとは異なり、形態素解析によるトークナイズと独自のルールベースアプローチを組み合わせた、リソース効率の良いAIシステムを構築しています。

### 🔍 特徴

- **軽量設計**: 最小限のリソースで動作
- **高速処理**: C言語による効率的な実装
- **透明性**: 処理過程が明示的で解釈可能
- **モジュール性**: 機能ごとに分離された設計
- **クロスプラットフォーム**: Linux/macOSで動作

### 🆚 従来のAIとの違い

現代の大規模言語モデル（LLM）とは異なるアプローチでAI機能を実装しています。OpenAIのGPTシリーズなどが大量のパラメータと深層学習に依存しているのに対し、このプロジェクトでは形態素解析による言語理解と、シンボリックなルールベースの推論を組み合わせています。

## 🚀 クイックスタート

### 前提条件

- C コンパイラ (gcc/clang)
- MeCab (形態素解析エンジン)

### Macでのセットアップ

```bash
# Homebrewがインストールされていない場合はインストール
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# MeCabとその辞書をインストール
brew install mecab mecab-ipadic

# リポジトリをクローン
git clone https://github.com/yourusername/c-lightweight-ai.git
cd c-lightweight-ai

# ビルド
./build.sh
```

### Linuxでのセットアップ

```bash
# Ubuntuの場合
sudo apt-get update
sudo apt-get install -y gcc make mecab libmecab-dev mecab-ipadic-utf8

# リポジトリをクローン
git clone https://github.com/yourusername/c-lightweight-ai.git
cd c-lightweight-ai

# ビルド
./build.sh
```

### 🧪 動作確認

```bash
# ルーターモデルを試す
./main router "量子コンピュータの仕組みを教えてください"

# 対話モードを試す
./main router -i
```

## 💡 実装機能

このプロジェクトでは、以下の機能を実装しています：

### 1️⃣ 意味概念抽出（簡易構文解析器）

<details>
<summary>詳細を表示</summary>

形態素解析を用いて日本語または英語の文から主語、動詞、結果を抽出します。

```bash
./main syntax "猫が魚を食べて嬉しかった。"
```

出力例:
```
入力文: 猫が魚を食べて嬉しかった。
抽出結果: [主語: 猫][動詞: 食べる][結果: 嬉しかった]
```
</details>

### 2️⃣ DNAベースの概念圧縮（シンボル化）

<details>
<summary>詳細を表示</summary>

主語、動詞、結果をDNA風の記号に圧縮します。

#### 圧縮の仕組み

1. **意味単位の分類**: 文の意味要素を主語(Entity/E)、動詞(Concept/C)、結果(Result/R)の3つの基本カテゴリに分類
2. **シンボル化**: 各カテゴリに1文字のコード（E, C, R）を割り当て、その後に固有の識別番号を付与
   - 例: 「猫」→ E00、「食べる」→ C01、「嬉しい」→ R02
3. **辞書管理**: 各シンボルと対応する自然言語表現を辞書に保存
4. **圧縮表現**: 「猫が魚を食べて嬉しかった」→「E00C01R02」

```bash
./main dna "猫" "食べる" "嬉しい"
```

出力例:
```
入力:
主語 = 猫
動詞 = 食べる
結果 = 嬉しい

DNA風圧縮: E00C01R02

辞書内容:
E00: 猫
C01: 食べる
R02: 嬉しい

再構築された文: 猫は食べ嬉しいになった。
```

#### 利点

- **データ圧縮**: 長い文や複雑な概念を少ないバイト数で表現
- **パターン認識**: 類似した概念パターンを容易に識別
- **効率的な処理**: 圧縮された形式での計算処理が高速
- **言語間変換**: 異なる言語間での概念の対応付けが容易
</details>

### 3️⃣ ベクトル検索

<details>
<summary>詳細を表示</summary>

多次元のfloatベクトルを高速に検索します。この機能は外部ライブラリに依存せず、純粋なC言語で実装されています。

```bash
./main vector
```

出力例:
```
ベクトルデータベースの作成中...
  0 / 1000 ベクトルを追加しました
  100 / 1000 ベクトルを追加しました
  ...
1000 ベクトルをデータベースに追加しました

クエリベクトル: [0.0700, 0.1370, -0.0112, -0.1491, 0.1183, ...]

検索結果（ユークリッド距離）:
  最も近いベクトルのID: 910
  検索時間: 0.42 ミリ秒

検索結果（コサイン類似度）:
  最も近いベクトルのID: 910
  検索時間: 0.44 ミリ秒
```
</details>

### 4️⃣ グラフ生成

<details>
<summary>詳細を表示</summary>

意味単位のノード（主語・動詞・結果）を非連続的に探索して組み合わせ、自然な文を生成します。

```bash
./main graph
```

出力例:
```
グラフ構造:
ノード一覧:
  ID: 0, タイプ: テンプレート, 値: {主語}は{動詞}て{結果}になりました。
  ID: 1, タイプ: テンプレート, 値: {主語}が{動詞}たら{結果}でした。
  ...

生成された文:
1: 猫は眠りて満足したになりました。
2: 犬が食べたら嬉しいでした。
3: 人間は食べて嬉しいになりました。
```
</details>

### 5️⃣ ルーターモデル

<details>
<summary>詳細を表示</summary>

入力された文に対して、知識ベースから適切な回答を生成します。この機能は形態素解析ライブラリ（MeCab）を使用して入力文を解析し、適切な応答を生成します。

```bash
./main router "量子コンピュータの仕組みを教えてください"
```

出力例:
```
量子コンピュータは量子力学の原理を利用し、従来のビットではなく量子ビット（キュービット）を使用します。キュービットは重ね合わせ状態で0と1を同時に表現できます。
```

対話モードも利用できます：

```bash
./main router -i
```

対話モードでは、プロンプトが表示され、質問を入力することができます。終了するには「exit」と入力します。

```
対話モードを開始します。終了するには 'exit' と入力してください。
> 料理の基本を教えてください
料理の基本は、食材の特性を理解し、適切な調理法と調味料を選ぶことです。包丁の扱いや火加減の調整も重要なスキルです。
> exit
```
</details>

## 📚 知識ベース

ルーターモデルは以下のトピックに関する知識を持っています：

<div style="columns: 2;">
  <ul>
    <li>幸せ（happiness）</li>
    <li>愛（love）</li>
    <li>健康（health）</li>
    <li>仕事（work）</li>
    <li>学習（learning）</li>
    <li>数学（math）</li>
    <li>物理学（physics）</li>
    <li>プログラミング（programming）</li>
    <li>ギター（guitar）</li>
    <li>サバイバル（survival）</li>
    <li>量子コンピュータ（quantum）</li>
    <li>料理（cooking）</li>
    <li>フィットネス（fitness）</li>
    <li>旅行（travel）</li>
    <li>金融（finance）</li>
    <li>エンターテイメント（entertainment）</li>
  </ul>
</div>

## 💻 プログラミング例

以下は、このプロジェクトを使って作成できるプログラミング例です。

### C言語で簡単な電卓プログラム

```c
#include <stdio.h>

int main() {
    char operator;
    double num1, num2, result;
    
    printf("簡易電卓プログラム\n");
    printf("使用可能な演算子: +, -, *, /\n");
    
    printf("計算式を入力してください (例: 5 + 3): ");
    scanf("%lf %c %lf", &num1, &operator, &num2);
    
    switch(operator) {
        case '+':
            result = num1 + num2;
            break;
        case '-':
            result = num1 - num2;
            break;
        case '*':
            result = num1 * num2;
            break;
        case '/':
            if(num2 != 0)
                result = num1 / num2;
            else {
                printf("エラー: ゼロで除算できません\n");
                return 1;
            }
            break;
        default:
            printf("エラー: 無効な演算子です\n");
            return 1;
    }
    
    printf("%.2lf %c %.2lf = %.2lf\n", num1, operator, num2, result);
    return 0;
}
```

### C言語でシンプルなTodoリスト

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 100
#define MAX_LENGTH 100

// タスク構造体
typedef struct {
    char description[MAX_LENGTH];
    int completed;
} Task;

// グローバル変数
Task tasks[MAX_TASKS];
int taskCount = 0;

// 関数プロトタイプ
void addTask(const char* description);
void viewTasks();
void markTaskCompleted(int taskIndex);
void deleteTask(int taskIndex);
void saveTasksToFile(const char* filename);
void loadTasksFromFile(const char* filename);

int main() {
    int choice, taskIndex;
    char description[MAX_LENGTH];
    char filename[] = "tasks.txt";
    
    // ファイルからタスクを読み込む
    loadTasksFromFile(filename);
    
    while(1) {
        printf("\n===== Todoリスト =====\n");
        printf("1. タスクを追加\n");
        printf("2. タスク一覧を表示\n");
        printf("3. タスクを完了としてマーク\n");
        printf("4. タスクを削除\n");
        printf("5. 終了\n");
        printf("選択してください: ");
        scanf("%d", &choice);
        getchar(); // 改行文字を消費
        
        switch(choice) {
            case 1:
                printf("新しいタスクを入力: ");
                fgets(description, MAX_LENGTH, stdin);
                description[strcspn(description, "\n")] = 0; // 改行を削除
                addTask(description);
                break;
            case 2:
                viewTasks();
                break;
            case 3:
                printf("完了としてマークするタスク番号: ");
                scanf("%d", &taskIndex);
                markTaskCompleted(taskIndex - 1); // 0-indexedに変換
                break;
            case 4:
                printf("削除するタスク番号: ");
                scanf("%d", &taskIndex);
                deleteTask(taskIndex - 1); // 0-indexedに変換
                break;
            case 5:
                saveTasksToFile(filename);
                printf("タスクを保存して終了します。\n");
                return 0;
            default:
                printf("無効な選択です。もう一度お試しください。\n");
        }
        
        // タスクをファイルに自動保存
        saveTasksToFile(filename);
    }
    
    return 0;
}

// 新しいタスクを追加
void addTask(const char* description) {
    if(taskCount < MAX_TASKS) {
        strcpy(tasks[taskCount].description, description);
        tasks[taskCount].completed = 0;
        taskCount++;
        printf("タスクが追加されました。\n");
    } else {
        printf("タスクリストがいっぱいです。\n");
    }
}

// タスク一覧を表示
void viewTasks() {
    if(taskCount == 0) {
        printf("タスクはありません。\n");
        return;
    }
    
    printf("\n現在のタスク:\n");
    for(int i = 0; i < taskCount; i++) {
        printf("%d. [%c] %s\n", 
               i + 1, 
               tasks[i].completed ? 'X' : ' ', 
               tasks[i].description);
    }
}

// タスクを完了としてマーク
void markTaskCompleted(int taskIndex) {
    if(taskIndex >= 0 && taskIndex < taskCount) {
        tasks[taskIndex].completed = 1;
        printf("タスクを完了としてマークしました。\n");
    } else {
        printf("無効なタスク番号です。\n");
    }
}

// タスクを削除
void deleteTask(int taskIndex) {
    if(taskIndex >= 0 && taskIndex < taskCount) {
        for(int i = taskIndex; i < taskCount - 1; i++) {
            strcpy(tasks[i].description, tasks[i+1].description);
            tasks[i].completed = tasks[i+1].completed;
        }
        taskCount--;
        printf("タスクを削除しました。\n");
    } else {
        printf("無効なタスク番号です。\n");
    }
}

// タスクをファイルに保存
void saveTasksToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if(file == NULL) {
        printf("ファイルを開けませんでした。\n");
        return;
    }
    
    for(int i = 0; i < taskCount; i++) {
        fprintf(file, "%d,%s\n", tasks[i].completed, tasks[i].description);
    }
    
    fclose(file);
}

// ファイルからタスクを読み込む
void loadTasksFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if(file == NULL) {
        // ファイルが存在しない場合は何もしない
        return;
    }
    
    char line[MAX_LENGTH];
    char description[MAX_LENGTH];
    int completed;
    
    taskCount = 0;
    
    while(fgets(line, MAX_LENGTH, file) && taskCount < MAX_TASKS) {
        sscanf(line, "%d,%[^\n]", &completed, description);
        strcpy(tasks[taskCount].description, description);
        tasks[taskCount].completed = completed;
        taskCount++;
    }
    
    fclose(file);
    printf("%d個のタスクを読み込みました。\n", taskCount);
}
```

### C言語で簡単なテキストエディタ

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 100

char lines[MAX_LINES][MAX_LINE_LENGTH];
int lineCount = 0;

// 関数プロトタイプ
void displayMenu();
void createNewFile();
void openFile();
void saveFile();
void editLine();
void insertLine();
void deleteLine();
void displayContent();

int main() {
    int choice;
    
    while(1) {
        displayMenu();
        printf("選択してください: ");
        scanf("%d", &choice);
        getchar(); // 改行文字を消費
        
        switch(choice) {
            case 1:
                createNewFile();
                break;
            case 2:
                openFile();
                break;
            case 3:
                saveFile();
                break;
            case 4:
                editLine();
                break;
            case 5:
                insertLine();
                break;
            case 6:
                deleteLine();
                break;
            case 7:
                displayContent();
                break;
            case 8:
                printf("エディタを終了します。\n");
                return 0;
            default:
                printf("無効な選択です。もう一度お試しください。\n");
        }
    }
    
    return 0;
}

// メニューを表示
void displayMenu() {
    printf("\n===== シンプルテキストエディタ =====\n");
    printf("1. 新規ファイル作成\n");
    printf("2. ファイルを開く\n");
    printf("3. ファイルを保存\n");
    printf("4. 行を編集\n");
    printf("5. 行を挿入\n");
    printf("6. 行を削除\n");
    printf("7. 内容を表示\n");
    printf("8. 終了\n");
}

// 新規ファイル作成
void createNewFile() {
    printf("新規ファイルを作成します。既存の内容は消去されます。\n");
    printf("続行しますか？ (y/n): ");
    char confirm;
    scanf("%c", &confirm);
    getchar(); // 改行文字を消費
    
    if(confirm == 'y' || confirm == 'Y') {
        lineCount = 0;
        printf("新規ファイルが作成されました。\n");
    }
}

// ファイルを開く
void openFile() {
    char filename[100];
    printf("開くファイル名を入力: ");
    fgets(filename, 100, stdin);
    filename[strcspn(filename, "\n")] = 0; // 改行を削除
    
    FILE* file = fopen(filename, "r");
    if(file == NULL) {
        printf("ファイルを開けませんでした。\n");
        return;
    }
    
    lineCount = 0;
    while(fgets(lines[lineCount], MAX_LINE_LENGTH, file) && lineCount < MAX_LINES) {
        // 改行文字を削除
        lines[lineCount][strcspn(lines[lineCount], "\n")] = 0;
        lineCount++;
    }
    
    fclose(file);
    printf("ファイル '%s' を読み込みました。%d行あります。\n", filename, lineCount);
}

// ファイルを保存
void saveFile() {
    char filename[100];
    printf("保存するファイル名を入力: ");
    fgets(filename, 100, stdin);
    filename[strcspn(filename, "\n")] = 0; // 改行を削除
    
    FILE* file = fopen(filename, "w");
    if(file == NULL) {
        printf("ファイルを保存できませんでした。\n");
        return;
    }
    
    for(int i = 0; i < lineCount; i++) {
        fprintf(file, "%s\n", lines[i]);
    }
    
    fclose(file);
    printf("ファイル '%s' に保存しました。\n", filename);
}

// 行を編集
void editLine() {
    int lineNum;
    
    displayContent();
    
    printf("編集する行番号を入力: ");
    scanf("%d", &lineNum);
    getchar(); // 改行文字を消費
    
    if(lineNum < 1 || lineNum > lineCount) {
        printf("無効な行番号です。\n");
        return;
    }
    
    printf("行 %d: %s\n", lineNum, lines[lineNum-1]);
    printf("新しい内容を入力: ");
    fgets(lines[lineNum-1], MAX_LINE_LENGTH, stdin);
    lines[lineNum-1][strcspn(lines[lineNum-1], "\n")] = 0; // 改行を削除
    
    printf("行を編集しました。\n");
}

// 行を挿入
void insertLine() {
    int lineNum;
    
    if(lineCount >= MAX_LINES) {
        printf("これ以上行を追加できません。\n");
        return;
    }
    
    displayContent();
    
    printf("挿入する位置を入力 (1-%d): ", lineCount+1);
    scanf("%d", &lineNum);
    getchar(); // 改行文字を消費
    
    if(lineNum < 1 || lineNum > lineCount+1) {
        printf("無効な位置です。\n");
        return;
    }
    
    // 後ろの行を1つずつ移動
    for(int i = lineCount; i >= lineNum; i--) {
        strcpy(lines[i], lines[i-1]);
    }
    
    printf("新しい行の内容を入力: ");
    fgets(lines[lineNum-1], MAX_LINE_LENGTH, stdin);
    lines[lineNum-1][strcspn(lines[lineNum-1], "\n")] = 0; // 改行を削除
    
    lineCount++;
    printf("行を挿入しました。\n");
}

// 行を削除
void deleteLine() {
    int lineNum;
    
    displayContent();
    
    printf("削除する行番号を入力: ");
    scanf("%d", &lineNum);
    getchar(); // 改行文字を消費
    
    if(lineNum < 1 || lineNum > lineCount) {
        printf("無効な行番号です。\n");
        return;
    }
    
    // 後ろの行を1つずつ前に移動
    for(int i = lineNum-1; i < lineCount-1; i++) {
        strcpy(lines[i], lines[i+1]);
    }
    
    lineCount--;
    printf("行を削除しました。\n");
}

// 内容を表示
void displayContent() {
    if(lineCount == 0) {
        printf("ファイルは空です。\n");
        return;
    }
    
    printf("\n===== ファイル内容 =====\n");
    for(int i = 0; i < lineCount; i++) {
        printf("%3d: %s\n", i+1, lines[i]);
    }
    printf("========================\n");
}

## ⚙️ 実装状況

| 機能 | 状態 | 説明 |
|------|------|------|
| 意味概念抽出 | ✅ | 形態素解析を用いた構文解析 |
| DNA概念圧縮 | ✅ | 主語・動詞・結果のシンボル化 |
| 概念から文への再構築 | ✅ | 圧縮された概念からの文生成 |
| ベクトル検索 | ✅ | 高速なベクトル近傍探索 |
| グラフ生成器 | ✅ | 非連続トークンからの文生成 |
| ルーターモデル | ✅ | 知識ベースからの応答生成 |
| P2P通信モジュール | 🔄 | UDPベースの通信（開発中） |
| JITコンパイル | 🔄 | LLVM利用の動的コンパイル（開発中） |

## 🛠️ ビルド方法

### ビルドスクリプトを使用

```bash
# すべてのプログラムを一度にビルド
./build.sh
```

### 個別にビルド

```bash
# 構文解析器
gcc -o simple_analyzer simple_analyzer.c -Wall -Wextra -std=c99

# DNA圧縮
gcc -o dna_compressor dna_compressor.c -Wall -Wextra -std=c99

# ベクトル検索
gcc -o vector_search vector_search.c -Wall -Wextra -std=c99 -lm

# グラフ生成器
gcc -o graph_generator graph_generator.c -Wall -Wextra -std=c99

# ルーターモデル
gcc -o c_implementation/improved_router_model c_implementation/improved_router_model.c -lmecab

# メインプログラム
gcc -o main main.c -Wall -Wextra -std=c99
```

## 🔮 今後の展望

このプロジェクトでは、今後以下の機能の実装を検討しています：

1. **分散P2P通信モジュール** - UDPベースの超簡易なP2P通信モジュール
   - ローカル環境（LAN内）でデータ（圧縮概念コードなど）を交換できる仕組み
   - 標準ソケットAPIのみを使用した実装

2. **JITコンパイル高速化** - LLVMを利用した動的コンパイルによる高速化
   - よく使われる概念コード検索や生成処理をネイティブコードとして動的にコンパイル・実行
   - LLVM C APIを利用した実装

## 📄 ライセンス

このプロジェクトはオープンソースで提供されています。詳細はLICENSEファイルを参照してください。

---

<p align="center">
  <i>Made with ❤️ by C言語AI実装チーム</i>
</p>