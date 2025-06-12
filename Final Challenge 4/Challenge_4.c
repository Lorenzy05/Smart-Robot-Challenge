// Challenge_2.c

#include <stdio.h>
#include <stdlib.h>
#include "Lib.h"
#include <string.h>
#include <limits.h>

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

// crossing_name 转 maze 坐标（只允许 c00-c44）
void crossingNameToCoord(const char* name, int* i, int* j) {
    if (name[0] == 'c' && isdigit(name[1]) && isdigit(name[2])) {
        int row = name[1] - '0';
        int col = name[2] - '0';
        if (row >= 0 && row <= 4 && col >= 0 && col <= 4) {
            *i = 2 + row * 2;
            *j = 2 + col * 2;
            return;
        }
    }
    *i = -1;
    *j = -1;
}

// Crossing 到 Station 路径（只记录合法 crossing）
char* Crossing_to_Station(const char* crossing_name, int station, int maze[13][13]) {
    int start_i, start_j;
    int target_i, target_j;


    crossingNameToCoord(crossing_name, &start_i, &start_j);
    if (start_i == -1 || start_j == -1) return NULL;

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

        // 只记录合法 crossing
        char* crossingName = NULL;
        if (!(next_i == target_i && next_j == target_j)) {
            int row = (next_i - 2) / 2;
            int col = (next_j - 2) / 2;
            if ((next_i - 2) % 2 == 0 && (next_j - 2) % 2 == 0 && row >= 0 && row <= 4 && col >= 0 && col <= 4) {
                crossingName = (char*)malloc(5);
                sprintf(crossingName, "c%d%d", row, col);
            }
        }
        if (crossingName) {
            char* temp = (char*)realloc(route, route_length + strlen(crossingName) + 2);
            if (!temp) {
                free(route);
                free(crossingName);
                return NULL;
            }
            route = temp;
            if (route_length > 0) route[route_length++] = ' ';
            strcpy(route + route_length, crossingName);
            route_length += strlen(crossingName);
            free(crossingName);
        }
        current_i = next_i;
        current_j = next_j;
    }

    if (route) route[route_length] = '\0';
    return route;
}

char* Fast_Route(int target_station, int start_station, int maze[13][13]) {


    int n, i, j;
    int v = 1;
    int start_i, start_j, target_i, target_j;

    readStation(&target_i, &target_j, start_station);
    maze[target_i][target_j] = v;

    readStation(&start_i, &start_j, target_station);
    maze[start_i][start_j] = 0;

    while (maze[start_i][start_j] == 0) {
        for (i = 0; i < 13; i++) {
            for (j = 0; j < 13; j++) {
                if (maze[i][j] == v) {
                    if (i + 1 < 13 && maze[i + 1][j] == 0) {
                        maze[i + 1][j] = v + 1;
                    }
                    if (i - 1 >= 0 && maze[i - 1][j] == 0) {
                        maze[i - 1][j] = v + 1;
                    }
                    if (j + 1 < 13 && maze[i][j + 1] == 0) {
                        maze[i][j + 1] = v + 1;
                    }
                    if (j - 1 >= 0 && maze[i][j - 1] == 0) {
                        maze[i][j - 1] = v + 1;
                    }
                }
            }
        }
        v++;
    }

    int current_i = start_i;
    int current_j = start_j;
    char* route = NULL;       // To store the route string
    size_t route_length = 0; // Length of the route string

    while (!(current_i == target_i && current_j == target_j)) {
        int current_value = maze[current_i][current_j];
        int next_i = current_i;
        int next_j = current_j;
        int min_value = current_value;

        if (current_i + 1 < 13 && maze[current_i + 1][current_j] > 0 && maze[current_i + 1][current_j] < min_value) {
            next_i = current_i + 1;
            next_j = current_j;
            min_value = maze[next_i][next_j];
        }
        if (current_i - 1 >= 0 && maze[current_i - 1][current_j] > 0 && maze[current_i - 1][current_j] < min_value) {
            next_i = current_i - 1;
            next_j = current_j;
            min_value = maze[next_i][next_j];
        }
        if (current_j + 1 < 13 && maze[current_i][current_j + 1] > 0 && maze[current_i][current_j + 1] < min_value) {
            next_i = current_i;
            next_j = current_j + 1;
            min_value = maze[next_i][next_j];
        }
        if (current_j - 1 >= 0 && maze[current_i][current_j - 1] > 0 && maze[current_i][current_j - 1] < min_value) {
            next_i = current_i;
            next_j = current_j - 1;
            min_value = maze[next_i][next_j];
        }

        if (!(next_i == target_i && next_j == target_j)) {
            char* crossingName = getCrossingName(next_i, next_j);
            if (crossingName != NULL) {
                // Append crossingName to route
                size_t name_length = strlen(crossingName);
                char* temp = (char*)realloc(route, route_length + name_length + 1);
                if (temp == NULL) {
                    perror("Failed to realloc memory for route");
                    free(route);
                    return NULL; // Indicate error by returning NULL
                }
                route = temp;
                strcpy(route + route_length, crossingName);
                route_length += name_length;
                free(crossingName); // Free the name!
            }
        }
        current_i = next_i;
        current_j = next_j;
    }

    // printf("Route: %s\n", route);
    return route;
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


int GetRouteLength(char* route) {
    int count = 0;
    for (int i = 0; route[i]; i++) {
        if (route[i] == '-') count++;
    }
    return count; // 交叉数就是站点之间的连接数
}


char* Crossing_to_Crossing(const char* start_crossing, const char* end_crossing, int maze[13][13]){
    int start_i, start_j, end_i, end_j;
    crossingNameToCoord(start_crossing, &start_i, &start_j);
    crossingNameToCoord(end_crossing, &end_i, &end_j);
    if (start_i == -1 || start_j == -1 || end_i == -1 || end_j == -1) return NULL;

    int temp_maze[13][13];
    memcpy(temp_maze, maze, sizeof(temp_maze));
    temp_maze[end_i][end_j] = 1;
    int v = 1;
    temp_maze[start_i][start_j] = 0;
    while (temp_maze[start_i][start_j] == 0) {
        for (int i = 0; i < 13; i++) {
            for (int j = 0; j < 13; j++) {
                if (temp_maze[i][j] == v) {
                    if (i + 1 < 13 && temp_maze[i + 1][j] == 0) temp_maze[i + 1][j] = v + 1;
                    if (i - 1 >= 0 && temp_maze[i - 1][j] == 0) temp_maze[i - 1][j] = v + 1;
                    if (j + 1 < 13 && temp_maze[i][j + 1] == 0) temp_maze[i][j + 1] = v + 1;
                    if (j - 1 >= 0 && temp_maze[i][j - 1] == 0) temp_maze[i][j - 1] = v + 1;
                }
            }
        }
        v++;
    }

    int current_i = start_i, current_j = start_j;
    char* route = NULL;
    size_t route_length = 0;

    while (!(current_i == end_i && current_j == end_j)) {
        int current_value = temp_maze[current_i][current_j];
        int next_i = current_i, next_j = current_j, min_value = current_value;

        if (current_i + 1 < 13 && temp_maze[current_i + 1][current_j] > 0 && temp_maze[current_i + 1][current_j] < min_value) {
            next_i = current_i + 1; next_j = current_j; min_value = temp_maze[next_i][next_j];
        }
        if (current_i - 1 >= 0 && temp_maze[current_i - 1][current_j] > 0 && temp_maze[current_i - 1][current_j] < min_value) {
            next_i = current_i - 1; next_j = current_j; min_value = temp_maze[next_i][next_j];
        }
        if (current_j + 1 < 13 && temp_maze[current_i][current_j + 1] > 0 && temp_maze[current_i][current_j + 1] < min_value) {
            next_i = current_i; next_j = current_j + 1; min_value = temp_maze[next_i][next_j];
        }
        if (current_j - 1 >= 0 && temp_maze[current_i][current_j - 1] > 0 && temp_maze[current_i][current_j - 1] < min_value) {
            next_i = current_i; next_j = current_j - 1; min_value = temp_maze[next_i][next_j];
        }

        // 只记录合法 crossing
        char* crossingName = NULL;
        int row = (next_i - 2) / 2;
        int col = (next_j - 2) / 2;
        if ((next_i - 2) % 2 == 0 && (next_j - 2) % 2 == 0 && row >= 0 && row <= 4 && col >= 0 && col <= 4) {
            crossingName = (char*)malloc(5);
            sprintf(crossingName, "c%d%d", row, col);
        }
        if (crossingName) {
            char* temp = (char*)realloc(route, route_length + strlen(crossingName) + 2);
            if (!temp) {
                free(route);
                free(crossingName);
                return NULL;
            }
            route = temp;
            if (route_length > 0) route[route_length++] = ' ';
            strcpy(route + route_length, crossingName);
            route_length += strlen(crossingName);
            free(crossingName);
        }
        current_i = next_i;
        current_j = next_j;
    }

    if (route) route[route_length] = '\0';
    return route;
}




int* Challenge_2(int Start, int Passing[]) {
    int A = Start;
    int B = Passing[0];
    int C = Passing[1];
    int D = Passing[2];

    printf("-----------------------------------------------------------\n");
    printf("All Stations:\n");
    printf("Start     : %d\n", A);
    printf("Passing 1 : %d\n", B);
    printf("Passing 2 : %d\n", C);
    printf("Passing 3 : %d\n", D);

    // All possible permutations
    int permutations[6][3] = {
        {B, C, D},
        {B, D, C},
        {C, B, D},
        {C, D, B},
        {D, B, C},
        {D, C, B}
    };

    // Allocate memory for the result (must be freed by caller)
    int* shortest_order = (int*)malloc(4 * sizeof(int));
    if (!shortest_order) {
        perror("Failed to allocate memory for shortest_order");
        return NULL;
    }

    shortest_order[0] = A;
    shortest_order[1] = 0;
    shortest_order[2] = 0;
    shortest_order[3] = 0;

    int shortest_length = INT_MAX;

    for (int i = 0; i < 6; i++) {
        int x = permutations[i][0];
        int y = permutations[i][1];
        int z = permutations[i][2];

        int maze1[13][13], maze2[13][13], maze3[13][13];
        memcpy(maze1, maze, sizeof(maze));
        memcpy(maze2, maze, sizeof(maze));
        memcpy(maze3, maze, sizeof(maze));

        char* r1 = Fast_Route(A, x, maze1);
        char* r2 = Fast_Route(x, y, maze2);
        char* r3 = Fast_Route(y, z, maze3);

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

        free(r1);
        free(r2);
        free(r3);
    }

    printf("Shortest total route length: %d\n", shortest_length);
    return shortest_order;  // Caller must free this!
}