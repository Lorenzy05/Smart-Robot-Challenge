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

// Crossing_to_Crossing function: This function finds the shortest path between two crossings in a maze represented by a 2D array.
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