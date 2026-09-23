#ifndef TREE_H
#define TREE_H

/* 이진트리의 노드 */
typedef struct Node {
    char data;
    struct Node* left;
    struct Node* right;
} Node;

/* 이진트리 (root 포인터만 가지고 있으면 됨) */
typedef struct {
    Node* root;
    int count;
} Tree;

Tree* create_btree(int size);
void destroy_btree(Tree* tree);

/* path("/A/B/C" 형태)로 노드를 찾는다. 못 찾으면 NULL 반환 */
Node* find_node(Tree* tree, char* path);

/* insert_root: 성공하면 1, 이미 root가 있으면 0 */
int insert_root(Tree* tree, char value);

/*
 * insert_child: 반환값
 *   1  = 성공
 *  -1  = parent 노드를 찾을 수 없음
 *  -2  = parent가 이미 자식 2개를 가짐
 *  -3  = 지정한 위치(L/R)에 이미 자식이 있음
 *  -4  = 형제 노드와 데이터가 중복됨
 *  -5  = child가 L 또는 R이 아님
 */
int insert_child(Tree* tree, char* parentPath, char child, char value);

/*
 * delete_node: 반환값
 *   1 = 성공, -1 = 노드를 찾을 수 없음, -2 = 단말 노드가 아님
 */
int delete_node(Tree* tree, char* path);

/*
 * update_value: 반환값
 *   1 = 성공, -1 = 노드를 찾을 수 없음, -2 = 형제 노드와 데이터 중복
 */
int update_value(Tree* tree, char* path, char value);

/* read_child: 찾으면 1(결과는 outBuf에 저장), 못 찾으면 0 */
int read_child(Tree* tree, char* path, char* outBuf, int bufSize);

void print_btree(Tree* tree);

#endif
