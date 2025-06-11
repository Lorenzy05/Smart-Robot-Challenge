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

// Crossing to Station
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