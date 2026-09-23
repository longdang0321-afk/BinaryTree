#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tree.h"

/* 입력한 토큰이 이 명령어와 일치하는지 확인: 약자 한 글자이거나, 전체 이름과 같으면 true */
int match_cmd(char* token, char* full, char abbrev) {
    if (strlen(token) == 1) {
        return toupper((unsigned char)token[0]) == toupper((unsigned char)abbrev);
    }
    return strcasecmp(token, full) == 0;
}

void do_insert(Tree* tree, char args[][256], int argc) {
    if (argc == 3 && strcmp(args[1], "/") == 0) {
        /* Insert / A -> 루트 노드 생성 */
        if (strlen(args[2]) != 1 || !isupper((unsigned char)args[2][0])) {
            printf("오류: 데이터가 올바르지 않습니다.\n");
            return;
        }
        if (insert_root(tree, args[2][0]) == 0) {
            printf("오류: 이미 루트 노드가 존재합니다.\n");
        } else {
            printf("완료: 루트 노드 %c 생성.\n", args[2][0]);
        }
        return;
    }

    if (argc == 4) {
        char child = toupper((unsigned char)args[2][0]);
        char value = args[3][0];
        if (strlen(args[3]) != 1 || !isupper((unsigned char)value)) {
            printf("오류: 데이터가 올바르지 않습니다.\n");
            return;
        }
        int result = insert_child(tree, args[1], child, value);
        if (result == 1) printf("완료: %c 노드 추가.\n", value);
        else if (result == -1) printf("오류: 부모 노드를 찾을 수 없습니다.\n");
        else if (result == -2) printf("오류: 부모 노드에 이미 자식이 2개 있습니다.\n");
        else if (result == -3) printf("오류: 해당 위치에 이미 자식 노드가 있습니다.\n");
        else if (result == -4) printf("오류: 형제 노드와 데이터가 중복됩니다.\n");
        else if (result == -5) printf("오류: child는 L 또는 R이어야 합니다.\n");
        return;
    }

    printf("오류: Insert 명령의 인자 개수가 올바르지 않습니다.\n");
}

void do_delete(Tree* tree, char args[][256], int argc) {
    if (argc != 2) {
        printf("오류: Delete 명령의 인자 개수가 올바르지 않습니다.\n");
        return;
    }
    int result = delete_node(tree, args[1]);
    if (result == 1) printf("완료: 삭제되었습니다.\n");
    else if (result == -1) printf("오류: 노드를 찾을 수 없습니다.\n");
    else if (result == -2) printf("오류: 단말 노드가 아닙니다.\n");
}

void do_update(Tree* tree, char args[][256], int argc) {
    if (argc != 3) {
        printf("오류: Update 명령의 인자 개수가 올바르지 않습니다.\n");
        return;
    }
    char value = args[2][0];
    if (strlen(args[2]) != 1 || !isupper((unsigned char)value)) {
        printf("오류: 데이터가 올바르지 않습니다.\n");
        return;
    }
    int result = update_value(tree, args[1], value);
    if (result == 1) printf("완료: %c(으)로 수정되었습니다.\n", value);
    else if (result == -1) printf("오류: 노드를 찾을 수 없습니다.\n");
    else if (result == -2) printf("오류: 형제 노드와 데이터가 중복됩니다.\n");
}

void do_read(Tree* tree, char args[][256], int argc) {
    if (argc != 2) {
        printf("오류: Read 명령의 인자 개수가 올바르지 않습니다.\n");
        return;
    }
    char buf[64];
    if (read_child(tree, args[1], buf, sizeof(buf))) {
        printf("%s\n", buf);
    } else {
        printf("오류: 노드를 찾을 수 없습니다.\n");
    }
}

int main(void) {
    Tree* tree = create_btree(1000);
    char line[256];

    while (fgets(line, sizeof(line), stdin) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        /* 공백/탭 기준으로 토큰 분리 */
        char args[6][256];
        int argc = 0;
        char* tok = strtok(line, " \t");
        while (tok != NULL && argc < 6) {
            strcpy(args[argc], tok);
            argc++;
            tok = strtok(NULL, " \t");
        }

        if (argc == 0) continue; /* 빈 줄은 무시 */

        char* cmd = args[0];

        if (match_cmd(cmd, "Quit", 'Q') || match_cmd(cmd, "Exit", 'X')) {
            break;
        } else if (match_cmd(cmd, "Insert", 'I')) {
            do_insert(tree, args, argc);
        } else if (match_cmd(cmd, "Delete", 'D')) {
            do_delete(tree, args, argc);
        } else if (match_cmd(cmd, "Update", 'U')) {
            do_update(tree, args, argc);
        } else if (match_cmd(cmd, "Read", 'R')) {
            do_read(tree, args, argc);
        } else if (match_cmd(cmd, "Print", 'P')) {
            if (argc != 1) printf("오류: Print 명령은 인자를 받지 않습니다.\n");
            else print_btree(tree);
        } else {
            printf("오류: 알 수 없는 명령어입니다.\n");
        }
    }

    destroy_btree(tree);
    return 0;
}
