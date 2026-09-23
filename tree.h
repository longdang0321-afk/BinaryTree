#ifndef TREE_H
#define TREE_H

/* 각 노드는 데이터와 두 자식의 주소를 저장한다. */
typedef struct Node
{
    char data;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct
{
    Node *root;
    int count; /* 현재 노드 수 */
    int size;  /* 최대 노드 수 */
} Tree;

Tree *create_btree(int size);
Node *find_node(Tree *tree, const char *path);
/* 수정 연산은 변경된 트리를 반환한다. 오류가 나면 기존 상태를 유지한다. */
Tree *insert_root(Tree *tree, char value);
Tree *insert_child(Tree *tree, const char *path, char child, char value);
Tree *delete_node(Tree *tree, const char *path);
Tree *update_value(Tree *tree, const char *path, char value);
/* 조회 성공 시 outBuf에 결과를 저장하고 1, 실패 시 0을 반환한다. */
int read_child(Tree *tree, const char *path, char *outBuf, int bufSize);
void print_btree(Tree *tree);
void destroy_btree(Tree *tree);

#endif
