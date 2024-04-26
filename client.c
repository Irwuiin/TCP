#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"

#define BUFFER_LENGTH 100

const char *echoString = "Hola mundo" ;

int main(int argc, char *argv[]) {

    if (argc < 3 || argc > 4) // Test for correct number of arguments
        DieWithUserMessage("Parameter(s)",
        "<Server Address> <Echo Word> [<Server Port>]");

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
    if (sock < 0)
        DieWithSystemMessage("socket() failed");

    // Construct the server address structure
    struct sockaddr_in servAddr; // Server address
    memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
    servAddr.sin_family = AF_INET; // IPv4 address family
    // Convert address

    int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
    if (rtnVal == 0)
        DieWithUserMessage("inet_pton() failed", "invalid address string");
    else if (rtnVal < 0)
        DieWithSystemMessage("inet_pton() failed");
    servAddr.sin_port = htons(servPort); // Server port

    // Establish the connection to the echo server
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithSystemMessage("connect() failed");

    // Determine file size
    fseek(inputFile, 0, SEEK_END);
    ssize_t remainingBytes = ftell(inputFile);
#ifdef DEBUG
    printf("Input file size: %d\n", (int)remainingBytes);
#endif
    rewind(inputFile);

    //ssize_t echoStringLen = strlen(echoString); // Determine input length

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
        //ssize_t numBytes = send(sock, echoString, echoStringLen, 0);
        ssize_t numBytes = send(sock, readBuffer, bytesRead, 0);
        remainingBytes -= numBytes;
    #ifdef DEBUG
        printf("Bytes send: %d\n", (int)numBytes);
    #endif
        if (numBytes < 0)
            DieWithSystemMessage("send() failed");
        //else if (numBytes != echoStringLen)
        else if (numBytes != bytesRead)
            DieWithUserMessage("send()", "sent unexpected number of bytes");

        // Receive the same string back from the server
        //unsigned int totalBytesRcvd = 0; // Count of total bytes received
        //fputs("Received: \n", stdout); // Setup to print the echoed string
        //while (totalBytesRcvd < echoStringLen) {
        //while (totalBytesRcvd < bytesRead) {
            //char buffer[BUFSIZE]; // I/O buffer
            /* Receive up to the buffer size (minus 1 to leave space for
            a null terminator) bytes from the sender */
            //numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
            //if (numBytes < 0)
                //DieWithSystemMessage("recv() failed");
            //else if (numBytes == 0)
                //DieWithUserMessage("recv()", "connection closed prematurely");
            //totalBytesRcvd += numBytes; // Keep tally of total bytes
            //buffer[numBytes] = '\0'; // Terminate the string!
            //fputs(buffer, stdout); // Print the echo buffer
        //}

        //fputc('\n', stdout); // Print a final linefeed
    }

    if (fclose(inputFile) != 0)
    {
        DieWithSystemMessage("fclose() failed");
    }
    close(sock);
    exit(0);
}
