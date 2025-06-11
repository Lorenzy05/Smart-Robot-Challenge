// Calculate the shortest path in a maze using BFS algorithm
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
