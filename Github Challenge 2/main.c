#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "Challenge_2.c"

//-------------------- 宏定义 --------------------
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


//-------------------- 路径与指令相关函数 --------------------
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

Direction get_final_facing_direction(const char* coords[], int count, Direction initial_dir) {
    Direction dir = initial_dir;
    for (int i = 0; i < count - 1; i++) {
        dir = get_move_direction(coords[i], coords[i + 1]);
    }
    return dir;
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
    return "";
}

//-------------------- 发送命令相关函数 --------------------
void send_robot_commands_with_log(HANDLE hSerial, char* command_list[], int command_count) {
    char recvChar = 0;
    int current_command = 0;
    printf("\n---------- Communication Log ----------\n");
    printf("[Format: Sending -> Reicever]\n\n");
    while (current_command < command_count) {
        do {
            readByte(hSerial, &recvChar);
            if (recvChar != '+') {
                printf("Unknown Signal: %c, Keep Waiting...\n", recvChar);
            }
        } while (recvChar != '+');
        printf("Receive : %c\n", recvChar);
        printf("Sending : %-5s -> ", command_list[current_command]);
        writeByte(hSerial, command_list[current_command]);
        if (strcmp(command_list[current_command], "R") == 0 || strcmp(command_list[current_command], "L") == 0) {
            Sleep(800);
        }
        if (strcmp(command_list[current_command], "F") == 0) {
            Sleep(2200);
        }
        current_command++;
    }
    printf("\n---------- Communication End ----------\n");
}

//-------------------- 主函数 --------------------
int main() {
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

    int* best_order = Challenge_2(Start_Station, Pass_stations);

    char* full_commands = (char*)malloc(1000 * sizeof(char));
    full_commands[0] = '\0';

    for (int i = 0; i < 3; i++) {
        char* route_str = Fast_Route(best_order[i], best_order[i + 1]);
        printf("Raw route from %d to %d: %s\n", best_order[i], best_order[i + 1], route_str);

        char* coords[100];
        int coord_count = 0;
        split_route_to_coords(route_str, coords, &coord_count);

        Direction initial_dir = get_initial_direction(best_order[i]);
        char* commands = path_to_commands((const char**)coords, coord_count, initial_dir);
        printf("Generated Commands: %s\n", commands);

        Direction final_facing = get_final_facing_direction((const char**)coords, coord_count, initial_dir);
        printf("Final Facing Direction: %s\n", dir_names[final_facing]);

        const char* final_move = Final_Move(final_facing, best_order[i + 1]);
        printf("Final Move: %s\n", final_move);
        printf("\n");

        strcat(commands, final_move);
        strcat(commands, " ");
        strcat(full_commands, commands);

        free(route_str);
        free(commands);
        for (int j = 0; j < coord_count; j++) {
            free(coords[j]);
        }
    }

    // ----------- 去除 R/L 后面 F 的优化 -----------
    char* optimized_commands = (char*)malloc(1000 * sizeof(char));
    optimized_commands[0] = '\0';
    int len = strlen(full_commands);
    for (int i = 0; i < len; i++) {
        if ((full_commands[i] == 'R' || full_commands[i] == 'L') &&
            full_commands[i + 1] == ' ' && full_commands[i + 2] == 'F' &&
            (full_commands[i + 3] == ' ' || full_commands[i + 3] == '\0')) {
            strncat(optimized_commands, &full_commands[i], 1);
            strcat(optimized_commands, " ");
            i += 2;
        } else {
            strncat(optimized_commands, &full_commands[i], 1);
        }
    }

    char* command_list[100];
    int command_count = 0;
    char* token = strtok(full_commands, " ");
    while (token != NULL) {
        command_list[command_count++] = strdup(token);
        token = strtok(NULL, " ");
    }
    printf("Command List:\n");
    for (int i = 0; i < command_count; i++) {
        printf("%s ", command_list[i]);
    }
    printf("\n");

    HANDLE hSerial = CreateFile(COMPORT, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error: Unable to open COM port.\n");
        return 1;
    }
    initSio(hSerial);

    send_robot_commands_with_log(hSerial, command_list, command_count);

    for (int i = 0; i < command_count; i++) {
        free(command_list[i]);
    }
    free(full_commands);
    free(optimized_commands);
    free(best_order);
    CloseHandle(hSerial);

    return 0;
}
