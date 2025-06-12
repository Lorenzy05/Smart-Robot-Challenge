#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "Challenge_4.c"
#include <stdbool.h>
#include <time.h>

#define COMPORT "COM5"
#define BAUDRATE CBR_9600

#define MAX_CMD_LOG 200
char commands_log[MAX_CMD_LOG][16];
int commands_log_count = 0;

//--------------------------------------------------------------
// Function: initSio
// Description: intializes the parameters as Baudrate, Bytesize,
//           Stopbits, Parity and Timeoutparameters of
//           the COM port
//--------------------------------------------------------------
void initSio(HANDLE hSerial){

    COMMTIMEOUTS timeouts ={0};
    DCB dcbSerialParams = {0};

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        //error getting state
        printf("error getting state \n");
    }

    dcbSerialParams.BaudRate = BAUDRATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    if(!SetCommState(hSerial, &dcbSerialParams)){
        //error setting serial port state
        printf("error setting state \n");
    }

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if(!SetCommTimeouts(hSerial, &timeouts)){
    //error occureed. Inform user
        printf("error setting timeout state \n");
    }
}

//--------------------------------------------------------------
// Function: readByte
// Description: reads a single byte from the COM port into
//              buffer buffRead
//--------------------------------------------------------------
int readByte(HANDLE hSerial, char *buffRead) {

    DWORD dwBytesRead = 0;

    if (!ReadFile(hSerial, buffRead, 1, &dwBytesRead, NULL))
    {
        printf("error reading byte from input buffer \n");
    }
    printf("Byte read from read buffer is: %c \n", buffRead[0]);
    return(0);
}

//--------------------------------------------------------------
// Function: writeByte
// Description: writes a single byte stored in buffRead to
//              the COM port
//--------------------------------------------------------------
int writeByte(HANDLE hSerial, char *buffWrite){

    DWORD dwBytesWritten = 0;

    if (!WriteFile(hSerial, buffWrite, 1, &dwBytesWritten, NULL))
    {
        printf("error writing byte to output buffer \n");
    }
    printf("Byte written to write buffer is: %c \n", buffWrite[0]);

    return(0);
}

// 定义方向和指令
typedef enum { N, E, S, W } Direction;
const char* dir_names[] = {"N", "E", "S", "W"};

// 获取移动方向（从当前点到下一点）
Direction get_move_direction(const char* current, const char* next) {
    int current_i = current[1] - '0';
    int current_j = current[2] - '0';
    int next_i = next[1] - '0';
    int next_j = next[2] - '0';

    if (next_i < current_i) return N;      // 北移
    if (next_i > current_i) return S;      // 南移
    if (next_j > current_j) return E;      // 东移
    return W;                              // 西移
}

// 根据起始站获取初始朝向
Direction get_initial_direction(int start_station) {
    if (start_station >= 1 && start_station <= 3) return N;
    if (start_station >= 4 && start_station <= 6) return W;
    if (start_station >= 7 && start_station <= 9) return S;
    return E; // 10-12
}

// 生成转向指令
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
        case 1:  // 右转
            append_command(commands, "R");
            append_command(commands, "F");
            break;
        case 2:  // 掉头
            append_command(commands, "B");
            break;
        case 3:  // 左转
            append_command(commands, "L");
            append_command(commands, "F");
            break;
    }
}

// 主转换函数
char* path_to_commands(const char* coords[], int count, Direction initial_dir) {
    char* commands = (char*)malloc(1000 * sizeof(char));
    commands[0] = '\0';
    Direction current_dir = initial_dir;

    for (int i = 0; i < count - 1; i++) {
        Direction move_dir = get_move_direction(coords[i], coords[i+1]);
        generate_turn_commands(current_dir, move_dir, commands);
        current_dir = move_dir; // 更新当前朝向
    }
    return commands;
}


// 拆分字符串为 crossing 坐标数组
void split_route_to_coords(const char* route, char* coords[], int* count) {
    char* temp = strdup(route);
    char* token = strtok(temp, " ");
    int idx = 0;

    while (token != NULL) {
        coords[idx] = strdup(token); // 分配副本
        idx++;
        token = strtok(NULL, " ");
    }
    *count = idx;
    free(temp);
}

Direction get_final_facing_direction(const char* coords[], int count, Direction initial_dir) {
    Direction dir = initial_dir;
    for (int i = 0; i < count - 1; i++) {
        dir = get_move_direction(coords[i], coords[i + 1]);
    }
    return dir;
}


const char* Begin_Crossing(int station){
    if (station == 1) return "c41";
    if (station == 2) return "c42";
    if (station == 3) return "c43";
    if (station == 4) return "c34";
    if (station == 5) return "c24";
    if (station == 6) return "c14";
    if (station == 7) return "c03";
    if (station == 8) return "c02";
    if (station == 9) return "c01";
    if (station == 10) return "c10";
    if (station == 11) return "c20";
    if (station == 12) return "c30";
    return ""; // 如果不匹配，返回空字符串，防止崩溃
}


const char* Final_Move(Direction current_dir, int station) {
    if (station == 1 || station == 2 || station == 3) {
        if (current_dir == S) return "B P";
        if (current_dir == E) return "L P";
        if (current_dir == W) return "R P";
    }

    if (station == 4 || station == 5 || station == 6) {
        if (current_dir == N) return "L P";
        if (current_dir == S) return "R P";
        if (current_dir == E) return "B P";
    }

    if (station == 7 || station == 8 || station == 9) {
        if (current_dir == N) return "B P";
        if (current_dir == E) return "R P";
        if (current_dir == W) return "L P";
    }

    if (station == 10 || station == 11 || station == 12) {
        if (current_dir == N) return "R P";
        if (current_dir == S) return "L P";
        if (current_dir == W) return "B P";
    }

    return ""; // 如果不匹配，返回空字符串，防止崩溃
}

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

}

typedef struct {
    char crossing[8];
    char direction; // 'N'/'E'/'S'/'W'
    int distance;
} Obstacle;

typedef struct { int row, col, dist; } Node;

#define MAX_OBSTACLES 200
Obstacle obstacles[MAX_OBSTACLES];
int obstacle_count = 0;

// 纯C实现：返回最近未访问 crossing 的坐标（row, col），找不到返回 -1,-1
void find_nearest_unvisited(const char* current, int visited[5][5], int* out_row, int* out_col) {
    int start_row = current[1] - '0';
    int start_col = current[2] - '0';
    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};
    int bfs_visited[5][5] = {0};
    Node queue[32];
    int front = 0, rear = 0;
    queue[rear++] = (Node){start_row, start_col, 0};
    bfs_visited[start_row][start_col] = 1;
    while (front < rear) {
        Node node = queue[front++];
        if (!visited[node.row][node.col] && !(node.row == start_row && node.col == start_col)) {
            *out_row = node.row;
            *out_col = node.col;
            return;
        }
        for (int d = 0; d < 4; d++) {
            int nr = node.row + dr[d];
            int nc = node.col + dc[d];
            if (nr >= 0 && nr < 5 && nc >= 0 && nc < 5 && !bfs_visited[nr][nc]) {
                queue[rear++] = (Node){nr, nc, node.dist + 1};
                bfs_visited[nr][nc] = 1;
            }
        }
    }
    *out_row = -1; *out_col = -1;
}



int main() {
    // 打开串口
    HANDLE hSerial = CreateFile(COMPORT,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("串口打开失败！\n");
        //return 1;
    }
    initSio(hSerial);

    // 初始化 visited 数组
    int visited[5][5] = {0};

    int park_station;
    printf("Input Park Station (1-12): ");
    scanf("%d", &park_station);
    const char* park_crossing = Begin_Crossing(park_station);
    printf("\n");

    char response = 0;
    printf("Waiting '+' Signal...");
    do {
        readByte(hSerial, &response);
    } while (response != '+');
    printf("Received: %c\n", response);

    // 设置起点
    char Current_Crossing[8] = "c41";
    int cur_row = Current_Crossing[1] - '0';
    int cur_col = Current_Crossing[2] - '0';
    visited[cur_row][cur_col] = 1;

    // 初始方向
    Direction initial_dir = N;
    Direction dir = N; // 初始方向

    while (1) {
        // 找到最近未访问 crossing
        printf("\n");
        int next_row, next_col;
        find_nearest_unvisited(Current_Crossing, visited, &next_row, &next_col);

        if (next_row == -1 && next_col == -1) {
            printf("All crossing Visited. \n");
            break;
        }

        char next_crossing[8];
        sprintf(next_crossing, "c%d%d", next_row, next_col);

        printf("==== Route: %s -> %s ====\n", Current_Crossing, next_crossing);

        // Route planning
        int maze_for_plan[13][13];
        memcpy(maze_for_plan, maze, sizeof(maze));
        for (int i = 0; i < obstacle_count; i++) {
            create_wall(maze_for_plan, obstacles[i].crossing, obstacles[i].distance, obstacles[i].direction);
        }
        char* route = Crossing_to_Crossing(Current_Crossing, next_crossing, maze_for_plan);
        char* coords[64];
        int count = 0;
        coords[0] = strdup(Current_Crossing);
        split_route_to_coords(route, coords + 1, &count);
        count++;

        int cur_idx = 0;
        int need_replan = 0;
        while (cur_idx < count - 1) {

            Direction move_dir = get_move_direction(coords[cur_idx], coords[cur_idx+1]);
            char commands[64] = "";
            generate_turn_commands(dir, move_dir, commands);
            char* cmd = strtok(commands, " ");
            while (cmd) {

                printf("Current Position: %s, Next crossing: %s, Current Direction: %s\n", coords[cur_idx], coords[cur_idx+1], dir_names[dir]);
                printf("Ready to Send: %s\n", cmd);
                if (cmd[0] == 'F') {
                    /*
                    int distance;
                    printf("请输入障碍距离（0=无障碍，1~4为距离）：");
                    scanf("%d", &distance);
                    */
                    // 1. Check if there is a wall in front
                    char distance_from_sensor;
                    readByte(hSerial, &distance_from_sensor);
                    int distance = distance_from_sensor - '0';

                    if (distance == 1 && obstacle_count < MAX_OBSTACLES) {
                        printf(">>> Adding: crossing=%s, direction=%s, facing=%s\n", coords[cur_idx], dir_names[dir], dir_names[dir]);
                        strcpy(obstacles[obstacle_count].crossing, coords[cur_idx]);
                        obstacles[obstacle_count].direction = dir_names[dir][0];
                        obstacles[obstacle_count].distance = distance;
                        obstacle_count++;
                        need_replan = 1;
                        break; // 跳出本 crossing 的指令循环，重新规划
                    }
                    // 前进一步
                    cur_idx++;
                    int row = coords[cur_idx][1] - '0';
                    int col = coords[cur_idx][2] - '0';
                    visited[row][col] = 1;
                    strcpy(Current_Crossing, coords[cur_idx]);
                } else if (cmd[0] == 'R') {
                    dir = (dir + 1) % 4;
                } else if (cmd[0] == 'L') {
                    dir = (dir + 3) % 4;
                } else if (cmd[0] == 'B') {


                    int ROW = coords[cur_idx][1] - '0';
                    int COL = coords[cur_idx][2] - '0';

                    if (ROW == 0 && COL == 0 && dir == N) {
                        cmd[0] = 'L';
                    }
                    if (ROW == 4 && COL == 0 && dir == W) {
                        cmd[0] = 'L';
                    }
                    if (ROW == 4 && COL == 4 && dir == S) {
                        cmd[0] = 'L';
                    }
                    if (ROW == 0 && COL == 4 && dir == E) {
                        cmd[0] = 'L';
                    }

                    dir = (dir + 2) % 4;
                }
                printf("Send Command: %s\n", cmd);
                char send_ch = cmd[0];
                writeByte(hSerial, &send_ch);

                if (send_ch == 'R' || send_ch == 'L') {
                    Sleep(1500);
                }
                if (send_ch == 'F') {
                    Sleep(2500);
                }

                char response = 0;
                DWORD start_time = GetTickCount();
                int timeout_ms = 7000;

                printf("Waiting '+' Signal...");
                while (1) {
                    readByte(hSerial, &response);
                    if (response == '+') break;
                    if (GetTickCount() - start_time > timeout_ms) {
                        printf("\nTimeout: No '+' received in 5 seconds! Fuck Xbee Continue.\n");
                        // 你可以选择 break 或 return 或其它处理
                        break;
                    }
                }

                printf("Received: %c\n", response);
                printf("\n");
                cmd = strtok(NULL, " ");



            }
            if (need_replan) break; // 有障碍，重新规划
        }

        for (int i = 0; i < count; i++) free(coords[i]);
        free(route);

        if (need_replan) continue; // 有障碍，重新规划
        // 到达目标 crossing，保存当前朝向
        initial_dir = dir;
    }

    printf("\n");
    printf("ALL crossing Visited, Start Parking Programme。\n");


    printf("==== Park into : %s ====\n", park_crossing);

    while (strcmp(Current_Crossing, park_crossing) != 0) {
        int maze_for_plan[13][13];
        memcpy(maze_for_plan, maze, sizeof(maze));
        for (int i = 0; i < obstacle_count; i++) {
            create_wall(maze_for_plan, obstacles[i].crossing, obstacles[i].distance, obstacles[i].direction);
        }
        char* route = Crossing_to_Crossing(Current_Crossing, park_crossing, maze_for_plan);
        char* coords[64];
        int count = 0;
        coords[0] = strdup(Current_Crossing);
        split_route_to_coords(route, coords + 1, &count);
        count++;

        int cur_idx = 0;
        int need_replan = 0;
        while (cur_idx < count - 1) {
            Direction move_dir = get_move_direction(coords[cur_idx], coords[cur_idx+1]);
            char commands[64] = "";
            generate_turn_commands(dir, move_dir, commands);
            char* cmd = strtok(commands, " ");
            while (cmd) {
                printf("Current Position: %s, Next crossing: %s, Current Direction: %s\n", coords[cur_idx], coords[cur_idx+1], dir_names[dir]);
                printf("Ready to Send: %s\n", cmd);
                if (cmd[0] == 'F') {
                    /*
                    int distance;
                    printf("请输入障碍距离（0=无障碍，1~4为距离）：");
                    scanf("%d", &distance);
                    */

                    // 1. 检查前方是否有 wall
                    char distance_from_sensor;
                    readByte(hSerial, &distance_from_sensor);
                    int distance = distance_from_sensor - '0'; // 将字符转换为整数

                    if (distance == 1 && obstacle_count < MAX_OBSTACLES) {
                        printf(">>> Adding: crossing=%s, direction=%s, facing=%s\n", coords[cur_idx], dir_names[dir], dir_names[dir]);
                        strcpy(obstacles[obstacle_count].crossing, coords[cur_idx]);
                        obstacles[obstacle_count].direction = dir_names[dir][0];
                        obstacles[obstacle_count].distance = distance;
                        obstacle_count++;
                        need_replan = 1;
                        break; // 跳出本 crossing 的指令循环，重新规划
                    }
                    // 前进一步
                    cur_idx++;
                    int row = coords[cur_idx][1] - '0';
                    int col = coords[cur_idx][2] - '0';
                    visited[row][col] = 1;
                    strcpy(Current_Crossing, coords[cur_idx]);
                } else if (cmd[0] == 'R') {
                    dir = (dir + 1) % 4;
                } else if (cmd[0] == 'L') {
                    dir = (dir + 3) % 4;
                } else if (cmd[0] == 'B') {
                    dir = (dir + 2) % 4;
                }

                printf("Sending: %s\n", cmd);
                printf("\n");
                char send_ch = cmd[0];
                writeByte(hSerial, &send_ch);


                if (send_ch == 'R' || send_ch == 'L') {
                    Sleep(1500);
                }

                if (send_ch == 'F') {
                    Sleep(2500);
                }

                 // 等待接收 '+'
                char response = 0;
                DWORD start_time = GetTickCount();
                int timeout_ms = 7000;

                printf("Waiting '+' Signal...");
                while (1) {
                    readByte(hSerial, &response);
                    if (response == '+') break;
                    if (GetTickCount() - start_time > timeout_ms) {
                        printf("\nTimeout: No '+' received in 5 seconds! Fuck Xbee Continue.\n");
                        // 你可以选择 break 或 return 或其它处理
                        break;
                    }
                }

                cmd = strtok(NULL, " ");
            }
            if (need_replan) break;
        }
        for (int i = 0; i < count; i++) free(coords[i]);
        free(route);
        if (need_replan) continue;
        // 到达目标 crossing，保存当前朝向
        initial_dir = dir;
        break; // 到达停车场，退出循环
    }

    // 获取最终动作
    const char* final_cmds = Final_Move(dir, park_station);
    if (strlen(final_cmds) > 0) {
        char cmds[64];
        strcpy(cmds, final_cmds);
        char* cmd = strtok(cmds, " ");
        while (cmd) {
            printf("Last Move into Station: %s\n", cmd);
            char send_ch = cmd[0];
            writeByte(hSerial, &send_ch);
            if (send_ch == 'R' || send_ch == 'L') {
                Sleep(1500);
            }
            if (send_ch == 'F') {
                Sleep(2500);
            }
            // 等待接收 '+'
            char response = 0;
            DWORD start_time = GetTickCount();
            int timeout_ms = 7000;

            printf("Waiting '+' Signal...");
            while (1) {
                readByte(hSerial, &response);
                if (response == '+') break;
                if (GetTickCount() - start_time > timeout_ms) {
                     printf("\nTimeout: No '+' received in 5 seconds! Fuck Xbee Continue.\n");
                        // 你可以选择 break 或 return 或其它处理
                    break;
                }
            }

            cmd = strtok(NULL, " ");
        }
    }

    printf("Reached station %s, End.\n", park_crossing);

    return 0;
}
