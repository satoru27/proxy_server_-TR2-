#include "server.h"
#include "blacklist.h"
#define not_blacklisted 1
int run_tcp_server(long int port){
  //clientSocket and listenSocket are defined globally on common.h
  struct sockaddr_in echoServerAddress; //local adress
  struct sockaddr_in echoClientAddress; //client address
  unsigned int clientLen; //length of client address data structure
  char buffer[BUFFER_SIZE];
  char init_message[BUFFER_SIZE];
  int rw_flag = 0;
  bool close_flag = false;
  int remaining_data = 0;
  bool first_message = true;
  int rw_flag_c_h = 0;
  int rw_flag_h_c = 0;
  int header_size = 0;

  //messages to be sent if there was some problem with the connection
  //char* forbidden = "HTTP/1.1 403 Forbidden\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nContent-Length: 310\r\n\r\n<!DOCTYPE html>\n<html lang=\"en\">\n<body>\n<div class=\"cover\"><h1>Access Denied <small>- Error 403</small></h1><p class=\"lead\">The access to the requested resource was blocked by the proxy.</p></div>\n</body>\n</html>\n";
  char* forbidden = load_html_error_page("403 Forbidden","proxy_server_-TR2-/http_errors/403.html");
  //proxy-finalhost communication structures
  char* destination_host = NULL;
  char* urlSent = NULL;
  struct hostent *final_host;
  struct sockaddr_in server_addr;
  long int host_port = 80;

  signal(SIGALRM,&timeout_error);//setting a signal for a possible timeout event
  signal(SIGALRM,&gtfo);

  //BEGIN TEST
  signal(SIGPIPE,SIG_IGN);//se existir uma tetativa de escrever no socket do cliente o SIGPIPE geralmente fecha o programa, esse comando implica em ignorar esse tipo de situacao
  //fazer um tratamento melhor, pegando esse signal e etc
  //END TEST
  printf("[.] RUNNING TCP SERVER\n");
  
  for(;;){
    /*BEGIN - CONNECTIONS SETUP*/
    //creating a TCP socket
    if((listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 )
        handle_error("[!] socket() failed");
    printf("[*] Listening socket created \n");

    /*codigo de teste para tentar resolver o bind na mesma porta depois de rodar uma vez*/
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
      handle_error("[!] setsockopt(SO_REUSEADDR) failed");
    
    //construct local address structure
    memset(&echoServerAddress, 0, sizeof(echoServerAddress));
    echoServerAddress.sin_family = AF_INET;
    echoServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServerAddress.sin_port = htons((unsigned short) port);
    //Assign a port to a socket
    if((bind(listenSocket, (struct sockaddr *) &echoServerAddress, sizeof(echoServerAddress))) < 0)
      handle_error("[!] bind() failed\n");
    printf("[*] Bind successful \n");

    bool  stop_receiving_denied_pages=false;//pages that have denied terms

    for(;;){
      //Set socket to listen
      alarm(TIMEOUT);//alarm is set for every possible blocking call
      printf("[*] Waiting for connection... \n");
      listen(listenSocket,100); //set to 1 the maximum length to which the queue of pending connections for sockfd may grow
      clientLen = sizeof(echoClientAddress);

      //Accept new connection
      alarm(TIMEOUT);
      if((clientSocket = accept(listenSocket,(struct sockaddr *) &echoClientAddress,&clientLen)) < 0 )
        handle_error("[!] accept() failed");
      printf("[*] Connection accepted \n");
      printf("[*] Client socket created \n");

      printf("------------------------------------\n");
      bzero(buffer,BUFFER_SIZE);//clears the message buffer
      bzero(init_message,BUFFER_SIZE);
      
      alarm(TIMEOUT);
      if((rw_flag = read(clientSocket,buffer,BUFFER_SIZE))<0)//read message sent from the client
        handle_error("[!] read() failed");
      printf("[S] Received the following message from client:\n %s", buffer);
      memcpy(init_message,buffer,BUFFER_SIZE);
      //header_content(buffer);
      //printf("[*] Sending to the final host\n");
      
      int blacklistOK = verifyGET(buffer); //returns 1 if whitelist; returns -1 if blacklist
      if(blacklistOK==not_blacklisted){
        //only contacts final host/client if website isn't blacklisted
        /*BEGIN - OPENING CONNECTION WITH FINAL HOST*/
       
        if((hostSocket = socket(AF_INET,SOCK_STREAM,0)) < 0)
          handle_error("[!] socket() failed\n");
        printf("[*] Host socket created \n");

        printf("[*] Extracting hostname\n");
        destination_host = get_final_host(init_message);

        if((final_host = gethostbyname(destination_host)) == NULL)
          handle_error("[!] Unknown host\n");
        printf("[*] Host found \n");
        //MUDAR
        // o programa fecha se nao encontrar o host --> inconveniente
        //solucao? -- tratar com uma mensagem padrao, ou ignorar e depois voltar para o listen
        //necessario usar goto?
        
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

        //sending first message
        printf("[*] Writing request to the host\n");
        if((send(hostSocket,buffer,rw_flag,0)<0))
          handle_error("[!] write() failed");
        printf("\n[C] Wrote: %s\n",buffer);

        //header_content(buffer);
        //first_message = true;
        gtfo_flag = false;
        /*BEGIN - CLIENT-HOST COMMUNICATION*/
        printf("********************************************\n");

        int total = 0;
        int packet = 0;
        alarm(10);
        do{         
          do{
            bzero(buffer,BUFFER_SIZE);
            rw_flag_h_c = recv(hostSocket,buffer,BUFFER_SIZE,MSG_DONTWAIT);
            //printf("LOOP: %d \r", rw_flag_h_c);

            if(!(rw_flag_h_c <=0)) {
              printf("[H] Wrote: %s\n",buffer);
              /*VERIFICAR SE buffer CONTÉM DENY_TERMS*/
              blacklistOK = verifyDenyTerms(buffer);

              if(blacklistOK == not_blacklisted){ //DenyTerm not found
                send(clientSocket,buffer,rw_flag_h_c,MSG_DONTWAIT);
                packet ++; 
                total += rw_flag_h_c;
                if(packet == 1)
                  printf("[H] First packet content:\"\n%s\"\n", buffer);
                printf("[H] Packet #%d . Wrote %d bytes on client socket so far\n", packet, total);
                alarm(2);
              }else { //DenyTerm found
                rw_flag_h_c = 0; //sair do loop de packets dessa requisição pois um denyterm foi encontrado
                gtfo_flag=true;
                stop_receiving_denied_pages=true;
                printf("Sorry, this website has denied terms\n");
                if(forbidden!=NULL){
                  send(clientSocket,forbidden,strlen(forbidden),0);//atencao: usar strlen e nao sizeof, sizeof retorna o tamanho do ponteiro
                  printf("Error message sent to client: %s\n",forbidden);  
                }else{
                  printf("forbidden null\n");
                }
              }
            }
          } while((rw_flag_h_c > 0));
        } while(!(gtfo_flag));

        /*END - CLIENT-HOST COMMUNICATION*/
        printf("----TOTAL = %d ----\n\n",total);
      }else{//->if(blacklistOK==not_blacklisted)
        printf("Sorry, we do not allow access to this website\n");
        if(forbidden!=NULL){
          send(clientSocket,forbidden,strlen(forbidden),0);//atencao: usar strlen e nao sizeof, sizeof retorna o tamanho do ponteiro
          printf("Error message sent to client: %s\n",forbidden);  
        }else{
          printf("forbidden null\n");
        }       
      }
      printf("[*] Communication ended\n");
      printf("[*] Cleaning buffer\n");
      bzero(buffer,BUFFER_SIZE); 
        
      close(clientSocket);//close the client socket
      printf("[*] Client socket closed\n");

      close(hostSocket);//close the host socket
      printf("[*] Host socket closed\n");
      //system("clear");
      if(stop_receiving_denied_pages)
        break;
    }//for(;;) -> Set socket to listen
    close(listenSocket);//close the server socket
    printf("[*] Listening socket closed... Sleeping\n");
    if(stop_receiving_denied_pages)
      sleep(2);//to prevent pages from receiving the remaining pices of content 
  }//for(;;)->CONNECTIONS SETUP
  if(forbidden!=NULL){//pergunta: a funcao chega ate este ponto?
    free(forbidden);
  }
  return 0;

}
//load_html_error_page(char* html_error_code,char* path_to_html_file)
//will include html message header and data fields in order to be able
//to load an error page. The function returns a dinamically allocated
//pointer 
char* load_html_error_page(char* html_error_code,char* path_to_html_file){
  FILE* html_file = fopen(path_to_html_file,"r");
  if(html_file!=NULL){
    char* http_message = build_header_error_message(html_error_code);
    if(http_message!=NULL){//only proceeds with creation of http_message if there is space for the header
      http_message= include_html_data(http_message,html_file);
    }
    fclose(html_file);
    return http_message;
  }else{
    printf("Path to html file not found. I cannot display the error page\n");
    return NULL;
  }
}
//return a dinamically allocated char* that contains http header
char* build_header_error_message(char* html_error_code){
  char v[]= "HTTP/1.1 ";//html header version 
  char nf[]="\r\n";//next header field indicator
  //403 Forbidden or similar messages are inside html_error_code
  char KA[] = "Connection: keep-alive\r\n";//KA = keep alive status 
  char ct[] = "Content-Type: text/html\r\n";//content type field
  //char cl[] = "Content-Length: 1600\r\n";//content lenght field 
  int hl = strlen(v)+2*strlen(nf)+strlen(html_error_code)+strlen(KA)+
           strlen(ct)+1;//+strlen(cl)+1;//total length of header string (including null terminating character)
  //example of http:
//"HTTP/1.1 403 Forbidden\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nContent-Length: 310\r\n\r\n"
  char* http_header = (char*)malloc(sizeof(char)*hl);
  if(http_header!=NULL){
    http_header = strcpy(http_header,v);
    http_header = strcat(http_header,html_error_code);
    http_header = strcat(http_header,nf);
    http_header = strcat(http_header,KA);
    http_header = strcat(http_header,ct);
  //  http_header = strcat(http_header,cl);
    http_header = strcat(http_header,nf);
  }else{
    printf("There is not enough memory to create http header\n");
  }
  return http_header;
}
//include_html_data expects a http_message containing the http header
// and a FILE* already opened 
//include_html_data should be internally called by load_html_error_page 
char* include_html_data(char* http_message,FILE* html_file){
  char c = fgetc(html_file);
  int http_message_size=strlen(http_message);//strlen reuturn will point to '\0' (and strlen()-1 will point to '\n')
  char* large_http_message;//to handle possible realloc() failures 
  while(c!=EOF){
    http_message_size++;
    large_http_message = (char*)realloc(http_message,http_message_size*sizeof(char));
    if(large_http_message!=NULL){//realloc succesful (with space to include the null terminating character)
      http_message = large_http_message;
      http_message[http_message_size-1] = c;//as strlen() pointed to '\0', http_message_size-1 will continue to point to '\0' in the first round if realloc was sucessful 
    }else{//there isn't enough memory to include the html data
      free(http_message);
      printf("There is not enough memory to create http  html data\n");
      return NULL;
    }
    c = fgetc(html_file);
  }
  http_message[http_message_size-1] = '\0';
  return http_message;
}
void header_content(char* buffer){
  char *buffer_copy = (char*)malloc(strlen(buffer)*sizeof(char) +1);//to not lose buffer when tokenizing 
  buffer_copy = strcpy(buffer_copy,buffer);
  char *header_lines = strtok(buffer_copy,"\r\n");
  while(header_lines!=NULL){
    
    printf("\nRead line: %s \n",header_lines);
    header_lines = strtok(NULL,"\r\n");
  }
  free(buffer_copy);
}

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

//int get_header_size(char* buffer){

//}

bool is_blacklisted(char* hostname){
  if((strstr(hostname,blacklistPointer)) != NULL)
    return true;
  else
    return false;
}

bool is_whitelisted(char* hostname){
  if((strstr(hostname,whitelistPointer)) != NULL)
    return true;
  else
    return false;
}

bool has_denied_terms(char* buffer){
  return false;
}

void log_entry(char* buffer){
  return;
}