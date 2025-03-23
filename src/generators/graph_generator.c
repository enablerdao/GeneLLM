#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_STR_LEN 1024
#define MAX_NODES 100
#define MAX_EDGES 500
#define MAX_TEMPLATES 10

// ノードの種類
typedef enum {
    SUBJECT,  // 主語
    VERB,     // 動詞
    RESULT,   // 結果
    TEMPLATE  // テンプレート
} NodeType;

// グラフのノード
typedef struct {
    int id;
    NodeType type;
    char value[MAX_STR_LEN];
} Node;

// グラフのエッジ
typedef struct {
    int from;
    int to;
    float weight;
} Edge;

// グラフ構造
typedef struct {
    Node nodes[MAX_NODES];
    int node_count;
    Edge edges[MAX_EDGES];
    int edge_count;
} Graph;

// グラフの初期化
void init_graph(Graph* graph) {
    graph->node_count = 0;
    graph->edge_count = 0;
}

// ノードの追加
int add_node(Graph* graph, NodeType type, const char* value) {
    if (graph->node_count >= MAX_NODES) {
        return -1;  // グラフが満杯
    }
    
    int id = graph->node_count;
    graph->nodes[id].id = id;
    graph->nodes[id].type = type;
    strncpy(graph->nodes[id].value, value, MAX_STR_LEN - 1);
    graph->nodes[id].value[MAX_STR_LEN - 1] = '\0';
    
    graph->node_count++;
    return id;
}

// エッジの追加
bool add_edge(Graph* graph, int from, int to, float weight) {
    if (graph->edge_count >= MAX_EDGES) {
        return false;  // エッジが満杯
    }
    
    if (from < 0 || from >= graph->node_count || to < 0 || to >= graph->node_count) {
        return false;  // 無効なノードID
    }
    
    graph->edges[graph->edge_count].from = from;
    graph->edges[graph->edge_count].to = to;
    graph->edges[graph->edge_count].weight = weight;
    
    graph->edge_count++;
    return true;
}

// ノードの検索
int find_node(Graph* graph, NodeType type, const char* value) {
    for (int i = 0; i < graph->node_count; i++) {
        if (graph->nodes[i].type == type && strcmp(graph->nodes[i].value, value) == 0) {
            return i;
        }
    }
    return -1;  // 見つからない
}

// ノードの隣接ノードを取得
int get_adjacent_nodes(Graph* graph, int node_id, int* adjacent, float* weights) {
    int count = 0;
    
    for (int i = 0; i < graph->edge_count; i++) {
        if (graph->edges[i].from == node_id) {
            adjacent[count] = graph->edges[i].to;
            weights[count] = graph->edges[i].weight;
            count++;
        }
    }
    
    return count;
}

// 重みに基づいてランダムに隣接ノードを選択
int select_random_adjacent(int* adjacent, float* weights, int count) {
    if (count == 0) {
        return -1;
    }
    
    // 重みの合計を計算
    float total_weight = 0.0f;
    for (int i = 0; i < count; i++) {
        total_weight += weights[i];
    }
    
    // ランダムな値を生成
    float random_value = ((float)rand() / RAND_MAX) * total_weight;
    
    // 重みに基づいて選択
    float cumulative_weight = 0.0f;
    for (int i = 0; i < count; i++) {
        cumulative_weight += weights[i];
        if (random_value <= cumulative_weight) {
            return adjacent[i];
        }
    }
    
    return adjacent[count - 1];  // フォールバック
}

// 文字列内のプレースホルダーを置換
void replace_placeholder(char* str, const char* placeholder, const char* value) {
    char result[MAX_STR_LEN];
    char* pos = strstr(str, placeholder);
    
    if (pos) {
        int prefix_len = pos - str;
        strncpy(result, str, prefix_len);
        result[prefix_len] = '\0';
        strcat(result, value);
        strcat(result, pos + strlen(placeholder));
        strcpy(str, result);
    }
}

// グラフを使って文を生成
void generate_sentence(Graph* graph, char* sentence) {
    // テンプレートをランダムに選択
    int template_ids[MAX_TEMPLATES];
    int template_count = 0;
    
    for (int i = 0; i < graph->node_count; i++) {
        if (graph->nodes[i].type == TEMPLATE) {
            template_ids[template_count++] = i;
            if (template_count >= MAX_TEMPLATES) {
                break;
            }
        }
    }
    
    if (template_count == 0) {
        strcpy(sentence, "テンプレートがありません。");
        return;
    }
    
    int template_id = template_ids[rand() % template_count];
    char template_str[MAX_STR_LEN];
    strcpy(template_str, graph->nodes[template_id].value);
    
    // 主語を選択
    int subject_ids[MAX_NODES];
    int subject_count = 0;
    
    for (int i = 0; i < graph->node_count; i++) {
        if (graph->nodes[i].type == SUBJECT) {
            subject_ids[subject_count++] = i;
        }
    }
    
    if (subject_count == 0) {
        strcpy(sentence, "主語がありません。");
        return;
    }
    
    int subject_id = subject_ids[rand() % subject_count];
    
    // 主語から到達可能な動詞を選択
    int adjacent[MAX_NODES];
    float weights[MAX_NODES];
    int adj_count = get_adjacent_nodes(graph, subject_id, adjacent, weights);
    
    if (adj_count == 0) {
        strcpy(sentence, "動詞がありません。");
        return;
    }
    
    int verb_id = select_random_adjacent(adjacent, weights, adj_count);
    
    // 動詞から到達可能な結果を選択
    adj_count = get_adjacent_nodes(graph, verb_id, adjacent, weights);
    
    if (adj_count == 0) {
        strcpy(sentence, "結果がありません。");
        return;
    }
    
    int result_id = select_random_adjacent(adjacent, weights, adj_count);
    
    // テンプレートに値を埋め込む
    char result[MAX_STR_LEN];
    strcpy(result, template_str);
    
    // プレースホルダーを置換
    replace_placeholder(result, "{主語}", graph->nodes[subject_id].value);
    replace_placeholder(result, "{動詞}", graph->nodes[verb_id].value);
    replace_placeholder(result, "{結果}", graph->nodes[result_id].value);
    
    strcpy(sentence, result);
}

// グラフの内容を表示
void print_graph(Graph* graph) {
    printf("ノード一覧:\n");
    for (int i = 0; i < graph->node_count; i++) {
        printf("  ID: %d, タイプ: ", graph->nodes[i].id);
        switch (graph->nodes[i].type) {
            case SUBJECT: printf("主語"); break;
            case VERB: printf("動詞"); break;
            case RESULT: printf("結果"); break;
            case TEMPLATE: printf("テンプレート"); break;
        }
        printf(", 値: %s\n", graph->nodes[i].value);
    }
    
    printf("\nエッジ一覧:\n");
    for (int i = 0; i < graph->edge_count; i++) {
        int from = graph->edges[i].from;
        int to = graph->edges[i].to;
        printf("  %s (%d) -> %s (%d), 重み: %.2f\n",
               graph->nodes[from].value, from,
               graph->nodes[to].value, to,
               graph->edges[i].weight);
    }
}

int main() {
    // 乱数の初期化
    srand((unsigned int)time(NULL));
    
    // グラフの初期化
    Graph graph;
    init_graph(&graph);
    
    // テンプレートの追加
    add_node(&graph, TEMPLATE, "{主語}は{動詞}て{結果}になりました。");
    add_node(&graph, TEMPLATE, "{主語}が{動詞}たら{結果}でした。");
    add_node(&graph, TEMPLATE, "{主語}は{結果}ので{動詞}ました。");
    
    // 主語の追加
    int cat_id = add_node(&graph, SUBJECT, "猫");
    int dog_id = add_node(&graph, SUBJECT, "犬");
    int bird_id = add_node(&graph, SUBJECT, "鳥");
    int human_id = add_node(&graph, SUBJECT, "人間");
    
    // 動詞の追加
    int eat_id = add_node(&graph, VERB, "食べ");
    int drink_id = add_node(&graph, VERB, "飲み");
    int see_id = add_node(&graph, VERB, "見");
    int run_id = add_node(&graph, VERB, "走り");
    int sleep_id = add_node(&graph, VERB, "眠り");
    
    // 結果の追加
    int happy_id = add_node(&graph, RESULT, "嬉しい");
    int sad_id = add_node(&graph, RESULT, "悲しい");
    int tired_id = add_node(&graph, RESULT, "疲れた");
    int surprised_id = add_node(&graph, RESULT, "驚いた");
    int satisfied_id = add_node(&graph, RESULT, "満足した");
    
    // エッジの追加（主語 -> 動詞）
    add_edge(&graph, cat_id, eat_id, 0.7f);
    add_edge(&graph, cat_id, sleep_id, 0.8f);
    add_edge(&graph, cat_id, see_id, 0.5f);
    
    add_edge(&graph, dog_id, eat_id, 0.6f);
    add_edge(&graph, dog_id, run_id, 0.9f);
    add_edge(&graph, dog_id, sleep_id, 0.4f);
    
    add_edge(&graph, bird_id, eat_id, 0.5f);
    add_edge(&graph, bird_id, see_id, 0.7f);
    
    add_edge(&graph, human_id, eat_id, 0.6f);
    add_edge(&graph, human_id, drink_id, 0.7f);
    add_edge(&graph, human_id, see_id, 0.8f);
    add_edge(&graph, human_id, run_id, 0.5f);
    add_edge(&graph, human_id, sleep_id, 0.6f);
    
    // エッジの追加（動詞 -> 結果）
    add_edge(&graph, eat_id, happy_id, 0.7f);
    add_edge(&graph, eat_id, satisfied_id, 0.8f);
    
    add_edge(&graph, drink_id, satisfied_id, 0.6f);
    
    add_edge(&graph, see_id, surprised_id, 0.7f);
    add_edge(&graph, see_id, happy_id, 0.4f);
    add_edge(&graph, see_id, sad_id, 0.3f);
    
    add_edge(&graph, run_id, tired_id, 0.8f);
    add_edge(&graph, run_id, happy_id, 0.5f);
    
    add_edge(&graph, sleep_id, satisfied_id, 0.7f);
    add_edge(&graph, sleep_id, tired_id, 0.3f);
    
    // グラフの内容を表示
    printf("グラフ構造:\n");
    print_graph(&graph);
    
    // 文の生成
    printf("\n生成された文:\n");
    for (int i = 0; i < 5; i++) {
        char sentence[MAX_STR_LEN];
        generate_sentence(&graph, sentence);
        printf("%d: %s\n", i + 1, sentence);
    }
    
    return 0;
}