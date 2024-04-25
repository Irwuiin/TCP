CC:=gcc
CFLAGS:=-Wall -Wextra -pedantic -std=c99
LDFLAGS:=
RM:=rm

ifeq (1, $(DEBUG))
CFLAGS+=-DDEBUG -g
endif

SERVER:=server
CLIENT:=client
EXECUTABLES:= $(SERVER) $(CLIENT)

SRC_FILES:= DieWithMessage.c \
			TCPServerUtility.c

INC_FILES:= Practical.h

OBJS:=$(patsubst %.c,%.o,$(SRC_FILES))

.PHONY: all server client clean

all : server client

server : $(OBJS)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER).c $(OBJS)

client : $(OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT).c $(OBJS)

$(OBJS) : %.o:%.c
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@$(RM) -rf $(EXECUTABLES) *.o
