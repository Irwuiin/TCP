#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdint.h>
#include "Practical.h"

extern FILE* fileStream;

void HandleTCPClient(int clntSocket) {
    char buffer[BUFSIZE]; // Buffer for echo string
    char ackMessage[] = " ACK";
    // Receive message from client
    ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
#ifdef DEBUG
    printf("Bytes received: %d\n", (int)numBytesRcvd);
#endif
    if (numBytesRcvd < 0) {
        DieWithSystemMessage("recv() failed");
    }
    uint8_t messageCounter = '0';
    // Write data until end of stream
    while (numBytesRcvd > 0) { // 0 indicates end of stream
        messageCounter = (messageCounter % ':') ? messageCounter : '0';
        ssize_t numBytesWrt = fwrite(buffer, 1, numBytesRcvd, fileStream);
        // Send ACK message to client
        ackMessage[0] = messageCounter;
        ssize_t numBytesSent = send(clntSocket, ackMessage, strlen(ackMessage), 0);
#ifdef DEBUG
        printf("Bytes sent: %d\n", (int)numBytesSent);
        printf("Bytes write: %d\n", (int)numBytesWrt);
#endif
        if (numBytesSent < 0)
            DieWithSystemMessage("send() failed");
        else if (numBytesSent != numBytesRcvd)
        if (numBytesWrt != numBytesRcvd) {
            DieWithUserMessage("send()", "sent unexpected number of bytes");
            DieWithUserMessage("fwrite()", "write unexpected number of bytes");
        }

        // See if there is more data to receive
        numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
#ifdef DEBUG
        printf("Bytes received: %d\n", (int)numBytesRcvd);
#endif
        if (numBytesRcvd < 0) {
            DieWithSystemMessage("recv() failed");
        }
        messageCounter++;
    }

    if (fclose(fileStream) != 0) {
        DieWithSystemMessage("fclose() failed");
    }
    close(clntSocket); // Close client socket
    syncfs();
}
