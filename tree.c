#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

#ifndef LEAF_ONLY
#define LEAF_ONLY 1
#endif

Tree *error(Tree *tree, const char *message)
{
    printf("Error: %s.\n", message);
    return tree;
}

Tree *create_btree(int size)
{
    Tree *tree;
    if (size < 0)
        return error(NULL, "Size must be greater than or equal to 0");
    tree = malloc(sizeof(Tree));
    if (tree == NULL)
        return error(NULL, "Memory allocation failed");
    tree->root = NULL;
    tree->count = 0;
    tree->size = size;
    return tree;
}

/* /A/B/C의 형식을 먼저 검사한 뒤, 문자 순서대로 이동한다.
   parentOut에는 찾은 노드의 부모 주소를 저장한다. */
Node *find_with_parent(Tree *tree, const char *path, Node **parentOut)
{
    Node *current = tree->root;
    size_t i, len = strlen(path);
    *parentOut = NULL;
    if (len < 2 || len % 2 != 0)
        return NULL;
    for (i = 0; i < len; i += 2)
    {
        if (path[i] != '/' || path[i + 1] < 'A' || path[i + 1] > 'Z')
            return NULL;
    }
    if (current == NULL || current->data != path[1])
        return NULL;
    for (i = 3; i < len; i += 2)
    {
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

Node *find_node(Tree *tree, const char *path)
{
    Node *parent;
    return find_with_parent(tree, path, &parent);
}

Node *new_node(char value)
{
    Node *node = malloc(sizeof(Node));
    if (node != NULL)
    {
        node->data = value;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

Tree *insert_root(Tree *tree, char value)
{
    Node *node;
    if (value < 'A' || value > 'Z')
        return error(tree, "Data must be uppercase letters A-Z");
    if (tree->root != NULL)
        return error(tree, "Root already exists");
    if (tree->count >= tree->size)
        return error(tree, "Maximum node count reached");
    node = new_node(value);
    if (node == NULL)
        return error(tree, "Memory allocation failed");
    tree->root = node;
    tree->count++;
    printf("Created root %c.\n", value);
    return tree;
}

/* 메뉴 설명에 따라 부모의 빈 자식 위치에 삽입한다.
   단말 노드에만 허용하면 두 자식을 가진 노드를 만들 수 없다. */
Tree *insert_child(Tree *tree, const char *path, char child, char value)
{
    Node *parent = find_node(tree, path);
    Node *node;
    if (parent == NULL)
        return error(tree, "Invalid path or parent does not exist");
    if (child != 'L' && child != 'R')
        return error(tree, "Child position must be L or R");
    if (value < 'A' || value > 'Z')
        return error(tree, "Data must be uppercase letters A-Z");
#if LEAF_ONLY
    if (parent->left != NULL || parent->right != NULL)
        return error(tree, "Children can only be added to leaf nodes");
#endif
    if (child == 'L')
    {
        if (parent->left != NULL)
            return error(tree, "Left child already exists");
        if (parent->right != NULL && parent->right->data == value)
            return error(tree, "Data cannot be identical to sibling node");
    }
    else
    {
        if (parent->right != NULL)
            return error(tree, "Right child already exists");
        if (parent->left != NULL && parent->left->data == value)
            return error(tree, "Data cannot be identical to sibling node");
    }
    if (tree->count >= tree->size)
        return error(tree, "Maximum node count reached");
    node = new_node(value);
    if (node == NULL)
        return error(tree, "Memory allocation failed");
    if (child == 'L')
        parent->left = node;
    else
        parent->right = node;
    tree->count++;
    printf("Added node %c.\n", value);
    return tree;
}

Tree *delete_node(Tree *tree, const char *path)
{
    Node *parent;
    Node *node = find_with_parent(tree, path, &parent);
    if (node == NULL)
        return error(tree, "Invalid path or node does not exist");
    if (node->left != NULL || node->right != NULL)
        return error(tree, "Only leaf nodes can be deleted");
    /* 부모와의 연결을 끊은 뒤 메모리를 해제한다. */
    if (parent == NULL)
        tree->root = NULL;
    else if (parent->left == node)
        parent->left = NULL;
    else
        parent->right = NULL;
    free(node);
    tree->count--;
    puts("Deleted the node.");
    return tree;
}

Tree *update_value(Tree *tree, const char *path, char value)
{
    Node *parent;
    Node *node = find_with_parent(tree, path, &parent);
    Node *sibling = NULL;
    if (node == NULL)
        return error(tree, "Invalid path or node does not exist");
    if (value < 'A' || value > 'Z')
        return error(tree, "Data must be uppercase letters A-Z");
    if (parent != NULL)
    {
        if (parent->left == node)
            sibling = parent->right;
        else
            sibling = parent->left;
    }
    if (sibling != NULL && sibling->data == value)
        return error(tree, "Data cannot be identical to sibling node");
    node->data = value;
    printf("Updated data to %c.\n", value);
    return tree;
}

int read_child(Tree *tree, const char *path, char *outBuf, int bufSize)
{
    Node *node = find_node(tree, path);
    if (node == NULL || outBuf == NULL || bufSize <= 0)
        return 0;
    if (node->left != NULL && node->right != NULL)
        snprintf(outBuf, bufSize, "%c(L), %c(R)", node->left->data, node->right->data);
    else if (node->left != NULL)
        snprintf(outBuf, bufSize, "%c(L)", node->left->data);
    else if (node->right != NULL)
        snprintf(outBuf, bufSize, "%c(R)", node->right->data);
    else
        snprintf(outBuf, bufSize, "No child nodes.");
    return 1;
}

/* 루트, 왼쪽, 오른쪽 순서로 출력하며 깊이에 따라 들여쓴다. */
void print_helper(Node *node, int depth)
{
    int i;
    if (node == NULL)
        return;
    for (i = 1; i < depth; i++)
        printf("    ");
    if (depth > 0)
        printf("+---");
    printf("%c\n", node->data);
    print_helper(node->left, depth + 1);
    print_helper(node->right, depth + 1);
}

void print_btree(Tree *tree)
{
    if (tree->root == NULL)
        puts("Tree is empty.");
    else
        print_helper(tree->root, 0);
}

/* 자식을 먼저 해제하고 마지막에 부모를 해제한다. */
void destroy_helper(Node *node)
{
    if (node == NULL)
        return;
    destroy_helper(node->left);
    destroy_helper(node->right);
    free(node);
}

void destroy_btree(Tree *tree)
{
    if (tree == NULL)
        return;
    destroy_helper(tree->root);
    free(tree);
}
