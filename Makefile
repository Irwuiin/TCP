CC:=arm-linux-gnueabihf-gcc
ifeq (1, $(HOST_EXEC))
CC:=gcc
endif

CFLAGS:=-Wall -Wextra -pedantic -std=c99
LDFLAGS:=
RM:=rm

ifeq (1, $(DEBUG))
CFLAGS+=-DDEBUG -g
endif

SERVER:=server
CLIENT:=client
EXECUTABLES:= $(SERVER) $(CLIENT)

SERVER_FILES:=  TCPServerUtility.c \
				server.c

CLIENT_FILES:= client.c

INC_FILES:= Practical.h

SERVER_OBJS:=$(patsubst %.c,%.o,$(SERVER_FILES))
CLIENT_OBJS:=$(patsubst %.c,%.o,$(CLIENT_FILES))

.PHONY: all server client clean

all : server client

server : $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $(SERVER) DieWithMessage.c $(SERVER_OBJS)
	scp $(SERVER) debian@192.168.100.24:/home/debian/server

client : $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT) DieWithMessage.c $(CLIENT_OBJS)

$(SERVER_OBJS) : %.o:%.c
	@$(CC) -c $(CFLAGS) $< -o $@

$(CLIENT_OBJS) : %.o:%.c
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@$(RM) -rf $(EXECUTABLES) *.o
