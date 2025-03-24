#ifndef KNOWLEDGE_MANAGER_H
#define KNOWLEDGE_MANAGER_H

// 知識ベース更新関数
int update_knowledge();

// トークナイズ処理関数
int tokenize_knowledge();

// 単語ベクトル更新関数
int update_word_vectors(const char *tokenized_dir);

#endif // KNOWLEDGE_MANAGER_H