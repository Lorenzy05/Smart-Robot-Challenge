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

//-------------------- Route and Command --------------------
typedef enum { N, E, S, W } Direction;
const char* dir_names[] = {"N", "E", "S", "W"};

Direction get_move_direction(const char* current, const char* next) {
    int current_i = current[1] - '0';
    int current_j = current[2] - '0';
    int next_i = next[1] - '0';
    int next_j = next[2] - '0';
    if (next_i < current_i) return N;
    if (next_i > current_i) return S;
    if (next_j > current_j) return E;
    return W;
}

Direction get_initial_direction(int start_station) {
    if (start_station >= 1 && start_station <= 3) return N;
    if (start_station >= 4 && start_station <= 6) return W;
    if (start_station >= 7 && start_station <= 9) return S;
    return E; // 10-12
}

void append_command(char* commands, const char* cmd) {
    strcat(commands, cmd);
    strcat(commands, " ");
}

void generate_turn_commands(Direction current, Direction target, char* commands) {
    if (current == target) {
        append_command(commands, "F");
        return;
    }
    int diff = (target - current + 4) % 4;
    switch (diff) {
        case 1:  append_command(commands, "R"); append_command(commands, "F"); break;
        case 2:  append_command(commands, "B"); break;
        case 3:  append_command(commands, "L"); append_command(commands, "F"); break;
    }
}

char* path_to_commands(const char* coords[], int count, Direction initial_dir) {
    char* commands = (char*)malloc(1000 * sizeof(char));
    commands[0] = '\0';
    Direction current_dir = initial_dir;
    for (int i = 0; i < count - 1; i++) {
        Direction move_dir = get_move_direction(coords[i], coords[i+1]);
        generate_turn_commands(current_dir, move_dir, commands);
        current_dir = move_dir;
    }
    return commands;
}

void split_route_to_coords(const char* route, char* coords[], int* count) {
    char* temp = strdup(route);
    char* token = strtok(temp, " ");
    int idx = 0;
    while (token != NULL) {
        coords[idx] = strdup(token);
        idx++;
        token = strtok(NULL, " ");
    }
    *count = idx;
    free(temp);
}