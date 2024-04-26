#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "Practical.h"

extern FILE* fileStream;

void HandleTCPClient(int clntSocket) {
    char buffer[BUFSIZE]; // Buffer for echo string

    // Receive message from client
    ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
#ifdef DEBUG
    printf("Bytes received: %d\n", (int)numBytesRcvd);
#endif
    if (numBytesRcvd < 0) {
        DieWithSystemMessage("recv() failed");
    }

    // Send received string and receive again until end of stream
    while (numBytesRcvd > 0) { // 0 indicates end of stream
        // Write data to file
        ssize_t numBytesWrt = fwrite(buffer, 1, numBytesRcvd, fileStream);
        // Echo message back to client
        //ssize_t numBytesSent = send(clntSocket, buffer, numBytesRcvd, 0);
#ifdef DEBUG
        //printf("Bytes sent: %d\n", (int)numBytesSent);
        printf("Bytes write: %d\n", (int)numBytesWrt);
#endif
        //if (numBytesSent < 0)
            //DieWithSystemMessage("send() failed");
        //else if (numBytesSent != numBytesRcvd)
        if (numBytesWrt != numBytesRcvd) {
            //DieWithUserMessage("send()", "sent unexpected number of bytes");
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
    }

    if (fclose(fileStream) != 0) {
        DieWithSystemMessage("fclose() failed");
    }
    close(clntSocket); // Close client socket
    syncfs();
}
