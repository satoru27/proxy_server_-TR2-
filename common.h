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
#define BUFFER_SIZE 1024

int handle_error(char *message);
void timeout_error();

extern int serverSocket;
extern int clientSocket;
extern int hostSocket;


//for the lulz
void spinning_thing(int sleep_time);
