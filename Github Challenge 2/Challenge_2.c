// Challenge_2.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "Lib.h"

//-------------------- 路径搜索与工具函数 --------------------

// 交叉点名转 maze 坐标
void crossingNameToCoord(const char* name, int* i, int* j) {
    if (name[0] == 'c' && isdigit(name[1]) && isdigit(name[2])) {
        int row = name[1] - '0';
        int col = name[2] - '0';
        *i = 2 + row * 2;
        *j = 2 + col * 2;
    } else {
        *i = -1;
        *j = -1;
    }
}

// 去除连续重复坐标
void removeDuplicates(const char* coords[], int* count) {
    if (*count <= 1) return;
    int j = 0;
    for (int i = 1; i < *count; i++) {
        if (strcmp(coords[i], coords[j])) {
            coords[++j] = coords[i];
        }
    }
    *count = j + 1;
}

// 路径字符串的长度（交叉点数）
int GetRouteLength(char* route) {
    int count = 0;
    for (int i = 0; route[i]; i++) {
        if (route[i] == '-') count++;
    }
    return count;
}

//-------------------- 路径搜索主函数 --------------------

// 最短路径搜索，返回交叉点名字符串
char* Fast_Route(int target_station, int start_station) {
    int maze[13][13] = {
        {-1, -1, -1, -1, 0, -1, 0, -1, 0, -1, -1, -1, -1},
        {-1, -1, -1, -1, 0, -1, 0, -1, 0, -1, -1, -1, -1},
        {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1},
        {-1, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, -1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {-1, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, -1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {-1, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, -1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {-1, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, -1},
        {-1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1},
        {-1, -1, -1, -1, 0, -1, 0, -1, 0, -1, -1, -1, -1},
        {-1, -1, -1, -1, 0, -1, 0, -1, 0, -1, -1, -1, -1}
    };

    int v = 1;
    int start_i, start_j, target_i, target_j;

    readStation(&target_i, &target_j, start_station);
    maze[target_i][target_j] = v;

    readStation(&start_i, &start_j, target_station);
    maze[start_i][start_j] = 0;

    // BFS
    while (maze[start_i][start_j] == 0) {
        for (int i = 0; i < 13; i++) {
            for (int j = 0; j < 13; j++) {
                if (maze[i][j] == v) {
                    if (i + 1 < 13 && maze[i + 1][j] == 0) maze[i + 1][j] = v + 1;
                    if (i - 1 >= 0 && maze[i - 1][j] == 0) maze[i - 1][j] = v + 1;
                    if (j + 1 < 13 && maze[i][j + 1] == 0) maze[i][j + 1] = v + 1;
                    if (j - 1 >= 0 && maze[i][j - 1] == 0) maze[i][j - 1] = v + 1;
                }
            }
        }
        v++;
    }

    // 回溯路径
    int current_i = start_i, current_j = start_j;
    char* route = NULL;
    size_t route_length = 0;

    while (!(current_i == target_i && current_j == target_j)) {
        int current_value = maze[current_i][current_j];
        int next_i = current_i, next_j = current_j, min_value = current_value;

        if (current_i + 1 < 13 && maze[current_i + 1][current_j] > 0 && maze[current_i + 1][current_j] < min_value) {
            next_i = current_i + 1; next_j = current_j; min_value = maze[next_i][next_j];
        }
        if (current_i - 1 >= 0 && maze[current_i - 1][current_j] > 0 && maze[current_i - 1][current_j] < min_value) {
            next_i = current_i - 1; next_j = current_j; min_value = maze[next_i][next_j];
        }
        if (current_j + 1 < 13 && maze[current_i][current_j + 1] > 0 && maze[current_i][current_j + 1] < min_value) {
            next_i = current_i; next_j = current_j + 1; min_value = maze[next_i][next_j];
        }
        if (current_j - 1 >= 0 && maze[current_i][current_j - 1] > 0 && maze[current_i][current_j - 1] < min_value) {
            next_i = current_i; next_j = current_j - 1; min_value = maze[next_i][next_j];
        }

        if (!(next_i == target_i && next_j == target_j)) {
            char* crossingName = getCrossingName(next_i, next_j);
            if (crossingName != NULL) {
                size_t name_length = strlen(crossingName);
                char* temp = (char*)realloc(route, route_length + name_length + 1);
                if (temp == NULL) {
                    perror("Failed to realloc memory for route");
                    free(route);
                    return NULL;
                }
                route = temp;
                strcpy(route + route_length, crossingName);
                route_length += name_length;
                free(crossingName);
            }
        }
        current_i = next_i;
        current_j = next_j;
    }
    return route;
}

// Crossing 到 Station 路径
char* Crossing_to_Station(const char* crossing_name, int station) {
    int start_i, start_j, target_i, target_j;
    int maze[13][13] = {0};

    crossingNameToCoord(crossing_name, &start_i, &start_j);
    readStation(&target_i, &target_j, station);

    maze[target_i][target_j] = 1;
    int v = 1;
    maze[start_i][start_j] = 0;
    while (maze[start_i][start_j] == 0) {
        for (int i = 0; i < 13; i++) {
            for (int j = 0; j < 13; j++) {
                if (maze[i][j] == v) {
                    if (i + 1 < 13 && maze[i + 1][j] == 0) maze[i + 1][j] = v + 1;
                    if (i - 1 >= 0 && maze[i - 1][j] == 0) maze[i - 1][j] = v + 1;
                    if (j + 1 < 13 && maze[i][j + 1] == 0) maze[i][j + 1] = v + 1;
                    if (j - 1 >= 0 && maze[i][j - 1] == 0) maze[i][j - 1] = v + 1;
                }
            }
        }
        v++;
    }

    int current_i = start_i, current_j = start_j;
    char* route = NULL;
    size_t route_length = 0;

    while (!(current_i == target_i && current_j == target_j)) {
        int current_value = maze[current_i][current_j];
        int next_i = current_i, next_j = current_j, min_value = current_value;

        if (current_i + 1 < 13 && maze[current_i + 1][current_j] > 0 && maze[current_i + 1][current_j] < min_value) {
            next_i = current_i + 1; next_j = current_j; min_value = maze[next_i][next_j];
        }
        if (current_i - 1 >= 0 && maze[current_i - 1][current_j] > 0 && maze[current_i - 1][current_j] < min_value) {
            next_i = current_i - 1; next_j = current_j; min_value = maze[next_i][next_j];
        }
        if (current_j + 1 < 13 && maze[current_i][current_j + 1] > 0 && maze[current_i][current_j + 1] < min_value) {
            next_i = current_i; next_j = current_j + 1; min_value = maze[next_i][next_j];
        }
        if (current_j - 1 >= 0 && maze[current_i][current_j - 1] > 0 && maze[current_i][current_j - 1] < min_value) {
            next_i = current_i; next_j = current_j - 1; min_value = maze[next_i][next_j];
        }

        if (!(next_i == target_i && next_j == target_j)) {
            char* crossingName = getCrossingName(next_i, next_j);
            if (crossingName != NULL) {
                size_t name_length = strlen(crossingName);
                if (crossingName[name_length - 1] == ' ') crossingName[name_length - 1] = '\0';
                char* temp = (char*)realloc(route, route_length + name_length + 2);
                if (temp == NULL) {
                    perror("Failed to realloc memory for route");
                    free(route);
                    free(crossingName);
                    return NULL;
                }
                route = temp;
                if (route_length > 0) {
                    route[route_length++] = ' ';
                }
                strcpy(route + route_length, crossingName);
                route_length += strlen(crossingName);
                free(crossingName);
            }
        }
        current_i = next_i;
        current_j = next_j;
    }
    return route;
}

//-------------------- 路径优化与排列 --------------------

// 站点排列最短路径
int* Challenge_2(int Start, int Passing[]) {
    int A = Start, B = Passing[0], C = Passing[1], D = Passing[2];
    printf("-----------------------------------------------------------\n");
    printf("All Stations:\n");
    printf("Start     : %d\n", A);
    printf("Passing 1 : %d\n", B);
    printf("Passing 2 : %d\n", C);
    printf("Passing 3 : %d\n", D);

    int permutations[6][3] = {
        {B, C, D}, {B, D, C}, {C, B, D},
        {C, D, B}, {D, B, C}, {D, C, B}
    };

    int* shortest_order = (int*)malloc(4 * sizeof(int));
    if (!shortest_order) {
        perror("Failed to allocate memory for shortest_order");
        return NULL;
    }
    shortest_order[0] = A;
    shortest_order[1] = shortest_order[2] = shortest_order[3] = 0;

    int shortest_length = INT_MAX;

    for (int i = 0; i < 6; i++) {
        int x = permutations[i][0], y = permutations[i][1], z = permutations[i][2];
        char* r1 = Fast_Route(A, x);
        char* r2 = Fast_Route(x, y);
        char* r3 = Fast_Route(y, z);
        int len1 = GetRouteLength(r1);
        int len2 = GetRouteLength(r2);
        int len3 = GetRouteLength(r3);
        int total_length = len1 + len2 + len3;
        if (total_length < shortest_length) {
            shortest_length = total_length;
            shortest_order[1] = x;
            shortest_order[2] = y;
            shortest_order[3] = z;
        }
        free(r1); free(r2); free(r3);
    }
    printf("Shortest total route length: %d\n", shortest_length);
    return shortest_order;
}