#!/bin/bash

# GeneLLM 統合コマンド
# 使用方法: ./genellm [コマンド] [オプション]

# 定数
VERSION="1.0.0"
WORKSPACE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
BIN_DIR="$WORKSPACE_DIR/bin"
SCRIPTS_DIR="$WORKSPACE_DIR/scripts"
WEB_DIR="$WORKSPACE_DIR/web/server"
KNOWLEDGE_DIR="$WORKSPACE_DIR/knowledge"
DOCS_DIR="$WORKSPACE_DIR/docs"
CONFIG_FILE="$WORKSPACE_DIR/.genellm_config"
LOG_DIR="$WORKSPACE_DIR/logs"
PID_FILE="$LOG_DIR/genellm.pid"

# 設定ファイルの読み込み
if [ -f "$CONFIG_FILE" ]; then
    source "$CONFIG_FILE"
fi

# デフォルト設定
PORT=${PORT:-50065}
DEBUG=${DEBUG:-false}
FALLBACK=${FALLBACK:-true}
AUTO_UPDATE=${AUTO_UPDATE:-true}
LOG_LEVEL=${LOG_LEVEL:-info}

# ディレクトリの作成
mkdir -p "$LOG_DIR"

# ログ関数
log() {
    local level="$1"
    local message="$2"
    local timestamp=$(date "+%Y-%m-%d %H:%M:%S")
    
    case "$level" in
        debug)
            [ "$LOG_LEVEL" = "debug" ] && echo "[$timestamp] [DEBUG] $message"
            ;;
        info)
            [ "$LOG_LEVEL" = "debug" -o "$LOG_LEVEL" = "info" ] && echo "[$timestamp] [INFO] $message"
            ;;
        warn)
            [ "$LOG_LEVEL" = "debug" -o "$LOG_LEVEL" = "info" -o "$LOG_LEVEL" = "warn" ] && echo "[$timestamp] [WARN] $message"
            ;;
        error)
            echo "[$timestamp] [ERROR] $message"
            ;;
    esac
}

# 使用方法を表示
show_usage() {
    cat << EOF
GeneLLM 統合コマンド v$VERSION

使用方法: ./genellm [コマンド] [オプション]

コマンド:
  ask <質問>                質問に対する回答を取得
  chat                      対話モードを開始
  server                    APIサーバーを起動
  web                       Webインターフェースを起動
  update                    知識ベースを更新
  compile                   ソースコードをコンパイル
  status                    サーバーの状態を確認
  stop                      実行中のサーバーを停止
  config                    設定を表示/変更
  vectors                   単語ベクトルデータを表示
  help                      このヘルプを表示
  version                   バージョン情報を表示

オプション:
  --debug, -d               デバッグモードを有効化
  --port=<ポート番号>        サーバーのポート番号を指定
  --no-fallback             フォールバック機能を無効化
  --no-update               自動更新を無効化
  --log-level=<レベル>       ログレベルを設定 (debug, info, warn, error)

例:
  ./genellm ask "日本の首都は？"
  ./genellm chat
  ./genellm server --port=8080
  ./genellm web
  ./genellm update
  ./genellm config --port=8080 --debug=true

詳細は ./genellm help <コマンド> で確認できます。
EOF
}

# コマンド別のヘルプを表示
show_command_help() {
    local command="$1"
    
    case "$command" in
        ask)
            cat << EOF
使用方法: ./genellm ask <質問> [オプション]

説明:
  指定した質問に対する回答を取得します。

オプション:
  --debug, -d               デバッグモードを有効化
  --no-fallback             フォールバック機能を無効化

例:
  ./genellm ask "日本の首都は？"
  ./genellm ask "C言語のポインタとは？" --debug
EOF
            ;;
        vectors)
            cat << EOF
使用方法: ./genellm vectors [オプション]

説明:
  単語ベクトルデータを表示します。

オプション:
  --limit=<数値>            表示する単語の数 (デフォルト: 10)
  --offset=<数値>           表示を開始する位置 (デフォルト: 0)

例:
  ./genellm vectors
  ./genellm vectors --limit=20
  ./genellm vectors --offset=100 --limit=5
EOF
            ;;
        chat)
            cat << EOF
使用方法: ./genellm chat [オプション]

説明:
  対話モードを開始します。質問を入力し、回答を得ることができます。
  終了するには "exit" または "quit" と入力してください。

オプション:
  --debug, -d               デバッグモードを有効化
  --no-fallback             フォールバック機能を無効化

例:
  ./genellm chat
  ./genellm chat --debug
EOF
            ;;
        server)
            cat << EOF
使用方法: ./genellm server [オプション]

説明:
  GeneLLM APIサーバーを起動します。

オプション:
  --port=<ポート番号>        サーバーのポート番号を指定 (デフォルト: 50065)
  --debug, -d               デバッグモードを有効化
  --foreground, -f          フォアグラウンドで実行

例:
  ./genellm server
  ./genellm server --port=8080
  ./genellm server --debug --foreground
EOF
            ;;
        web)
            cat << EOF
使用方法: ./genellm web [オプション]

説明:
  GeneLLM Webインターフェースを起動します。
  ブラウザで http://localhost:<ポート番号>/ にアクセスしてください。

オプション:
  --port=<ポート番号>        サーバーのポート番号を指定 (デフォルト: 50065)
  --debug, -d               デバッグモードを有効化
  --foreground, -f          フォアグラウンドで実行

例:
  ./genellm web
  ./genellm web --port=8080
  ./genellm web --debug --foreground
EOF
            ;;
        update)
            cat << EOF
使用方法: ./genellm update [オプション]

説明:
  知識ベースを更新します。knowledgeとdocsディレクトリから新しい知識を抽出します。

オプション:
  --force, -f               すべてのファイルを強制的に再処理

例:
  ./genellm update
  ./genellm update --force
EOF
            ;;
        compile)
            cat << EOF
使用方法: ./genellm compile [オプション]

説明:
  GeneLLMのソースコードをコンパイルします。

オプション:
  --clean, -c               ビルドディレクトリをクリーンアップ
  --optimize, -O            最適化を有効化

例:
  ./genellm compile
  ./genellm compile --clean
  ./genellm compile --optimize
EOF
            ;;
        status)
            cat << EOF
使用方法: ./genellm status

説明:
  GeneLLMサーバーの状態を確認します。

例:
  ./genellm status
EOF
            ;;
        stop)
            cat << EOF
使用方法: ./genellm stop

説明:
  実行中のGeneLLMサーバーを停止します。

例:
  ./genellm stop
EOF
            ;;
        config)
            cat << EOF
使用方法: ./genellm config [設定名=値 ...]

説明:
  GeneLLMの設定を表示または変更します。
  引数なしで実行すると、現在の設定を表示します。

設定可能な項目:
  port=<ポート番号>          サーバーのポート番号
  debug=<true|false>        デバッグモードの有効/無効
  fallback=<true|false>     フォールバック機能の有効/無効
  auto_update=<true|false>  自動更新の有効/無効
  log_level=<レベル>         ログレベル (debug, info, warn, error)

例:
  ./genellm config
  ./genellm config port=8080
  ./genellm config debug=true fallback=false
EOF
            ;;
        *)
            show_usage
            ;;
    esac
}

# 設定を保存
save_config() {
    cat > "$CONFIG_FILE" << EOF
# GeneLLM 設定ファイル
# 自動生成されました - 手動で編集しないでください

PORT=$PORT
DEBUG=$DEBUG
FALLBACK=$FALLBACK
AUTO_UPDATE=$AUTO_UPDATE
LOG_LEVEL=$LOG_LEVEL
EOF
    log info "設定を保存しました: $CONFIG_FILE"
}

# サーバーの状態を確認
check_server_status() {
    if [ -f "$PID_FILE" ]; then
        local pid=$(cat "$PID_FILE")
        if ps -p "$pid" > /dev/null; then
            return 0  # 実行中
        else
            rm -f "$PID_FILE"  # 古いPIDファイルを削除
            return 1  # 停止中
        fi
    else
        return 1  # 停止中
    fi
}

# ポートが使用可能かチェック
check_port_available() {
    local port="$1"
    
    # 一時的なサーバーソケットを作成してバインドを試みる
    python3 -c "
import socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    s.bind(('0.0.0.0', $port))
    s.close()
    exit(0)  # 利用可能
except:
    exit(1)  # 使用中
" >/dev/null 2>&1
    
    return $?  # 0=利用可能, 1=使用中
}

# 利用可能なポートを見つける
find_available_port() {
    local start_port="$1"
    local port="$start_port"
    local max_attempts=100
    local attempt=0
    
    # Pythonを使って利用可能なポートを見つける（より確実な方法）
    local available_port=$(python3 -c "
import socket
def find_port(start_port):
    port = start_port
    max_port = 65535
    
    for _ in range(100):  # 最大100回試行
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.bind(('0.0.0.0', port))
            s.close()
            return port  # 利用可能なポートを返す
        except:
            port += 1
            if port >= max_port:
                port = 50000  # 65535を超えたら50000に戻る
    
    return None  # 利用可能なポートが見つからなかった

port = find_port($start_port)
if port:
    print(port)
" 2>/dev/null)
    
    if [ -n "$available_port" ]; then
        echo "$available_port"
        return 0
    else
        log error "利用可能なポートが見つかりませんでした"
        return 1
    fi
}

# サーバーを起動
start_server() {
    local foreground="$1"
    
    # 既に実行中かチェック
    if check_server_status; then
        log error "サーバーは既に実行中です (PID: $(cat "$PID_FILE"))"
        return 1
    fi
    
    # 知識ベースの自動更新
    if [ "$AUTO_UPDATE" = "true" ]; then
        log info "知識ベースを更新しています..."
        "$SCRIPTS_DIR/update_knowledge_optimized.sh" > /dev/null
    fi
    
    # 利用可能なポートを見つける
    local port_to_use="$PORT"
    if ! python3 -c "import socket; s = socket.socket(); s.bind(('0.0.0.0', $PORT)); s.close()" 2>/dev/null; then
        log warn "ポート $PORT は既に使用されています。別のポートを探します..."
        
        # 次の利用可能なポートを探す
        for ((p=$PORT+1; p<65535; p++)); do
            if python3 -c "import socket; s = socket.socket(); s.bind(('0.0.0.0', $p)); s.close()" 2>/dev/null; then
                port_to_use="$p"
                log info "ポート $port_to_use を使用します"
                PORT="$port_to_use"
                save_config
                break
            fi
        done
    fi
    
    # サーバーを起動
    log info "サーバーを起動しています (ポート: $PORT)..."
    
    if [ "$foreground" = "true" ]; then
        # フォアグラウンドで実行
        cd "$WEB_DIR" && ./gllm_rest_server --port "$PORT" $([ "$DEBUG" = "true" ] && echo "--debug")
    else
        # バックグラウンドで実行
        cd "$WEB_DIR" && ./gllm_rest_server --port "$PORT" $([ "$DEBUG" = "true" ] && echo "--debug") > "$LOG_DIR/server.log" 2>&1 &
        local server_pid=$!
        echo "$server_pid" > "$PID_FILE"
        
        # サーバーが正常に起動したか確認
        sleep 2
        if kill -0 "$server_pid" 2>/dev/null; then
            log info "サーバーをバックグラウンドで起動しました (PID: $server_pid)"
            log info "ログファイル: $LOG_DIR/server.log"
        else
            log error "サーバーの起動に失敗しました"
            cat "$LOG_DIR/server.log"
            rm -f "$PID_FILE"
            return 1
        fi
    fi
}

# サーバーを停止
stop_server() {
    if check_server_status; then
        local pid=$(cat "$PID_FILE")
        log info "サーバーを停止しています (PID: $pid)..."
        kill "$pid"
        rm -f "$PID_FILE"
        log info "サーバーを停止しました"
        return 0
    else
        log error "実行中のサーバーが見つかりません"
        return 1
    fi
}

# 質問に回答
ask_question() {
    local question="$1"
    local debug_flag=""
    local fallback_flag=""
    
    [ "$DEBUG" = "true" ] && debug_flag="-d"
    [ "$FALLBACK" = "false" ] && fallback_flag="--no-fallback"
    
    if [ -z "$question" ]; then
        log error "質問が指定されていません"
        show_command_help "ask"
        return 1
    fi
    
    if [ "$FALLBACK" = "true" ]; then
        # DNAコード検索を使用
        if [[ "$question" == *"GeneLLM"* || "$question" == *"知識ベース"* || 
              "$question" == *"自然言語処理"* || "$question" == *"機械学習"* ]]; then
            "$BIN_DIR/genellm_dna" "$question"
        else
            "$WORKSPACE_DIR/gllm_enhanced" $debug_flag "$question"
        fi
    else
        "$BIN_DIR/gllm" $debug_flag "$question"
    fi
}

# 対話モード
start_chat() {
    local debug_flag=""
    [ "$DEBUG" = "true" ] && debug_flag="-d"
    
    echo "GeneLLM 対話モード (終了するには 'exit' または 'quit' と入力)"
    echo "--------------------------------------------------------------"
    
    while true; do
        echo -n "> "
        read -r question
        
        case "$question" in
            exit|quit)
                echo "対話モードを終了します"
                break
                ;;
            *)
                if [ -n "$question" ]; then
                    if [ "$FALLBACK" = "true" ]; then
                        # DNAコード検索を使用
                        if [[ "$question" == *"GeneLLM"* || "$question" == *"知識ベース"* || 
                              "$question" == *"自然言語処理"* || "$question" == *"機械学習"* ]]; then
                            "$BIN_DIR/genellm_dna" "$question"
                        else
                            "$WORKSPACE_DIR/gllm_enhanced" $debug_flag "$question"
                        fi
                    else
                        "$BIN_DIR/gllm" $debug_flag "$question"
                    fi
                    echo ""
                fi
                ;;
        esac
    done
}

# ソースコードをコンパイル
compile_source() {
    local clean="$1"
    local optimize="$2"
    
    log info "GeneLLMをコンパイルしています..."
    
    # ビルドディレクトリをクリーンアップ
    if [ "$clean" = "true" ]; then
        log info "ビルドディレクトリをクリーンアップしています..."
        rm -rf "$WORKSPACE_DIR/build"
    fi
    
    # ビルドディレクトリを作成
    mkdir -p "$WORKSPACE_DIR/build"
    cd "$WORKSPACE_DIR/build"
    
    # CMakeを実行
    log info "CMakeを実行しています..."
    cmake_options=""
    [ "$optimize" = "true" ] && cmake_options="-DCMAKE_BUILD_TYPE=Release"
    
    cmake $cmake_options .. || {
        log error "CMakeの実行に失敗しました"
        return 1
    }
    
    # ビルド
    log info "ビルドを実行しています..."
    make || {
        log error "ビルドに失敗しました"
        return 1
    }
    
    log info "コンパイルが完了しました"
    return 0
}

# 知識ベースを更新
update_knowledge() {
    local force="$1"
    
    log info "知識ベースを更新しています..."
    
    if [ "$force" = "true" ]; then
        log info "すべてのファイルを強制的に再処理します..."
        rm -f "/tmp/knowledge_update/processed_files.txt"
    fi
    
    "$SCRIPTS_DIR/update_knowledge_optimized.sh" || {
        log error "知識ベースの更新に失敗しました"
        return 1
    }
    
    log info "知識ベースの更新が完了しました"
    return 0
}

# 設定を表示/変更
manage_config() {
    # 引数がない場合は現在の設定を表示
    if [ $# -eq 0 ]; then
        echo "現在の設定:"
        echo "  ポート番号: $PORT"
        echo "  デバッグモード: $DEBUG"
        echo "  フォールバック機能: $FALLBACK"
        echo "  自動更新: $AUTO_UPDATE"
        echo "  ログレベル: $LOG_LEVEL"
        return 0
    fi
    
    # 設定を変更
    local changed=false
    
    for arg in "$@"; do
        local key="${arg%%=*}"
        local value="${arg#*=}"
        
        case "$key" in
            port)
                if [[ "$value" =~ ^[0-9]+$ ]]; then
                    PORT="$value"
                    changed=true
                else
                    log error "無効なポート番号: $value"
                fi
                ;;
            debug)
                if [ "$value" = "true" ] || [ "$value" = "false" ]; then
                    DEBUG="$value"
                    changed=true
                else
                    log error "無効な値 (debug): $value (true または false を指定してください)"
                fi
                ;;
            fallback)
                if [ "$value" = "true" ] || [ "$value" = "false" ]; then
                    FALLBACK="$value"
                    changed=true
                else
                    log error "無効な値 (fallback): $value (true または false を指定してください)"
                fi
                ;;
            auto_update)
                if [ "$value" = "true" ] || [ "$value" = "false" ]; then
                    AUTO_UPDATE="$value"
                    changed=true
                else
                    log error "無効な値 (auto_update): $value (true または false を指定してください)"
                fi
                ;;
            log_level)
                if [ "$value" = "debug" ] || [ "$value" = "info" ] || [ "$value" = "warn" ] || [ "$value" = "error" ]; then
                    LOG_LEVEL="$value"
                    changed=true
                else
                    log error "無効なログレベル: $value (debug, info, warn, error のいずれかを指定してください)"
                fi
                ;;
            *)
                log error "未知の設定: $key"
                ;;
        esac
    done
    
    # 設定が変更された場合は保存
    if [ "$changed" = "true" ]; then
        save_config
        echo "設定を更新しました:"
        echo "  ポート番号: $PORT"
        echo "  デバッグモード: $DEBUG"
        echo "  フォールバック機能: $FALLBACK"
        echo "  自動更新: $AUTO_UPDATE"
        echo "  ログレベル: $LOG_LEVEL"
    fi
}

# ベクトルデータを表示
show_vectors() {
    local limit=${1:-10}
    local offset=${2:-0}
    local vector_file="$WORKSPACE_DIR/data/word_vectors.dat"
    
    if [ ! -f "$vector_file" ]; then
        log error "ベクトルデータファイルが見つかりません: $vector_file"
        return 1
    fi
    
    echo "単語ベクトルデータ（位置 $offset から $limit 件表示）："
    echo "----------------------------------------"
    
    # ファイルの行数を取得
    local total_lines=$(wc -l < "$vector_file")
    echo "総単語数: $total_lines"
    echo ""
    
    # 指定された範囲の行を表示
    if [ "$offset" -ge "$total_lines" ]; then
        log error "指定されたオフセット($offset)が総単語数($total_lines)を超えています"
        return 1
    fi
    
    head -n $((offset + limit)) "$vector_file" | tail -n $limit | while read -r line; do
        # 単語と最初の数個のベクトル値を表示
        word=$(echo "$line" | cut -d' ' -f1)
        vector_preview=$(echo "$line" | cut -d' ' -f2-11 | tr ' ' ', ')
        echo "単語: $word"
        echo "ベクトル: [$vector_preview, ...]"
        echo "----------------------------------------"
    done
}

# メイン処理
main() {
    # コマンドライン引数がない場合はヘルプを表示
    if [ $# -eq 0 ]; then
        show_usage
        return 0
    fi
    
    # コマンドを取得
    local command="$1"
    shift
    
    # オプションを解析
    local debug_flag=false
    local foreground=false
    local force=false
    local clean=false
    local optimize=false
    
    while [ $# -gt 0 ]; do
        case "$1" in
            --debug|-d)
                DEBUG=true
                shift
                ;;
            --port=*)
                PORT="${1#*=}"
                shift
                ;;
            --no-fallback)
                FALLBACK=false
                shift
                ;;
            --no-update)
                AUTO_UPDATE=false
                shift
                ;;
            --log-level=*)
                LOG_LEVEL="${1#*=}"
                shift
                ;;
            --foreground|-f)
                foreground=true
                shift
                ;;
            --force|-f)
                force=true
                shift
                ;;
            --clean|-c)
                clean=true
                shift
                ;;
            --optimize|-O)
                optimize=true
                shift
                ;;
            -*)
                log error "未知のオプション: $1"
                return 1
                ;;
            *)
                break
                ;;
        esac
    done
    
    # コマンドを実行
    case "$command" in
        ask)
            ask_question "$*"
            ;;
        chat)
            start_chat
            ;;
        server)
            start_server "$foreground"
            ;;
        web)
            start_server "$foreground"
            ;;
        update)
            update_knowledge "$force"
            ;;
        compile)
            compile_source "$clean" "$optimize"
            ;;
        status)
            if check_server_status; then
                echo "GeneLLMサーバーは実行中です (PID: $(cat "$PID_FILE"), ポート: $PORT)"
                return 0
            else
                echo "GeneLLMサーバーは停止しています"
                return 1
            fi
            ;;
        stop)
            stop_server
            ;;
        config)
            manage_config "$@"
            ;;
        help)
            if [ $# -gt 0 ]; then
                show_command_help "$1"
            else
                show_usage
            fi
            ;;
        version)
            echo "GeneLLM v$VERSION"
            ;;
        vectors)
            local limit=10
            local offset=0
            
            # 引数として直接数値が指定された場合
            if [[ $# -ge 1 && "$1" =~ ^[0-9]+$ ]]; then
                limit="$1"
                shift
                if [[ $# -ge 1 && "$1" =~ ^[0-9]+$ ]]; then
                    offset="$1"
                    shift
                fi
            else
                # オプションを解析
                for arg in "$@"; do
                    case "$arg" in
                        --limit=*)
                            limit="${arg#*=}"
                            ;;
                        --offset=*)
                            offset="${arg#*=}"
                            ;;
                    esac
                done
            fi
            
            show_vectors "$limit" "$offset"
            ;;
        *)
            log error "未知のコマンド: $command"
            show_usage
            return 1
            ;;
    esac
}

# スクリプトを実行
main "$@"