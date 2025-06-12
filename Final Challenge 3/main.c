#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "Challenge_3.c"
#include <stdbool.h>
#include <time.h>

#define COMPORT "COM5"
#define BAUDRATE CBR_9600

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


void send_robot_commands_with_log(HANDLE hSerial, char* command_list[], int command_count) {
    char response = 0; // 单字符响应
    int current_command = 0;

    printf("\n---------- Communication Log ----------\n");
    while (current_command < command_count) {
    // 1. 发送当前命令
    printf("发送: %-5s\n", command_list[current_command]);
    writeByte(hSerial, command_list[current_command]);

    // 2. 等待接收 '+' 信号
    printf("等待 '+' 信号...");
    do {
        readByte(hSerial, &response);
    } while (response != '+');
    printf("收到: %c\n", response);

    current_command++;
    printf("\n");
}

    printf("\n---------- 通信结束 ----------\n");
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
        if (current_dir == S) return "F F";
        if (current_dir == E) return "R F F";
        if (current_dir == W) return "L F F";
    }

    if (station == 4 || station == 5 || station == 6) {
        if (current_dir == N) return "R F F";
        if (current_dir == S) return "L F F";
        if (current_dir == E) return "F F";
    }

    if (station == 7 || station == 8 || station == 9) {
        if (current_dir == N) return "F F";
        if (current_dir == E) return "L F F";
        if (current_dir == W) return "R F F";
    }

    if (station == 10 || station == 11 || station == 12) {
        if (current_dir == N) return "L F F";
        if (current_dir == S) return "R F F";
        if (current_dir == W) return "F F";
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

// 修改结构体
typedef struct {
    char crossing[8];
    char direction; // 'N'/'E'/'S'/'W'
    int distance;   // 新增：障碍距离
} Obstacle;

#define MAX_OBSTACLES 100
Obstacle obstacles[MAX_OBSTACLES];
int obstacle_count = 0;

 #define MAX_CMD_LIST 1024
char cmd_list[MAX_CMD_LIST][8];
int cmd_count = 0;


int main() {
    
    // Open the serial port
    HANDLE hSerial = CreateFile(COMPORT,
    GENERIC_READ | GENERIC_WRITE,
    0,
    0,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    0
    );
    if (hSerial == INVALID_HANDLE_VALUE) {
    printf("Failed Open COMM Port\n");
    //return 1;
    }
    initSio(hSerial);

    int Pass_stations[3];
    int Start_Station;

    printf("-----------------------------------------------------------\n");
    printf("Fill start station in : ");
    scanf("%d", &Start_Station);
    printf("-----------------------------------------------------------\n");
    printf("Fill pass stations:\n");
    for (int i = 0; i < 3; i++) {
        printf("Fill pass station %d in : ", i + 1);
        scanf("%d", &Pass_stations[i]);
    }

    // Calculate the best order of stations
    int* best_order = Challenge_2(Start_Station, Pass_stations);



    for (int seg = 0; seg < 3; seg++) {
        char* begin_crossing = Begin_Crossing(best_order[seg]);
        char Current_Crossing[8];
        strcpy(Current_Crossing, begin_crossing);

        Direction initial_dir = get_initial_direction(best_order[seg]);
        const char* target_crossing = Begin_Crossing(best_order[seg + 1]);

        printf("==== Route %d: %s -> %s ====\n", seg + 1, begin_crossing, target_crossing);

        while (strcmp(Current_Crossing, target_crossing) != 0) {

            
            // Detect for Crossing
            
            char response = 0;
            printf("Waiting '+' Signal...");
            do {
                readByte(hSerial, &response);
            } while (response != '+');
            printf("Received: %c\n", response);


            // 1. 检查前方是否有 wall 
            char distance_from_sensor;
            readByte(hSerial, &distance_from_sensor);
            int distance = distance_from_sensor - '0'; // 将字符转换为整数
             
            /*
            // 这里可以模拟障碍距离输入，实际应用中可以从传感器获取
            int distance;
            printf("请输入障碍距离（0=无障碍，1~4为距离）：");
            scanf("%d", &distance);
            */

            if (distance == 1 && obstacle_count < MAX_OBSTACLES) {
                strcpy(obstacles[obstacle_count].crossing, Current_Crossing);
                obstacles[obstacle_count].direction = dir_names[initial_dir][0];
                obstacles[obstacle_count].distance = distance;
                obstacle_count++;
                snprintf(cmd_list[cmd_count++], 8, "%d", distance);
            }

            // 2. Create a wall in the maze
            int maze_for_plan[13][13];
            memcpy(maze_for_plan, maze, sizeof(maze));
            for (int i = 0; i < obstacle_count; i++) {
                create_wall(maze_for_plan, obstacles[i].crossing, obstacles[i].distance, obstacles[i].direction);
            }

            // 3. Route planning
            char* route = Crossing_to_Station(Current_Crossing, best_order[seg + 1], maze_for_plan);
            char* coords[10];
            int count = 0;
            split_route_to_coords(route, coords, &count);

            if (count > 0) {
                Direction move_dir = get_move_direction(Current_Crossing, coords[0]);
                char commands[32] = "";
                generate_turn_commands(initial_dir, move_dir, commands);

                char* first_cmd = strtok(commands, " ");
                if (first_cmd == NULL) {
                    printf("All ways Blocked \n");
                    for (int i = 0; i < count; i++) free(coords[i]);
                    free(route);
                    break;
                }

                // 只更新方向，不更新 crossing
                if (first_cmd[0] == 'R')
                    initial_dir = (initial_dir + 1) % 4;
                else if (first_cmd[0] == 'L')
                    initial_dir = (initial_dir + 3) % 4;
                else if (first_cmd[0] == 'B')
                    initial_dir = (initial_dir + 2) % 4;
                else if (first_cmd[0] == 'F') {
                    strcpy(Current_Crossing, coords[0]);
                    initial_dir = move_dir;
                }

                printf("Current crossing : %s, Target crossing : %s, Current Direction : %s\n", Current_Crossing, coords[0], dir_names[initial_dir]);
                printf("Sending Command : %s\n", first_cmd);

                if (first_cmd != NULL) {
                    strncpy(cmd_list[cmd_count++], first_cmd, 8); // 记录动作
                }

                // Send Command To Robot
                
                writeByte(hSerial, first_cmd);

                if (strcmp(first_cmd, "R") == 0 || strcmp(first_cmd, "L") == 0) {
                    Sleep(1500); 
                }

                if (strcmp(first_cmd, "F") == 0) {
                    Sleep(2200);
                }
                

                
                

                printf("\n");

                for (int i = 0; i < count; i++) free(coords[i]);
                free(route);
            } else {
                printf("On the final target ||| No way to final target. \n");
                break;
            }
        }

        // In the end, make a Final Move to the target crossing
        if (strcmp(Current_Crossing, target_crossing) == 0) {
            Direction final_dir = initial_dir;
            const char* final_cmd = Final_Move(final_dir, best_order[seg + 1]);
            printf("Arrived to crossing, Final Movement into Station : %s\n", final_cmd);
            // 拆分并逐条记录
            /*
            char cmd_buf[32];
            strncpy(cmd_buf, final_cmd, sizeof(cmd_buf) - 1);
            cmd_buf[sizeof(cmd_buf) - 1] = '\0';

            char* token = strtok(cmd_buf, " ");
            while (token != NULL) {
                // 记录final move动作
                strncpy(cmd_list[cmd_count++], token, 8);
                // 这里可以继续发送给机器人等
                token = strtok(NULL, " ");
            }*/
            
            // Separate the Final Movement
            char cmd_buf[32];
            strncpy(cmd_buf, final_cmd, sizeof(cmd_buf) - 1);
            cmd_buf[sizeof(cmd_buf) - 1] = '\0';

            char* token = strtok(cmd_buf, " ");
            while (token != NULL) {
                printf("Send Final Move: %s\n", token);
                writeByte(hSerial, token);

                if (strcmp(token, "R") == 0 || strcmp(token, "L") == 0) {
                    Sleep(1500); 
                }

                if (strcmp(token, "F") == 0) {
                    Sleep(2200);
                }

                // Waiting for '+'
                char response = 0;
                printf("Waiting '+' Signal...");
                do {
                    readByte(hSerial, &response);
                } while (response != '+');
                printf("Reiceive: %c\n", response);

                token = strtok(NULL, " ");
            }
                
        }
    }

    printf("Complate List: \n");
    for (int i = 0; i < cmd_count; i++) {
        printf("%s ", cmd_list[i]);
    }
    return 0;
}