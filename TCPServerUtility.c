#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdint.h>
#include "Practical.h"

#define BUFFER_LENGTH 61440

const char* filePath = "partition.txt";
FILE* fileStream;

void HandleTCPClient(int clntSocket) {
    char buffer[BUFFER_LENGTH]; // Buffer for echo string
    char ackMessage[] = " ACK";

    // Create file to store received data
    fileStream = fopen(filePath, "wb");
    if (fileStream == NULL) {
        DieWithSystemMessage("fopen() failed");
    }
    // Receive message from client
    ssize_t numBytesRcvd = 1;
    ssize_t totalBytesRcvd = 0;
    while (totalBytesRcvd < BUFFER_LENGTH && numBytesRcvd != 0) //Read the entire buffer or until end of file
    {
        numBytesRcvd = recv(clntSocket, buffer + totalBytesRcvd, BUFFER_LENGTH - totalBytesRcvd, 0);
        #ifdef DEBUG
        printf("Bytes received: %d\n", (int)numBytesRcvd);
        #endif
        totalBytesRcvd += numBytesRcvd;
    }

#ifdef DEBUG
    printf("Total bytes received: %d\n", (int)totalBytesRcvd);
#endif
    if (totalBytesRcvd < 0) {
        DieWithSystemMessage("recv() failed");
    }
    uint8_t messageCounter = '0';
    // Write data until end of stream
    while (totalBytesRcvd > 0 ) { // 0 indicates end of stream
        messageCounter = (messageCounter % ':') ? messageCounter : '0';
        ssize_t numBytesWrt = fwrite(buffer, 1, totalBytesRcvd, fileStream);
        #ifdef DEBUG
        printf("Bytes write: %d\n", (int)numBytesWrt);
        #endif
        if (numBytesRcvd == 0)
        {
            break;//end of transmission
        }
        // Send ACK message to client
        ackMessage[0] = messageCounter;
        ssize_t numBytesSent = send(clntSocket, ackMessage, strlen(ackMessage), 0);
        #ifdef DEBUG
        printf("Bytes sent: %d\n", (int)numBytesSent);
        #endif
        if (numBytesSent < 0)
            DieWithSystemMessage("send() failed");
        else if (numBytesSent != (ssize_t)strlen(ackMessage))
            DieWithUserMessage("send()", "sent unexpected number of bytes");
        else if (numBytesWrt != totalBytesRcvd) {
            DieWithUserMessage("fwrite()", "write unexpected number of bytes");
        }

        // See if there is more data to receive
        totalBytesRcvd = 0;
        numBytesRcvd = 1;
        while (totalBytesRcvd < BUFFER_LENGTH && numBytesRcvd != 0) //Read the entire buffer or until end of file
        {
            numBytesRcvd = recv(clntSocket, buffer + totalBytesRcvd, BUFFER_LENGTH - totalBytesRcvd, 0);
            totalBytesRcvd += numBytesRcvd;
            #ifdef DEBUG
            printf("Bytes received: %d\n", (int)numBytesRcvd);
            #endif
        }
        #ifdef DEBUG
        printf("Total bytes received: %d\n", (int)totalBytesRcvd);
        #endif
        if (numBytesRcvd < 0) {
            DieWithSystemMessage("recv() failed");
        }
        messageCounter++;
    }

    if (fclose(fileStream) != 0) {
        DieWithSystemMessage("fclose() failed");
    }
    if (close(clntSocket) != 0) { // Close client socket
        DieWithSystemMessage("close() failed");
    }

    sync();
}
