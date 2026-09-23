#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tree.h"

Tree* create_btree(int size) {
    Tree* t = (Tree*)malloc(sizeof(Tree));
    t->root = NULL;
    t->count = 0;
    return t;
}

/* path "/A/B/C" 를 문자 배열 tokens = {'A','B','C'} 로 분리한다. 형식이 잘못되면 -1 반환 */
static int parse_path(char* path, char tokens[], int maxTokens) {
    char buf[256];
    strcpy(buf, path);

    int count = 0;
    char* tok = strtok(buf, "/");
    while (tok != NULL && count < maxTokens) {
        if (strlen(tok) != 1 || !isalpha((unsigned char)tok[0])) return -1;
        tokens[count] = tok[0];
        count++;
        tok = strtok(NULL, "/");
    }
    return count;
}

Node* find_node(Tree* tree, char* path) {
    char tokens[64];
    int count = parse_path(path, tokens, 64);
    if (count <= 0) return NULL;
    if (tree->root == NULL) return NULL;
    if (tree->root->data != tokens[0]) return NULL;

    Node* cur = tree->root;
    int i;
    for (i = 1; i < count; i++) {
        if (cur->left != NULL && cur->left->data == tokens[i]) {
            cur = cur->left;
        } else if (cur->right != NULL && cur->right->data == tokens[i]) {
            cur = cur->right;
        } else {
            return NULL;
        }
    }
    return cur;
}

/*
 * find_node와 비슷하지만, 해당 노드의 parent와 side(L/R)도 같이 찾아준다.
 * Delete와 Update는 그 노드가 누구의 어느 쪽 자식인지 알아야 하기 때문에 필요함.
 */
static Node* find_node_with_parent(Tree* tree, char* path, Node** parentOut, char* sideOut) {
    char tokens[64];
    int count = parse_path(path, tokens, 64);
    *parentOut = NULL;
    *sideOut = '\0';

    if (count <= 0) return NULL;
    if (tree->root == NULL) return NULL;
    if (tree->root->data != tokens[0]) return NULL;

    Node* cur = tree->root;
    Node* parent = NULL;
    char side = '\0';
    int i;
    for (i = 1; i < count; i++) {
        if (cur->left != NULL && cur->left->data == tokens[i]) {
            parent = cur; side = 'L'; cur = cur->left;
        } else if (cur->right != NULL && cur->right->data == tokens[i]) {
            parent = cur; side = 'R'; cur = cur->right;
        } else {
            return NULL;
        }
    }
    *parentOut = parent;
    *sideOut = side;
    return cur;
}

int insert_root(Tree* tree, char value) {
    if (tree->root != NULL) return 0;
    Node* n = (Node*)malloc(sizeof(Node));
    n->data = value;
    n->left = NULL;
    n->right = NULL;
    tree->root = n;
    tree->count++;
    return 1;
}

int insert_child(Tree* tree, char* parentPath, char child, char value) {
    Node* parent = find_node(tree, parentPath);
    if (parent == NULL) return -1;
    if (child != 'L' && child != 'R') return -5;
    if (parent->left != NULL && parent->right != NULL) return -2;

    if (child == 'L') {
        if (parent->left != NULL) return -3;
        if (parent->right != NULL && parent->right->data == value) return -4;
    } else {
        if (parent->right != NULL) return -3;
        if (parent->left != NULL && parent->left->data == value) return -4;
    }

    Node* n = (Node*)malloc(sizeof(Node));
    n->data = value;
    n->left = NULL;
    n->right = NULL;
    if (child == 'L') parent->left = n;
    else parent->right = n;
    tree->count++;
    return 1;
}

int delete_node(Tree* tree, char* path) {
    Node* parent;
    char side;
    Node* n = find_node_with_parent(tree, path, &parent, &side);
    if (n == NULL) return -1;
    if (n->left != NULL || n->right != NULL) return -2;

    if (parent == NULL) {
        tree->root = NULL; /* root를 삭제한 경우 */
    } else if (side == 'L') {
        parent->left = NULL;
    } else {
        parent->right = NULL;
    }
    free(n);
    tree->count--;
    return 1;
}

int update_value(Tree* tree, char* path, char value) {
    Node* parent;
    char side;
    Node* n = find_node_with_parent(tree, path, &parent, &side);
    if (n == NULL) return -1;

    if (parent != NULL) {
        Node* sibling = (side == 'L') ? parent->right : parent->left;
        if (sibling != NULL && sibling->data == value) return -2;
    }
    n->data = value;
    return 1;
}

int read_child(Tree* tree, char* path, char* outBuf, int bufSize) {
    Node* n = find_node(tree, path);
    if (n == NULL) return 0;

    if (n->left == NULL && n->right == NULL) {
        snprintf(outBuf, bufSize, "자식 노드가 없습니다.");
        return 1;
    }

    char tmp[64] = "";
    if (n->left != NULL) {
        char part[16];
        sprintf(part, "%c(L)", n->left->data);
        strcat(tmp, part);
    }
    if (n->right != NULL) {
        if (tmp[0] != '\0') strcat(tmp, ", ");
        char part[16];
        sprintf(part, "%c(R)", n->right->data);
        strcat(tmp, part);
    }
    strcpy(outBuf, tmp);
    return 1;
}

/* 트리를 왼쪽으로 눕힌 형태로 출력 (root -> 왼쪽 자식 -> 오른쪽 자식 순, 재귀) */
static void print_helper(Node* node, int depth) {
    if (node == NULL) return;

    if (depth == 0) {
        printf("%c\n", node->data);
    } else {
        int i;
        for (i = 0; i < depth - 1; i++) printf("    ");
        printf("+---%c\n", node->data);
    }
    print_helper(node->left, depth + 1);
    print_helper(node->right, depth + 1);
}

void print_btree(Tree* tree) {
    if (tree->root == NULL) {
        printf("트리가 비어 있습니다.\n");
        return;
    }
    print_helper(tree->root, 0);
}

static void destroy_helper(Node* node) {
    if (node == NULL) return;
    destroy_helper(node->left);
    destroy_helper(node->right);
    free(node);
}

void destroy_btree(Tree* tree) {
    destroy_helper(tree->root);
    free(tree);
}
