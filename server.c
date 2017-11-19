#include "server.h"

bool check_end(char* buffer){
  char* end = NULL;
  end = strstr(buffer,"\r\n\r\n");
  if(end != NULL)
    return true;
  else
    return false;
}

int get_length(char* buffer){
  char* start = strstr(buffer,"Content-Length: ");
  start += sizeof("Content-Length: ") - 1;
  char* end = strstr(start, "\r\n");
  start[end-start] = '\0';
  //printf("FUNCTION LENGTH STRING: %s\n", start);
  int size = atoi(start);
  //printf("FUNCTION LENGTH INT: %d\n", size);
  return size;
}

char* get_final_host(char* buffer){
  char* hostname = strstr(buffer, "Host: ");
  hostname += sizeof("Host: ") - 1;
  char* end = strstr(hostname,"\r\n");
  hostname[end-hostname] = '\0'; //parece que as vezes escreve onde nao deve, arrumar 
  printf("[*] Destination host: %s\n",hostname);
  return hostname;
}

bool close_connection(char* buffer){
  char* end = NULL;
  end = strstr(buffer, "Connection: close");
  if(end != NULL)
    return true;
  else
    return false;
}

bool have_content(char* buffer){
  char* content = NULL;
  content = strstr(buffer, "Content-Type:");
  if(content != NULL)
    return true;
  else
    return false;
}

int get_header_size(char* buffer){

}


int run_tcp_server(long int port){
  //clientSocket and serverSocket are defined globally on common.h
  struct sockaddr_in echoServerAddress; //local adress
  struct sockaddr_in echoClientAddress; //client address
  unsigned int clientLen; //length of client address data structure
  char buffer[BUFFER_SIZE];
  char init_message[BUFFER_SIZE];
  int rw_flag = 0;
  bool close_flag = false;
  int remaining_data = 0;
  bool first_message = true;
  int rw_flag1 = 0;
  int rw_flag2 = 0;
  int header_size = 0;


  //proxy-finalhost communication structures
  char* destination_host = NULL;
  struct hostent *final_host;
  struct sockaddr_in server_addr;
  long int host_port = 80;

  signal(SIGALRM,&timeout_error);//setting a signal for a possible timeout event
  signal(SIGALRM,&gtfo);
  printf("[.] RUNNING TCP SERVER\n");
  
  for(;;){
  /*BEGIN - CONNECTIONS SETUP*/
  //creating a TCP socket
  if( (serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 )
      handle_error("[!] socket() failed");
  printf("[*] Socket created \n");

  /*codigo de teste para tentar resolver o bind na mesma porta depois de rodar uma vez*/
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
    error("setsockopt(SO_REUSEADDR) failed");
  
  //construct local address structure
  memset(&echoServerAddress, 0, sizeof(echoServerAddress));
  echoServerAddress.sin_family = AF_INET;
  echoServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  echoServerAddress.sin_port = htons((unsigned short) port);
  //Assign a port to a socket
  if((bind(serverSocket, (struct sockaddr *) &echoServerAddress, sizeof(echoServerAddress))) < 0)
    handle_error("[!] bind() failed\n");
  printf("[*] Bind successful \n");

    //Set socket to listen
    printf("[*] Waiting for connection... \n");
    alarm(TIMEOUT);//alarm is set for every possible blocking call
    listen(serverSocket,1); //set to 1 the maximum length to which the queue of pending connections for sockfd may grow
    clientLen = sizeof(echoClientAddress);

    //Accept new connection
    alarm(TIMEOUT);
    if( (clientSocket = accept(serverSocket,(struct sockaddr *) &echoClientAddress,&clientLen)) < 0 )
      handle_error("[!] accept() failed");
    printf("[*] Connection accepted \n");

  
      printf("------------------------------------\n");
      bzero(buffer,BUFFER_SIZE);//clears the message buffer
      bzero(init_message,BUFFER_SIZE);

      alarm(TIMEOUT);
      if((rw_flag = read(clientSocket,buffer,BUFFER_SIZE))<0)//read message sent from the client
        handle_error("[!] read() failed");
      printf("[S] Received the following message:\n %s", buffer);
      memcpy(init_message,buffer,BUFFER_SIZE);
      //printf("[*] Sending to the final host\n");
      
      /*BEGIN - OPENING CONNECTION WITH FINAL HOST*/
     
      if( (hostSocket = socket(AF_INET,SOCK_STREAM,0)) < 0)
        handle_error("[!] socket() failed\n");
      printf("[*] Host socket created \n");

      printf("[*] Extracting hostname\n");
      destination_host = get_final_host(init_message);

      if((final_host = gethostbyname(destination_host)) == NULL)
        handle_error("[!] Unknown host\n");
      printf("[*] Host found \n");

      bzero((char *) &server_addr,sizeof(server_addr));
      server_addr.sin_family = AF_INET;

      bcopy((char *) final_host->h_addr,(char *)&server_addr.sin_addr.s_addr,final_host->h_length);

      server_addr.sin_port = htons((unsigned short) host_port);

      alarm(TIMEOUT);
      if((connect(hostSocket,&server_addr,sizeof(server_addr))) < 0)
        handle_error("[!] connect() error \n");
      printf("[*] Connection successful\n");

      /*END - OPENING CONNECTION WITH FINAL HOST*/
      /*END - CONNECTIONS SETUP*/

      first_message = true;
      gtfo_flag = false;
      /*BEGIN - CLIENT-HOST COMMUNICATION*/
      printf("********************************************\n");

      alarm(5);
      do{ 
        if(first_message){
          printf("[*] Writing first request to the host\n");
          if((send(hostSocket,buffer,rw_flag,0)<0))
            handle_error("[!] write() failed");
          printf("[C] Wrote: %s\n");
          first_message = false;
        }

       // printf("*********************************\n");
        
        do{
          printf("%");
          bzero(buffer,BUFFER_SIZE);
          rw_flag2 = recv(hostSocket,buffer,BUFFER_SIZE,MSG_DONTWAIT);


          if(!(rw_flag2<=0)){
            send(clientSocket,buffer,rw_flag2,MSG_DONTWAIT);
            printf("[H] Wrote: %s\n");
          }
        }while(rw_flag2 > 0);

        do{
          bzero(buffer,BUFFER_SIZE);   
          rw_flag1 = recv(clientSocket,buffer,BUFFER_SIZE,MSG_DONTWAIT);

          if(!(rw_flag1<=0)){
            send(hostSocket,buffer,rw_flag1,MSG_DONTWAIT);
            printf("[C] Wrote: %s\n");
          }
        }while(rw_flag1 > 0);
      }while(!(gtfo_flag));

      /*END - CLIENT-HOST COMMUNICATION*/
      printf("[*] Communication ended\n");
      printf("[*] Cleaning buffer\n");
      bzero(buffer,BUFFER_SIZE); 
      
    close(clientSocket);//close the client socket
    printf("[*] Client socket closed\n");

    close(hostSocket);//close the host socket
    printf("[*] Host socket closed\n");
    

    close(serverSocket);//close the server socket
    printf("[*] Server socket closed... Sleeping\n");
    sleep(2);
  }

  return 0;

}



