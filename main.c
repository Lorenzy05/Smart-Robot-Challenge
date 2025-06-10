#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>
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

int main()
{
    HANDLE hSerial;

    //----------------------------------------------------------
    // Open COMPORT for reading and writing
    //----------------------------------------------------------
    hSerial = CreateFile(COMPORT,
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            printf(" serial port does not exist \n");
        }
        printf(" some other error occurred. Inform user.\n");
        return 1;
    }

    //----------------------------------------------------------
    // Initialize the parameters of the COM port
    //----------------------------------------------------------
    initSio(hSerial);

    char byteBuffer[100];


    printf("\n---------- Manual Command Mode ----------\n");

    while (1) {
        char recvChar = 0;
        // Waiting for the 'c' character from the Zigbee device
        do {
            readByte(hSerial, &recvChar);
        } while (recvChar != 'c');

        printf("Received +\n");

        // Receive distance data from Zigbee device

        char distanceChar = 0;
        int Distance = -1;
        /*
        while (1) {
            readByte(hSerial, &distanceChar);
            if (distanceChar >= '0' && distanceChar <= '7') {
                Distance = distanceChar - '0';
                break;
            }
        }
        */

        readByte(hSerial, &distanceChar);
        printf("Received Distance: %d\n", Distance);

        // Input command to send to Zigbee device
        printf("Input Command : ");
        if (fgets(byteBuffer, sizeof(byteBuffer), stdin) == NULL)
            break;

        size_t len = strlen(byteBuffer);
        if (len > 0 && byteBuffer[len - 1] == '\n') {
            byteBuffer[len - 1] = '\0';
        }

        if (strcmp(byteBuffer, "q") == 0)
            break;

        writeByte(hSerial, byteBuffer);
    }


    printf("\n---------- Communication Done ----------\n");
    printf("ZIGBEE IO DONE!\n");

    CloseHandle(hSerial);

    return 0;
}

