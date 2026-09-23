#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

/* 오류 메시지 출력 후 트리를 반환하는 헬퍼 함수 */
static Tree* error(Tree* tree, const char* message) {
    printf("오류: %s.\n", message);
    return tree;
}

/* 지정된 size 크기의 빈 이진 트리를 생성합니다. */
Tree* create_btree(int size) {
    Tree* tree;
    if (size <= 0) return error(NULL, "크기는 1 이상이어야 합니다");
    tree = (Tree*)malloc(sizeof(Tree));
    if (tree == NULL) return error(NULL, "메모리 할당 실패");
    tree->root = NULL;
    tree->count = 0;
    tree->size = size;
    return tree;
}

/* /A/B/C 형식의 경로를 검사하고, 경로를 따라 이동하여 대상 노드와 부모 노드를 찾습니다. */
static Node* find_with_parent(Tree* tree, const char* path, Node** parentOut) {
    Node* current;
    size_t i, len;
    
    if (tree == NULL || tree->root == NULL || path == NULL) return NULL;
    
    current = tree->root;
    len = strlen(path);
    *parentOut = NULL;

    /* 경로 길이 및 /X 형식 검증 (/A -> 길이 2, /A/B -> 길이 4) */
    if (len < 2 || len % 2 != 0) return NULL;
    for (i = 0; i < len; i += 2) {
        if (path[i] != '/' || path[i + 1] < 'A' || path[i + 1] > 'Z')
            return NULL;
    }

    /* 루트 노드 데이터 일치 여부 확인 */
    if (current->data != path[1]) return NULL;

    /* 경로를 순회하며 자식 노드로 이동 */
    for (i = 3; i < len; i += 2) {
        *parentOut = current;
        if (current->left != NULL && current->left->data == path[i])
            current = current->left;
        else if (current->right != NULL && current->right->data == path[i])
            current = current->right;
        else 
            return NULL;
    }
    return current;
}

/* 경로에 해당하는 노드를 찾아 반환합니다. */
Node* find_node(Tree* tree, const char* path) {
    Node* parent = NULL;
    return find_with_parent(tree, path, &parent);
}

/* 새 노드를 동적으로 할당하고 초기화합니다. */
static Node* new_node(char value) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node != NULL) {
        node->data = value;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

/* 트리가 비어있을 때 루트 노드를 생성합니다. */
Tree* insert_root(Tree* tree, char value) {
    Node* node;
    if (tree == NULL) return NULL;
    if (value < 'A' || value > 'Z') return error(tree, "데이터는 A~Z만 가능합니다");
    if (tree->root != NULL) return error(tree, "빈 트리가 아닌 경우 루트를 추가할 수 없습니다");
    if (tree->count >= tree->size) return error(tree, "최대 노드 수에 도달했습니다");

    node = new_node(value);
    if (node == NULL) return error(tree, "메모리 할당 실패");

    tree->root = node;
    tree->count++;
    return tree;
}

/* 단말 노드(자식이 없는 부모 노드)의 지정된 위치(L/R)에 새 자식 노드를 추가합니다. */
Tree* insert_child(Tree* tree, const char* path, char child, char value) {
    Node* parent;
    Node* node;

    if (tree == NULL) return NULL;
    parent = find_node(tree, path);

    if (parent == NULL) return error(tree, "경로가 잘못되었거나 부모 노드가 존재하지 않습니다");
    
    /* 과제 03 요구사항: 부모 노드가 단말 노드(자식이 0개)인 경우에만 추가 가능 */
    if (parent->left != NULL || parent->right != NULL) {
        return error(tree, "부모 노드가 단말 노드가 아닙니다");
    }

    if (child != 'L' && child != 'R') return error(tree, "자식 위치는 L 또는 R입니다");
    if (value < 'A' || value > 'Z') return error(tree, "데이터는 A~Z만 가능합니다");
    if (tree->count >= tree->size) return error(tree, "최대 노드 수에 도달했습니다");

    node = new_node(value);
    if (node == NULL) return error(tree, "메모리 할당 실패");

    if (child == 'L') parent->left = node;
    else parent->right = node;

    tree->count++;
    return tree;
}

/* 단말 노드를 삭제하고 메모리를 해제합니다. */
Tree* delete_node(Tree* tree, const char* path) {
    Node* parent = NULL;
    Node* node;

    if (tree == NULL) return NULL;
    node = find_with_parent(tree, path, &parent);

    if (node == NULL) return error(tree, "경로가 잘못되었거나 노드가 없습니다");
    if (node->left != NULL || node->right != NULL)
        return error(tree, "단말 노드만 삭제할 수 있습니다");

    /* 부모 노드와의 연결을 해제 */
    if (parent == NULL) tree->root = NULL;
    else if (parent->left == node) parent->left = NULL;
    else parent->right = NULL;

    free(node);
    tree->count--;
    return tree;
}

/* 대상 노드의 데이터를 변경합니다 (형제 노드와 중복 데이터 금지). */
Tree* update_value(Tree* tree, const char* path, char value) {
    Node* parent = NULL;
    Node* node;
    Node* sibling = NULL;

    if (tree == NULL) return NULL;
    node = find_with_parent(tree, path, &parent);

    if (node == NULL) return error(tree, "경로가 잘못되었거나 노드가 없습니다");
    if (value < 'A' || value > 'Z') return error(tree, "데이터는 A~Z만 가능합니다");

    /* 형제 노드 확인 */
    if (parent != NULL) {
        if (parent->left == node) sibling = parent->right;
        else sibling = parent->left;
    }

    /* 동일한 부모 아래 두 자식이 같은 데이터를 가지지 못하도록 검사 */
    if (sibling != NULL && sibling->data == value)
        return error(tree, "동일한 부모 아래에 같은 데이터를 가진 두 자식이 생깁니다");

    node->data = value;
    return tree;
}

/* 지정한 노드의 자식 정보를 문자열 버퍼에 저장합니다. */
int read_child(Tree* tree, const char* path, char* outBuf, int bufSize) {
    Node* node;
    if (tree == NULL || outBuf == NULL || bufSize <= 0) return 0;

    node = find_node(tree, path);
    if (node == NULL) return 0;

    if (node->left != NULL && node->right != NULL)
        snprintf(outBuf, bufSize, "%c(L), %c(R)", node->left->data, node->right->data);
    else if (node->left != NULL)
        snprintf(outBuf, bufSize, "%c(L)", node->left->data);
    else if (node->right != NULL)
        snprintf(outBuf, bufSize, "%c(R)", node->right->data);
    else 
        snprintf(outBuf, bufSize, "자식 노드가 없습니다.");

    return 1;
}

/* 재귀적으로 들여쓰기와 +, - 문자를 사용하여 트리를 출력합니다. */
static void print_helper(Node* node, int depth) {
    int i;
    if (node == NULL) return;
    for (i = 1; i < depth; i++) printf("    ");
    if (depth > 0) printf("+---");
    printf("%c\n", node->data);
    print_helper(node->left, depth + 1);
    print_helper(node->right, depth + 1);
}

/* 전체 이진 트리를 왼쪽으로 눕힌 형태로 출력합니다. */
void print_btree(Tree* tree) {
    if (tree == NULL || tree->root == NULL) puts("트리가 비어 있습니다.");
    else print_helper(tree->root, 0);
}

/* 후위 순회(Post-order) 방식으로 자식부터 메모리를 해제합니다. */
static void destroy_helper(Node* node) {
    if (node == NULL) return;
    destroy_helper(node->left);
    destroy_helper(node->right);
    free(node);
}

/* 트리 구조체 및 내부의 모든 노드 메모리를 해제합니다. */
void destroy_btree(Tree* tree) {
    if (tree == NULL) return;
    destroy_helper(tree->root);
    free(tree);
}
