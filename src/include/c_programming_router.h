#ifndef C_PROGRAMMING_ROUTER_H
#define C_PROGRAMMING_ROUTER_H

// ルーティングパターン
typedef struct {
    char pattern[256];      // マッチングパターン
    float confidence;       // 信頼度
    char response[4096];    // 応答テンプレート
    int is_code_generator;  // コード生成器かどうか
    char code_template[4096]; // コードテンプレート
} RoutingPattern;

// ルーター
typedef struct {
    RoutingPattern patterns[50]; // ルーティングパターン配列
    int pattern_count;          // パターン数
} CRouter;

// ルーターの初期化
CRouter* c_router_init();

// ルーターの解放
void c_router_free(CRouter* router);

// ルーティングパターンの追加
int c_router_add_pattern(CRouter* router, const char* pattern, float confidence, 
                        const char* response, int is_code_generator, const char* code_template);

// 文字列が特定のパターンを含むかチェック
int contains_pattern(const char* text, const char* pattern);

// 入力に対する最適なルーティングパターンを見つける
RoutingPattern* c_router_find_best_match(CRouter* router, const char* input);

// コードテンプレートを入力に基づいてカスタマイズ
char* customize_code_template(const char* template, const char* input);

// 入力に対する応答を生成
char* c_router_generate_response(CRouter* router, const char* input);

// C言語ルーターの初期化と基本パターンの設定
CRouter* init_c_programming_router();

#endif // C_PROGRAMMING_ROUTER_H