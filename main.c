#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tree.h"

/* 컴파일 방법: gcc -std=c11 -Wall -Wextra main.c tree.c -o tree */
#define MAX_NODES 1000
#define LINE_SIZE (2 * MAX_NODES + 64)

/* 대소문자를 구분하지 않고 전체 명령어 이름 또는 약어(한 글자)를 비교합니다. */
int match_cmd(const char* token, const char* full, char abbrev) {
    int i;
    if (strlen(token) == 1)
        return toupper((unsigned char)token[0]) == abbrev;
    for (i = 0; token[i] != '\0' && full[i] != '\0'; i++) {
        if (toupper((unsigned char)token[i]) != toupper((unsigned char)full[i]))
            return 0;
    }
    return token[i] == '\0' && full[i] == '\0';
}

/* 입력된 데이터가 알파벳 대문자 한 글자인지 검사합니다. */
int valid_data(const char* text) {
    return strlen(text) == 1 && text[0] >= 'A' && text[0] <= 'Z';
}

/* Insert 명령을 처리합니다. */
void do_insert(Tree** treePtr, char args[][LINE_SIZE], int argc) {
    if (argc == 3 && strcmp(args[1], "/") == 0 && valid_data(args[2])) {
        *treePtr = insert_root(*treePtr, args[2][0]);
    } else if (argc == 4 && valid_data(args[3])) {
        char child;
        if (match_cmd(args[2], "Left", 'L')) child = 'L';
        else if (match_cmd(args[2], "Right", 'R')) child = 'R';
        else {
            puts("오류: 위치는 L/Left 또는 R/Right를 입력하세요.");
            return;
        }
        *treePtr = insert_child(*treePtr, args[1], child, args[3][0]);
    } else puts("오류: Insert / A 또는 Insert /A L B 형식으로 입력하세요.");
}

/* Delete 명령을 처리합니다. */
void do_delete(Tree** treePtr, char args[][LINE_SIZE], int argc) {
    if (argc == 2) *treePtr = delete_node(*treePtr, args[1]);
    else puts("오류: Delete 경로 형식으로 입력하세요.");
}

/* Update 명령을 처리합니다. */
void do_update(Tree** treePtr, char args[][LINE_SIZE], int argc) {
    if (argc == 3 && valid_data(args[2])) *treePtr = update_value(*treePtr, args[1], args[2][0]);
    else puts("오류: Update 경로 대문자 형식으로 입력하세요.");
}

/* Read 명령을 처리합니다. */
void do_read(Tree** treePtr, char args[][LINE_SIZE], int argc) {
    char buf[64];
    if (argc != 2) puts("오류: Read 경로 형식으로 입력하세요.");
    else if (read_child(*treePtr, args[1], buf, sizeof(buf))) puts(buf);
    else puts("오류: 경로가 잘못되었거나 노드가 없습니다.");
}

/* 표준 입력에서 한 줄을 읽어오며, 버퍼 크기를 초과하는 긴 입력은 버립니다. */
int read_line(char line[]) {
    if (fgets(line, LINE_SIZE, stdin) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        } else {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            puts("오류: 입력이 너무 깁니다.");
            return -1; 
        }
        return 1;
    }
    return 0;
}

/* 입력된 한 줄의 명령어를 분석하고 적절한 트리 연산 함수를 호출합니다. */
int process_command(Tree** treePtr, char line[]) {
    char args[5][LINE_SIZE];
    int argc = 0;
    char* token = strtok(line, " \t\r\n\v\f");
    while (token != NULL && argc < 5) {
        strcpy(args[argc++], token);
        token = strtok(NULL, " \t\r\n\v\f");
    }
    if (argc == 0) return 1;

    /* 종료 명령 (Quit 또는 Exit) */
    if (match_cmd(args[0], "Quit", 'Q') || match_cmd(args[0], "Exit", 'X')) {
        if (argc == 1) return 0;
        puts("오류: 종료 명령에는 인자가 필요하지 않습니다.");
    } else if (match_cmd(args[0], "Insert", 'I')) do_insert(treePtr, args, argc);
    else if (match_cmd(args[0], "Delete", 'D')) do_delete(treePtr, args, argc);
    else if (match_cmd(args[0], "Update", 'U')) do_update(treePtr, args, argc);
    else if (match_cmd(args[0], "Read", 'R')) do_read(treePtr, args, argc);
    else if (match_cmd(args[0], "Print", 'P')) {
        if (argc == 1) print_btree(*treePtr);
        else puts("오류: Print 명령에는 인자가 필요하지 않습니다.");
    } else puts("오류: 알 수 없는 명령어입니다.");
    
    return 1;
}

int main(void) {
    Tree* tree = create_btree(MAX_NODES);
    char line[LINE_SIZE];
    if (tree == NULL) return 1;

    /* 메인 명령 루프 실행 */
    while (1) {
        int res = read_line(line);
        if (res == 0) break;       /* EOF 도달 시 종료 */
        if (res == -1) continue;   /* 입력 초과 오류 시 다음 입력 대기 */
        if (process_command(&tree, line) == 0) break; /* Quit/Exit 입력 시 종료 */
    }

    /* 할당된 트리 메모리 해제 */
    destroy_btree(tree);
    return 0;
}
