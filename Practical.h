#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024

void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);
void HandleTCPClient(int clntSocket);