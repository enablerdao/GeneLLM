# GeneLLM 使用方法ガイド

このガイドでは、GeneLLMの詳細な使用方法と高度なオプションについて説明します。

## 基本的な使用方法

GeneLLMは以下の3つの主要なモードで実行できます：

### 1. 単一質問モード

単一の質問に対して回答を得るためのモードです。

```bash
./gllm "質問文をここに入力"
```

例：
```bash
./gllm "C言語でHello Worldを表示するプログラムを教えてください"
```

### 2. デバッグモード

内部処理の詳細を表示するデバッグモードです。

```bash
./gllm -d "質問文をここに入力"
```

または環境変数を使用：

```bash
DEBUG=1 ./gllm "質問文をここに入力"
```

デバッグモードでは以下の情報が表示されます：
- 形態素解析結果
- 使用された知識ベース
- ベクトル検索結果
- エージェント選択プロセス
- 応答生成ステップ

### 3. 対話モード

継続的な会話を行うための対話モードです。

```bash
./gllm -i
```

対話モードでは、プロンプトが表示され、質問を入力できます。`exit`と入力することで終了します。

## コマンドラインオプション

GeneLLMは以下のコマンドラインオプションをサポートしています：

```
使用法: ./gllm [オプション] [質問]

オプション:
  -h, --help        このヘルプメッセージを表示
  -v, --version     バージョン情報を表示
  -d, --debug       デバッグモードで実行
  -i, --interactive 対話モードで実行
  -f, --file FILE   指定したファイルから質問を読み込む
  --no-color        カラー出力を無効化
  --log FILE        ログファイルを指定
```

## 高度な使用方法

### ファイルからの質問読み込み

テキストファイルから質問を読み込むことができます：

```bash
./gllm -f questions.txt
```

ファイル内の各行が個別の質問として処理されます。

### 知識ベースの拡張

新しい知識を追加するには、`knowledge/base`ディレクトリに適切なフォーマットのマークダウンファイルを作成します：

```markdown
---
category: プログラミング
tags: C言語, 入門, サンプルコード
created_at: 2023-03-23 12:00:00
updated_at: 2023-03-23 12:00:00
---

# C言語入門

C言語は汎用プログラミング言語の一つで、...
```

### 環境変数

GeneLLMは以下の環境変数をサポートしています：

- `DEBUG`: デバッグモードを有効にします（値: 1 または true）
- `GLLM_KNOWLEDGE_PATH`: 知識ベースのパスを指定します
- `GLLM_LOG_PATH`: ログファイルのパスを指定します
- `GLLM_NO_COLOR`: カラー出力を無効にします（値: 1 または true）

例：
```bash
GLLM_KNOWLEDGE_PATH=/path/to/custom/knowledge ./gllm "質問"
```

## 応用例

### シェルスクリプトとの連携

GeneLLMをシェルスクリプトで活用する例：

```bash
#!/bin/bash
# 質問リストを処理するスクリプト

while IFS= read -r question; do
  echo "質問: $question"
  answer=$(./gllm "$question")
  echo "回答: $answer"
  echo "-------------------"
done < questions.txt
```

### パイプラインでの使用

標準入力からの入力を処理することもできます：

```bash
echo "C言語とは何ですか？" | ./gllm
```

### Webサービスとしての実行

簡易的なWebサービスとして実行する例（要Python）：

```bash
# サーバースクリプトを作成
cat > gllm_server.py << 'EOF'
from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse
import subprocess
import json

class GllmHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path.startswith('/ask'):
            query = urllib.parse.parse_qs(urllib.parse.urlparse(self.path).query)
            if 'q' in query:
                question = query['q'][0]
                result = subprocess.run(['./gllm', question], capture_output=True, text=True)
                answer = result.stdout.strip()
                
                self.send_response(200)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({'question': question, 'answer': answer}).encode())
                return
        
        self.send_response(400)
        self.send_header('Content-type', 'text/plain')
        self.end_headers()
        self.wfile.write(b'Bad Request')

if __name__ == '__main__':
    server = HTTPServer(('localhost', 8000), GllmHandler)
    print('GeneLLM server running at http://localhost:8000/')
    print('Example: http://localhost:8000/ask?q=C%E8%A8%80%E8%AA%9E%E3%81%A8%E3%81%AF%E4%BD%95%E3%81%A7%E3%81%99%E3%81%8B%EF%BC%9F')
    server.serve_forever()
EOF

# サーバーを実行
python3 gllm_server.py
```

## トラブルシューティング

### 一般的な問題

1. **MeCabエラー**: `error while loading shared libraries: libmecab.so.2`
   ```bash
   sudo ldconfig
   # または
   export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
   ```

2. **メモリ不足エラー**: 大きなテキストファイルを処理する場合
   ```bash
   # メモリ使用量を制限
   ulimit -v 1000000  # 1GBに制限
   ```

3. **応答が生成されない**: 知識ベースが見つからない場合
   ```bash
   # 知識ベースディレクトリの確認
   ls -la knowledge/base/
   # 必要に応じてサンプル知識ファイルをコピー
   cp -r samples/knowledge/* knowledge/base/
   ```

### パフォーマンスの最適化

1. **応答速度の向上**:
   ```bash
   # ベクトルデータベースを事前に構築
   ./gllm --build-vectors
   ```

2. **メモリ使用量の削減**:
   ```bash
   # 軽量モードで実行
   ./gllm --lightweight "質問"
   ```

## その他の情報

詳細な情報は以下のドキュメントを参照してください：

- [アーキテクチャ概要](architecture_overview.md)
- [知識ベース拡張ガイド](knowledge_base_guide.md)
- [ベンチマークレポート](../benchmark/README.md)
- [開発者ガイド](developer_guide.md)