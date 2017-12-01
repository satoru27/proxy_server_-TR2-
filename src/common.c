#include "../include/common.h"

int listenSocket = -1;
int clientSocket = -1;
int hostSocket = -1;
bool gtfo_flag = false;

char* blacklistPointer = NULL;
char* whitelistPointer = NULL;
char* bannedPointer = NULL; 

int handle_error(char *message){
  printf("------------------------------------\n");
  printf("[!] Program has encountered the following error:\n%s", message);
  printf("[!] Exiting...\n");
  if(clientSocket > -1){
    close(clientSocket);
    printf("[!] Closed client socket\n");
  }
  if(listenSocket > -1){
    close(listenSocket);
    printf("[!] Closed server socket\n");
  }
  if(hostSocket > -1){
    close(hostSocket);
    printf("[!] Closed host socket\n");
  }
  exit(1);
}

void timeout_error(){
  printf("------------------------------------\n");
  printf("[!] Program has timed out\n");
  printf("[!] Exiting...\n");
  if(clientSocket > -1){
    close(clientSocket);
    printf("[!] Closed client socket\n");
  }
  if(listenSocket > -1){
    close(listenSocket);
    printf("[!] Closed server socket\n");
  }
  if(hostSocket > -1){
    close(hostSocket);
    printf("[!] Closed host socket\n");
  }
  exit(1);
}


void gtfo(){
  gtfo_flag = true;
}

//for the lulz

void spinning_thing(int sleep_time){
printf("Welcome...\n ");
  while(1){
    printf("\b\\");
    fflush(stdout);
    sleep(sleep_time);
    printf("\b|");
    fflush(stdout);
    sleep(sleep_time);
    printf("\b/");
    fflush(stdout);
    sleep(sleep_time);
    printf("\b-");
    fflush(stdout);
    sleep(sleep_time);
  }
}

void listLoaders(){
  
}
