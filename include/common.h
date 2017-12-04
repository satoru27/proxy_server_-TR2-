#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>

#define TIMEOUT 60
#define BUFFER_SIZE 1500 //maximum tcp packet size

#define not_listed 0
#define whitelisted 1
#define blacklisted -1
#define denied_term -1

int handle_error(char *message);
void timeout_error();
void gtfo();

extern int listenSocket;
extern int clientSocket;
extern int hostSocket;
extern bool gtfo_flag;

void listLoaders();
extern char* blacklistPointer;
extern char* whitelistPointer;
extern char* bannedPointer; 


//for the lulz
void spinning_thing(int sleep_time);