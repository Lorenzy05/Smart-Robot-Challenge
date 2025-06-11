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

// Create a wall into maze
void create_wall(int maze[13][13], char* Current_Crossing, int Distance, char Current_Direction) {
    int y = Current_Crossing[1] - '0'; // crossing 行
    int x = Current_Crossing[2] - '0'; // crossing 列
    int i, j;
    
    if (Distance == 1){
        if (Current_Direction == 'N') {
        // 北边的墙在 crossing 上方
        y = y - 1;
        i = 2 + y * 2 + 1;
        j = 2 + x * 2;
        } else if (Current_Direction == 'S') {
            // 南边的墙在 crossing 下方
            i = 2 + y * 2 + 1;
            j = 2 + x * 2;
        } else if (Current_Direction == 'E') {
            // 东边的墙在 crossing 右侧
            i = 2 + y * 2;
            j = 2 + x * 2 + 1;
        } else if (Current_Direction == 'W') {
            // 西边的墙在 crossing 左侧
            x = x - 1;
            i = 2 + y * 2;
            j = 2 + x * 2 + 1;
        } else {
            // 非法方向
            return;
        }

        // 在 maze 中创建墙
        if (i >= 0 && i < 13 && j >= 0 && j < 13) {
            if (maze[i][j] != -1) {
                maze[i][j] = -1;
            }
        }
    }

    // 如果距离大于1，则需要创建一条完整的墙
    else if (Distance > 1) {
        for (int d = 0; d < Distance; d++) {
            if (Current_Direction == 'N') {
                y = y - d;
                i = 2 + y * 2 + 1;
                j = 2 + x * 2;
            } else if (Current_Direction == 'S') {
                i = 2 + y * 2 + 1;
                j = 2 + x * 2;
            } else if (Current_Direction == 'E') {
                i = 2 + y * 2;
                j = 2 + x * 2 + d + 1;
            } else if (Current_Direction == 'W') {
                x = x - d;
                i = 2 + y * 2;
                j = 2 + x * 2 + d + 1;
            } else {
                // 非法方向
                return;
            }

            // 在 maze 中创建墙
            if (i >= 0 && i < 13 && j >= 0 && j < 13) {
                if (maze[i][j] != -1) {
                    maze[i][j] = -1;
                }
            }
        }
    }
    
}