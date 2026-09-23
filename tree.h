#ifndef TREE_H
#define TREE_H

/* 각 노드는 데이터와 두 자식(왼쪽, 오른쪽)의 주소를 저장합니다. */
typedef struct Node {
    char data;
    struct Node* left;
    struct Node* right;
} Node;

/* 이진 트리 구조체 definition */
typedef struct {
    Node* root;
    int count;                  /* 현재 생성된 노드 수 */
    int size;                   /* 트리에 저장 가능한 최대 노드 수 */
} Tree;

/* 트리 생성 및 탐색 함수 */
Tree* create_btree(int size);
Node* find_node(Tree* tree, const char* path);

/* 노드 조작 연산 함수 (실패 시 기존 트리 상태를 유지함) */
Tree* insert_root(Tree* tree, char value);
Tree* insert_child(Tree* tree, const char* path, char child, char value);
Tree* delete_node(Tree* tree, const char* path);
Tree* update_value(Tree* tree, const char* path, char value);

/* 트리 조회 및 출력 함수 */
int read_child(Tree* tree, const char* path, char* outBuf, int bufSize);
void print_btree(Tree* tree);
void destroy_btree(Tree* tree);

#endif
