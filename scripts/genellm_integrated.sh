#!/bin/bash

# GeneLLM 統合コマンド
# 使用方法: ./genellm [コマンド] [引数]

# 定数
VERSION="1.1.0"
WORKSPACE_DIR="/workspace"
BIN_DIR="$WORKSPACE_DIR/bin"
SCRIPTS_DIR="$WORKSPACE_DIR/scripts"
USE_C_VERSION="true"  # C言語実装を使用するかどうか
CONFIG_FILE="$WORKSPACE_DIR/.genellm_config"
LOG_DIR="$WORKSPACE_DIR/logs"

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

# ログディレクトリの作成
mkdir -p "$LOG_DIR"

# ログ関数
log() {
    local level="$1"
    local message="$2"
    local timestamp=$(date +"%Y-%m-%d %H:%M:%S")
    
    case "$level" in
        debug)
            if [ "$LOG_LEVEL" = "debug" ]; then
                echo "[$timestamp] [DEBUG] $message" >> "$LOG_DIR/genellm.log"
                [ "$DEBUG" = "true" ] && echo "[DEBUG] $message"
            fi
            ;;
        info)
            if [[ "$LOG_LEVEL" = "debug" || "$LOG_LEVEL" = "info" ]]; then
                echo "[$timestamp] [INFO] $message" >> "$LOG_DIR/genellm.log"
                echo "[INFO] $message"
            fi
            ;;
        warn)
            if [[ "$LOG_LEVEL" = "debug" || "$LOG_LEVEL" = "info" || "$LOG_LEVEL" = "warn" ]]; then
                echo "[$timestamp] [WARN] $message" >> "$LOG_DIR/genellm.log"
                echo "[WARN] $message" >&2
            fi
            ;;
        error)
            echo "[$timestamp] [ERROR] $message" >> "$LOG_DIR/genellm.log"
            echo "[ERROR] $message" >&2
            ;;
    esac
}

# 使用方法を表示
show_usage() {
    cat << EOF
GeneLLM 統合コマンド v$VERSION

使用方法: ./genellm [コマンド] [引数]

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
  vectors [limit] [offset]  単語ベクトルデータを表示
  words [limit] [offset]    単語リストを表示
  tokenize <テキスト>        テキストをトークナイズして表示
  dna-compress <テキスト>    テキストをDNAコードに圧縮
  dna-decompress <コード>    DNAコードからテキストを復元
  verb-info <動詞>           動詞の活用形と種類を表示
  similar <単語> [limit]     類似単語を検索
  compare <単語1> <単語2>    2つの単語の類似度を計算
  dna-dict <サブコマンド>    DNA辞書を管理
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
  ./genellm tokenize "こんにちは世界"
  ./genellm dna-compress "猫が魚を食べる"
  ./genellm similar "猫" 5
EOF
}

# コマンド別のヘルプを表示
show_command_help() {
    local command="$1"
    
    case "$command" in
        vectors)
            cat << EOF
使用方法: ./genellm vectors [limit] [offset]

説明:
  単語ベクトルデータを表示します。

引数:
  limit                     表示する単語の数 (デフォルト: 10)
  offset                    表示を開始する位置 (デフォルト: 0)

例:
  ./genellm vectors
  ./genellm vectors 20
  ./genellm vectors 5 100
EOF
            ;;
        words)
            cat << EOF
使用方法: ./genellm words [limit] [offset]

説明:
  単語リストを表示します。

引数:
  limit                     表示する単語の数 (デフォルト: 10)
  offset                    表示を開始する位置 (デフォルト: 0)

例:
  ./genellm words
  ./genellm words 20
  ./genellm words 5 100
EOF
            ;;
        tokenize)
            cat << EOF
使用方法: ./genellm tokenize <テキスト>

説明:
  テキストをトークナイズして表示します。

引数:
  テキスト                  トークナイズするテキスト

例:
  ./genellm tokenize "こんにちは世界"
  ./genellm tokenize "GeneLLMはC言語で実装された軽量AIです"
EOF
            ;;
        dna-compress)
            cat << EOF
使用方法: ./genellm dna-compress <テキスト>

説明:
  テキストをDNAコードに圧縮します。

引数:
  テキスト                  圧縮するテキスト

例:
  ./genellm dna-compress "猫が魚を食べる"
  ./genellm dna-compress "犬が骨を噛む"
EOF
            ;;
        dna-decompress)
            cat << EOF
使用方法: ./genellm dna-decompress <DNAコード>

説明:
  DNAコードからテキストを復元します。

引数:
  DNAコード                 復元するDNAコード（例: E00C01R02）

例:
  ./genellm dna-decompress "E00C00R00"
  ./genellm dna-decompress "E01C01R01"
EOF
            ;;
        verb-info)
            cat << EOF
使用方法: ./genellm verb-info <動詞>

説明:
  動詞の活用形と種類を表示します。

引数:
  動詞                      活用情報を表示する動詞

例:
  ./genellm verb-info "食べる"
  ./genellm verb-info "走る"
EOF
            ;;
        similar)
            cat << EOF
使用方法: ./genellm similar <単語> [limit]

説明:
  指定した単語に類似した単語を検索します。

引数:
  単語                      検索する単語
  limit                     表示する類似単語の数 (デフォルト: 10)

例:
  ./genellm similar "猫"
  ./genellm similar "猫" 5
EOF
            ;;
        compare)
            cat << EOF
使用方法: ./genellm compare <単語1> <単語2>

説明:
  2つの単語間の類似度を計算します。

引数:
  単語1                     比較する1つ目の単語
  単語2                     比較する2つ目の単語

例:
  ./genellm compare "猫" "犬"
  ./genellm compare "車" "自動車"
EOF
            ;;
        dna-dict)
            cat << EOF
使用方法: ./genellm dna-dict <サブコマンド> [引数]

説明:
  DNA辞書を管理します。

サブコマンド:
  show [limit]              DNA辞書の内容を表示
  add <type> <word>         DNA辞書にエントリを追加
  delete <code>             DNA辞書からエントリを削除
  export [output_file]      DNA辞書をエクスポート
  import <input_file>       DNA辞書をインポート
  search <keyword>          DNA辞書を検索

例:
  ./genellm dna-dict show
  ./genellm dna-dict add E 人間
  ./genellm dna-dict search "猫"
EOF
            ;;
        *)
            show_usage
            ;;
    esac
}

# メイン処理
main() {
    # コマンドライン引数の解析
    local command=""
    local args=()
    
    # コマンドを取得
    if [ $# -gt 0 ]; then
        command="$1"
        shift
    fi
    
    # 引数を処理
    while [ $# -gt 0 ]; do
        case "$1" in
            --debug|-d)
                DEBUG=true
                ;;
            --port=*)
                PORT="${1#*=}"
                ;;
            --no-fallback)
                FALLBACK=false
                ;;
            --no-update)
                AUTO_UPDATE=false
                ;;
            --log-level=*)
                LOG_LEVEL="${1#*=}"
                ;;
            *)
                args+=("$1")
                ;;
        esac
        shift
    done
    
    # コマンドの実行
    case "$command" in
        vectors)
            local limit=10
            local offset=0
            
            if [ ${#args[@]} -ge 1 ]; then
                limit="${args[0]}"
            fi
            if [ ${#args[@]} -ge 2 ]; then
                offset="${args[1]}"
            fi
            
            "$WORKSPACE_DIR/bin/vectors" "$limit" "$offset"
            ;;
        words)
            local limit=10
            local offset=0
            
            if [ ${#args[@]} -ge 1 ]; then
                limit="${args[0]}"
            fi
            if [ ${#args[@]} -ge 2 ]; then
                offset="${args[1]}"
            fi
            
            "$WORKSPACE_DIR/bin/tokens" words "$limit" "$offset"
            ;;
        tokenize)
            if [ ${#args[@]} -ge 1 ]; then
                "$WORKSPACE_DIR/bin/tokens" tokenize "${args[0]}"
            else
                log error "テキストが指定されていません"
                show_command_help "tokenize"
                return 1
            fi
            ;;
        dna-compress)
            if [ ${#args[@]} -ge 1 ]; then
                "$WORKSPACE_DIR/bin/dna" compress "${args[0]}"
            else
                log error "テキストが指定されていません"
                show_command_help "dna-compress"
                return 1
            fi
            ;;
        dna-decompress)
            if [ ${#args[@]} -ge 1 ]; then
                "$WORKSPACE_DIR/bin/dna" decompress "${args[0]}"
            else
                log error "DNAコードが指定されていません"
                show_command_help "dna-decompress"
                return 1
            fi
            ;;
        verb-info)
            if [ ${#args[@]} -ge 1 ]; then
                "$WORKSPACE_DIR/bin/dna" verb-info "${args[0]}"
            else
                log error "動詞が指定されていません"
                show_command_help "verb-info"
                return 1
            fi
            ;;
        similar)
            local word=""
            local limit=10
            
            if [ ${#args[@]} -ge 1 ]; then
                word="${args[0]}"
            else
                log error "単語が指定されていません"
                show_command_help "similar"
                return 1
            fi
            
            if [ ${#args[@]} -ge 2 ]; then
                limit="${args[1]}"
            fi
            
            "$WORKSPACE_DIR/bin/similar" find "$word" "$limit"
            ;;
        compare)
            if [ ${#args[@]} -ge 2 ]; then
                "$WORKSPACE_DIR/bin/similar" compare "${args[0]}" "${args[1]}"
            else
                log error "2つの単語を指定してください"
                show_command_help "compare"
                return 1
            fi
            ;;
        dna-dict)
            if [ ${#args[@]} -ge 1 ]; then
                local subcommand="${args[0]}"
                args=("${args[@]:1}")
                "$WORKSPACE_DIR/bin/dna-dict" "$subcommand" "${args[@]}"
            else
                log error "サブコマンドが指定されていません"
                show_command_help "dna-dict"
                return 1
            fi
            ;;
        help)
            if [ ${#args[@]} -ge 1 ]; then
                show_command_help "${args[0]}"
            else
                show_usage
            fi
            ;;
        version)
            echo "GeneLLM v$VERSION"
            ;;
        update)
            # C言語実装の高速バージョンを使用するかどうかを確認
            if [ -f "$WORKSPACE_DIR/bin/knowledge_manager" ] && [ "$USE_C_VERSION" = "true" ]; then
                log info "C言語実装の高速バージョンを使用します"
                
                # C言語実装による知識ベース更新とトークナイズ
                time "$WORKSPACE_DIR/bin/knowledge_manager" all || {
                    log error "C言語実装による処理に失敗しました。シェルスクリプト版にフォールバックします。"
                    USE_C_VERSION="false"
                }
                
                if [ "$USE_C_VERSION" = "true" ]; then
                    log info "知識ベースの更新とトークナイズが完了しました"
                    return 0
                fi
            fi
            
            # シェルスクリプト版を使用
            log info "シェルスクリプト版を使用します"
            
            # 知識ベースの更新
            echo "知識ベースを更新しています..."
            "$WORKSPACE_DIR/scripts/update_knowledge_optimized.sh" || {
                log error "知識ベースの更新に失敗しました"
                return 1
            }
            
            # ナレッジとドキュメントのトークナイズ
            echo "ナレッジとドキュメントをトークナイズしています..."
            "$WORKSPACE_DIR/scripts/tokenize_knowledge.sh" || {
                log error "トークナイズに失敗しました"
                return 1
            }
            
            log info "知識ベースの更新とトークナイズが完了しました"
            return 0
            ;;
        ask|chat|server|web|compile|status|stop|config)
            # 既存のgenellmコマンドに転送
            "$WORKSPACE_DIR/genellm.bak" "$command" "${args[@]}"
            ;;
        "")
            show_usage
            ;;
        *)
            log error "未知のコマンド: $command"
            show_usage
            return 1
            ;;
    esac
}

# スクリプトの実行
main "$@"