#include "client.h"

int run_tcp_client(char *host,long int port){
  //clientSocket is defined globally on common.h
  printf("[.] RUNNING TCP CLIENT\n");
  int rw_flag;
  struct sockaddr_in server_addr;
  struct hostent *server;
  char buffer[BUFFER_SIZE];
  int close_flag = 0;

  signal(SIGALRM,&timeout_error);

  if( (clientSocket = socket(AF_INET,SOCK_STREAM,0)) < 0)
    handle_error("[!] socket() failed\n");
  printf("[*] Socket created \n");

  if((server = gethostbyname(host)) == NULL)
    handle_error("[!] Unknown host\n");
  printf("[*] Host found \n");

  bzero((char *) &server_addr,sizeof(server_addr));
  server_addr.sin_family = AF_INET;

  bcopy((char *) server->h_addr,(char *)&server_addr.sin_addr.s_addr,server->h_length);

  server_addr.sin_port = htons((unsigned short) port);

  alarm(TIMEOUT);
  if((connect(clientSocket,&server_addr,sizeof(server_addr))) < 0)
    handle_error("[!] connect() error \n");
  printf("[*] Connection successful\n");

  for(;;){
    //printf("------------------------------------\n");
    bzero(buffer,BUFFER_SIZE);
    //printf("[*] Enter the message to be sent: > ");
    //fgets(buffer,(BUFFER_SIZE-1),stdin);
    
    strcpy(buffer,"GET / HTTP/1.1\r\nHost: www.theuselessweb.com\r\nConnection: keep-alive\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.75 Safari/537.36\r\nUpgrade-Insecure-Requests: 1\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: en-US,en;q=0.9,pt;q=0.8,ja;q=0.7\r\nDNT: 1\r\n\r\n");
    

    alarm(TIMEOUT);
    if((rw_flag = write(clientSocket,buffer,strlen(buffer))) < 0)
      handle_error("[!] write() error \n");
    printf("[*] Write successful: %s\n", buffer);
    

    close_flag = strcmp(buffer,"close()\n");

    alarm(TIMEOUT);
    printf("\n");
    
    int first = 1;
    int header_end=0;
    do{ 
    if(first){
      bzero(buffer,BUFFER_SIZE);
      
      if((rw_flag = read(clientSocket,buffer,BUFFER_SIZE)) < 0)
        handle_error("[!] read() error \n");

      header_end = check_end(buffer);
      printf("\n----------------------------------------\n");
      printf("%.*s", header_end, buffer);
      printf("\n[*] SIZE = %d", get_length(buffer));
      printf("\n----------------------------------------\n");
      first = 0;
    }
      bzero(buffer,BUFFER_SIZE);
    
      if((rw_flag = read(clientSocket,buffer,BUFFER_SIZE)) < 0)
        handle_error("[!] read() error \n");
    }while(1);


    if(close_flag == 0){
      printf("[*] Ending connection with server\n");
      break;

    }

  }
  close(clientSocket);
  printf("[*] Client socket closed\n");
  return 0;
}

