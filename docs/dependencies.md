# 依存関係インストールガイド

GeneLLMを実行するには、以下の依存関係をインストールする必要があります。

## 必須依存関係

### 1. MeCab（形態素解析エンジン）

MeCabは日本語テキストを単語に分割するための形態素解析エンジンです。

#### Ubuntu/Debian系

```bash
sudo apt-get update
sudo apt-get install -y libmecab-dev mecab mecab-ipadic-utf8
```

#### Red Hat/CentOS/Fedora系

```bash
sudo yum install -y mecab mecab-devel mecab-ipadic
```

#### macOS

```bash
brew install mecab mecab-ipadic
```

#### Windows

Windows版MeCabは[こちら](https://taku910.github.io/mecab/#download)からダウンロードできます。インストール後、環境変数の設定が必要です。

### 2. libcurl（HTTPクライアントライブラリ）

libcurlはウェブからのデータ取得に使用されます。

#### Ubuntu/Debian系

```bash
sudo apt-get install -y libcurl4-openssl-dev
```

#### Red Hat/CentOS/Fedora系

```bash
sudo yum install -y libcurl-devel
```

#### macOS

```bash
brew install curl
```

#### Windows

Windows版libcurlは[こちら](https://curl.se/windows/)からダウンロードできます。

## オプション依存関係

### 1. SQLite（データベース）

より高度な学習機能を使用する場合に必要です。

#### Ubuntu/Debian系

```bash
sudo apt-get install -y libsqlite3-dev
```

#### Red Hat/CentOS/Fedora系

```bash
sudo yum install -y sqlite-devel
```

#### macOS

```bash
brew install sqlite
```

## トラブルシューティング

### MeCabのインストール問題

#### 辞書が見つからない場合

```bash
# Ubuntu/Debian系
sudo apt-get install -y mecab-ipadic-utf8

# または手動でダウンロードして設定
wget https://drive.google.com/uc?export=download&id=0B4y35FiV1wh7MWVlSDBCSXZMTXM -O mecab-ipadic-2.7.0-20070801.tar.gz
tar -xzf mecab-ipadic-2.7.0-20070801.tar.gz
cd mecab-ipadic-2.7.0-20070801
./configure --with-charset=utf8
make
sudo make install
```

#### ライブラリパスの問題

```bash
# ライブラリパスを確認
sudo ldconfig -v | grep mecab

# 必要に応じてパスを追加
echo '/usr/local/lib' | sudo tee -a /etc/ld.so.conf.d/local.conf
sudo ldconfig
```

### libcurlのインストール問題

#### SSL関連のエラー

```bash
# OpenSSLの依存関係をインストール
sudo apt-get install -y libssl-dev

# または
sudo yum install -y openssl-devel
```

## 依存関係の確認

インストールが正常に完了したかを確認するには、以下のコマンドを実行します：

```bash
# MeCabの確認
mecab --version
echo "こんにちは世界" | mecab

# libcurlの確認
curl --version
```

正常にインストールされていれば、これらのコマンドはエラーなく実行され、適切な出力が表示されます。