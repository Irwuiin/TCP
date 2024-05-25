#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "Practical.h"

#define BUFFER_LENGTH 61440

int main(int argc, char *argv[]) {

    char ackMessage[] = " ACK";

    if (argc < 3 || argc > 4) { // Test for correct number of arguments
        DieWithUserMessage("Parameter(s)",
        "<Server Address> <Echo Word> [<Server Port>]");
    }

    char *servIP = argv[1]; // First arg: server IP address (dotted quad)
    char *filePath = argv[2]; // Second arg: file path

    //Check if the file is valid
    FILE* inputFile = fopen(filePath, "rb");
    if (inputFile == NULL)
    {
        DieWithSystemMessage("fopen() failed");
    }

    // Third arg (optional): server port (numeric). 7 is well-known echo port
    in_port_t servPort = (argc == 4) ? atoi(argv[3]) : 7;

    // Create a reliable, stream socket using TCP
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        DieWithSystemMessage("socket() failed");
    }

    // Construct the server address structure
    struct sockaddr_in servAddr; // Server address
    memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
    servAddr.sin_family = AF_INET; // IPv4 address family
    // Convert address
    int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
    if (rtnVal == 0) {
        DieWithUserMessage("inet_pton() failed", "invalid address string");
    }
    else if (rtnVal < 0) {
        DieWithSystemMessage("inet_pton() failed");
    }
    servAddr.sin_port = htons(servPort); // Server port

    // Establish the connection to the echo server
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        DieWithSystemMessage("connect() failed");
    }

    // Determine file size
    fseek(inputFile, 0, SEEK_END);
    ssize_t remainingBytes = ftell(inputFile);
    #ifdef DEBUG
    printf("Input file size: %d\n", (int)remainingBytes);
    #endif
    rewind(inputFile);
    uint8_t messageCounter = '0';
    // Start sending file to the server
    while(remainingBytes > 0) {
        char readBuffer[BUFFER_LENGTH];
        ssize_t  bytesRead = fread(readBuffer, 1, BUFFER_LENGTH, inputFile);
        #ifdef DEBUG
        printf("Bytes read from file %d\n", (int)bytesRead);
        #endif
        if (BUFFER_LENGTH != bytesRead)
        {
            if(feof(inputFile) != 0) {
                // End-of-file reached 
                clearerr(inputFile);
            }
            else if(ferror(inputFile) != 0) {
                clearerr(inputFile);
                DieWithSystemMessage("fread() error");
            }
        }
        ssize_t numBytes = send(sock, readBuffer, bytesRead, 0);
        remainingBytes -= numBytes;
        #ifdef DEBUG
        printf("Bytes send: %d\n", (int)numBytes);
        #endif
        if (numBytes < 0)
            DieWithSystemMessage("send() failed");
        else if (numBytes != bytesRead)
            DieWithUserMessage("send()", "sent unexpected number of bytes");

        if (remainingBytes > BUFFER_LENGTH)
        {
            // Receive ACK from the server
            unsigned int totalBytesRcvd = 0; // Count of total bytes received
            while (totalBytesRcvd < strlen(ackMessage)) {
                char buffer[5]; // I/O buffer
                /* Receive up to the buffer size (minus 1 to leave space for
                a null terminator) bytes from the sender */
                numBytes = recv(sock, buffer, 5 - 1, 0);
                #ifdef DEBUG
                printf("Received Bytes: %d\n", (int)numBytes);
                #endif
                if (numBytes < 0) {
                    DieWithSystemMessage("recv() failed");
                }
                else if (numBytes == 0) {
                    DieWithUserMessage("recv()", "connection closed prematurely");
                }
                totalBytesRcvd += numBytes; // Keep tally of total bytes
                // Check counter block
                messageCounter = (messageCounter % ':') ? messageCounter : '0';
                if (messageCounter == buffer[0]) {
                    buffer[numBytes] = '\0'; // Terminate the string!
                    fputs(buffer, stdout); // Print the ACK message
                    messageCounter++;
                }
                else {
                    DieWithUserMessage(" ACK error","wrong sequence number!!!");
                }
            }
            fputc('\n', stdout); // Print a final linefeed
        }
        else { 
            //Last transmission, no ACK message
        }
    }
    if (fclose(inputFile) != 0)
        {
            DieWithSystemMessage("fclose() failed");
        }
    close(sock);
    exit(0);
}
