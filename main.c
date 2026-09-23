#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tree.h"

#define MAX_NODES 1000
#define LINE_SIZE (2 * MAX_NODES + 64)

/* 대소문자를 구분하지 않고 전체 이름 또는 한 글자 약어를 비교한다. */
int match_cmd(const char *token, const char *full, char abbrev)
{
    int i;
    if (strlen(token) == 1)
        return toupper((unsigned char)token[0]) == abbrev;
    for (i = 0; token[i] != '\0' && full[i] != '\0'; i++)
    {
        if (toupper((unsigned char)token[i]) != toupper((unsigned char)full[i]))
            return 0;
    }
    return token[i] == '\0' && full[i] == '\0';
}

int valid_data(const char *text)
{
    return strlen(text) == 1 && text[0] >= 'A' && text[0] <= 'Z';
}

void do_insert(Tree *tree, char *args[], int argc)
{
    if (argc == 3 && strcmp(args[1], "/") == 0 && valid_data(args[2]))
    {
        tree = insert_root(tree, args[2][0]);
    }
    else if (argc == 4 && valid_data(args[3]))
    {
        char child;
        if (match_cmd(args[2], "Left", 'L'))
            child = 'L';
        else if (match_cmd(args[2], "Right", 'R'))
            child = 'R';
        else
        {
            puts("Error: Enter L/Left or R/Right for the position.");
            return;
        }
        tree = insert_child(tree, args[1], child, args[3][0]);
    }
    else
        puts("Error: Enter in Insert / A or Insert /A L B format.");
}

void do_delete(Tree *tree, char *args[], int argc)
{
    if (argc == 2)
        tree = delete_node(tree, args[1]);
    else
        puts("Error: Enter in Delete path format.");
}

void do_update(Tree *tree, char *args[], int argc)
{
    if (argc == 3 && valid_data(args[2]))
        tree = update_value(tree, args[1], args[2][0]);
    else
        puts("Error: Enter in Update path uppercase format.");
}

void do_read(Tree *tree, char *args[], int argc)
{
    char buf[64];
    if (argc != 2)
        puts("Error: Enter in Read path format.");
    else if (read_child(tree, args[1], buf, sizeof(buf)))
        puts(buf);
    else
        puts("Error: Invalid path or node does not exist.");
}

/* 한 줄이 너무 길면 나머지 입력까지 버리고 다시 입력받는다. */
int read_line(char line[])
{
    int ch;
    while (fgets(line, LINE_SIZE, stdin) != NULL)
    {
        if (strchr(line, '\n') == NULL)
        {
            ch = getchar();
            if (ch != '\n' && ch != EOF)
            {
                while ((ch = getchar()) != '\n' && ch != EOF)
                {
                }
                puts("Error: Input is too long.");
                continue;
            }
        }
        return 1;
    }
    return 0;
}

/* 명령을 나누어 실행한다. 종료 명령이면 0을 반환한다. */
int process_command(Tree *tree, char line[])
{
    char *args[5];
    int argc = 0;
    char *token = strtok(line, " \t\r\n\v\f");
    while (token != NULL && argc < 5)
    {
        /* 나눈 단어를 복사하지 않고 시작 주소를 저장한다. */
        args[argc++] = token;
        token = strtok(NULL, " \t\r\n\v\f");
    }
    if (argc == 0)
        return 1;
    if (match_cmd(args[0], "Quit", 'Q') || match_cmd(args[0], "Exit", 'X'))
    {
        if (argc == 1)
            return 0;
        puts("Error: Quit command does not require arguments.");
    }
    else if (match_cmd(args[0], "Insert", 'I'))
        do_insert(tree, args, argc);
    else if (match_cmd(args[0], "Delete", 'D'))
        do_delete(tree, args, argc);
    else if (match_cmd(args[0], "Update", 'U'))
        do_update(tree, args, argc);
    else if (match_cmd(args[0], "Read", 'R'))
        do_read(tree, args, argc);
    else if (match_cmd(args[0], "Print", 'P'))
    {
        if (argc == 1)
            print_btree(tree);
        else
            puts("Error: Print command does not require arguments.");
    }
    else
        puts("Error: Unknown command.");
    return 1;
}

int main(void)
{
    Tree *tree = create_btree(MAX_NODES);
    char line[LINE_SIZE];
    if (tree == NULL)
        return 1;
    puts("Commands: Insert(I), Delete(D), Update(U), Read(R), Print(P), Quit(Q)");
    puts("Enter one command per line. Example: I / A, I /A L B");
    while (read_line(line))
    {
        if (process_command(tree, line) == 0)
            break;
    }
    destroy_btree(tree);
    return 0;
}
