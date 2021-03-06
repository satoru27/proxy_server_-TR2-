#include "../include/server.h"
#include "../include/blacklist.h"
#include "../include/inspecao.h"
#include "../include/caching.h"

bool stop_receiving_denied_pages;
char buffer[BUFFER_SIZE];
char* destination_host = NULL;

int run_tcp_server(long int port, bool inspection_neeeded){
  //clientSocket and listenSocket are defined globally on common.h
  bool close_flag = false;
  int remaining_data = 0;
  bool first_message = true;
  int header_size = 0;
  int rw_flag = 0;
  //messages to be sent if there was some problem with the connection
  //char* forbidden = "HTTP/1.1 403 Forbidden\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nContent-Length: 310\r\n\r\n<!DOCTYPE html>\n<html lang=\"en\">\n<body>\n<div class=\"cover\"><h1>Access Denied <small>- Error 403</small></h1><p class=\"lead\">The access to the requested resource was blocked by the proxy.</p></div>\n</body>\n</html>\n";
  //proxy-finalhost communication structures
  char* urlSent = NULL;
  //bool  stop_receiving_denied_pages;//pages that have denied terms
  char *deny_terms_log_content=NULL;
  signal(SIGALRM,&timeout_error);//setting a signal for a possible timeout event
  signal(SIGALRM,&gtfo);
  //BEGIN TEST
  signal(SIGPIPE,SIG_IGN);//se existir uma tetativa de escrever no socket do cliente o SIGPIPE geralmente fecha o programa, esse comando implica em ignorar esse tipo de situacao
  //fazer um tratamento melhor, pegando esse signal e etc
  //END TEST
  printf("[.] RUNNING TCP SERVER\n");

  init_cache_reg();
  load_cache();
  show_cache();
  loadDenyTerms();
  
  printf("\nTecle enter para iniciar o proxy");
  getchar();

  for(;;){

    connection_setup(port);

    stop_receiving_denied_pages=false;

    for(;;){
      //Set socket to listen
      alarm(TIMEOUT);//alarm is set for every possible blocking call
      printf("[*] Waiting for connection... \n");
      listen(listenSocket,100); //set to 1 the maximum length to which the queue of pending connections for sockfd may grow

      //Accept new connection
      rw_flag = client_connect(); //conecta com o cliente e retorna flag que vai ser usada na escrita
      if(inspection_neeeded)
        if(inspectsHeader(buffer) )//tries to inspect the buffer
          if( recoverHeader(buffer) )//if the inspection was successfully written on a file, recover that file
            printf("[*] Buffer successfully inspected. New buffer is:\n%s\n",buffer);

      if (buffer[0] == '\0') {
        printf ("BUFFER VAZIO - NAO REALIZAR TRANSMISSAO\n");
        close(clientSocket);
        break;
      }
      deny_terms_log_content = save_deny_term_log(buffer); //prepares buffer in case of log request

      int blacklistOK = verifyGET(buffer); //returns 1 if whitelist; returns -1 if blacklist
      int denyTermOK = verifyDenyTerms(buffer, deny_terms_log_content);

      if(blacklistOK != blacklisted && denyTermOK != denied_term){ //request is clear
        //only contacts final host/client if website isn't blacklisted
        /*BEGIN - OPENING CONNECTION WITH FINAL HOST*/

        host_connect(rw_flag);

        /*END - CONNECTIONS SETUP*/
        
        //sending first message
        printf("[*] Writing request to the host\n");
        if((send(hostSocket,buffer,rw_flag,0)<0))
          handle_error("[!] write() failed");
        printf("\n[C] Wrote: %s\n",buffer);

        client_host_communication(deny_terms_log_content,blacklistOK,inspection_neeeded);

        //header_content(buffer);
        //first_message = true;

        if(deny_terms_log_content!=NULL){
          //printf("before free deny log\n");
          //printf("deny log: %s\n",deny_terms_log_content);
          //it seems that the double free or corruption bug was caused because deny_terms_log_content did not have the
          //space to receive strcpy's last '\0'
          free(deny_terms_log_content);
          //printf("after\n");
        }

      } else if (blacklistOK == blacklisted) {
        send_denied_access_message(blacklisted);
      } else {
        send_denied_access_message(denied_term);
      }

      close_client_and_host_sockets();
      write_cache();

      if(stop_receiving_denied_pages)
        break;
    }//for(;;) -> Set socket to listen
    close(listenSocket);//close the server socket
    printf("[*] Listening socket closed... Sleeping\n");
    if(stop_receiving_denied_pages)
      sleep(2);//to prevent pages from receiving the remaining pices of content

  }//for(;;)->CONNECTIONS SETUP
  return 0;
}
//int type will indicate if the request was in the blacklist
//or denied terms. As it is an integer parameter, the function 
//send_denied_access_message may accept more types of denials
//in the future 
void send_denied_access_message(int type){
//TODO: redirect client if denied term is found (current method of
// redirection only works for blacklisted requests)
  char* forbidden=NULL;
  if(type==blacklisted){
    forbidden = load_html_error_page("403 Forbidden","proxy_server_-TR2-/http_errors/blacklist.html");
    printf("Sorry, we do not allow access to this website\n"); 
  }else if(type==denied_term){
    forbidden = load_html_error_page("403 Forbidden","proxy_server_-TR2-/http_errors/deny_terms.html");;
    printf("Sorry, this website has denied terms\n");
  }
  if(forbidden!=NULL){
    send(clientSocket,forbidden,strlen(forbidden),0);//atencao: usar strlen e nao sizeof, sizeof retorna o tamanho do ponteiro 
    printf("Error message sent to client: %s\n",forbidden);  
    free(forbidden);
  }else{
    printf("forbidden null\n");
  }
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
      if(http_message != NULL)
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
  if(buffer_copy != NULL)
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

void get_final_host(){
  char* start = strstr(buffer,"Host: ");
  start += sizeof("Host: ") -1;
  char* end = strstr(start, "\r\n");
  int size = end - start;

  destination_host = (char *)malloc(sizeof(char)*size + 1);

  strncpy(destination_host,start,size);
  destination_host[size] = '\0';
  
  printf("[*] Destination host: [%s]\n",destination_host);
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
void client_host_communication(char* deny_terms_log_content, int blacklistOK, bool inspection_neeeded){
  gtfo_flag = false;
  /*BEGIN - CLIENT-HOST COMMUNICATION*/
  printf("********************************************\n");
  int total = 0;
  int packet = 1;
  int rw_flag_c_h = 0;
  int rw_flag_h_c = 0;
  bool send_response=true;
  char* pagename = NULL;
  bool is_cached = false;
  int available_cache_index = 0;
  alarm(10);

  available_cache_index = find_empty_cr_index();
  
  printf("[*] Hostname: %s\n",destination_host);
  pagename = get_page_name(buffer,destination_host);
  printf("[*] Pagename %s\n", pagename);

  do{         
    do{
      //printf("before bzero\n");
      bzero(buffer,BUFFER_SIZE);
      //printf("after bzero\n");
      //printf("before recv\n");
      rw_flag_h_c = recv(hostSocket,buffer,BUFFER_SIZE,MSG_DONTWAIT);
      //printf("after recv\n");
      //printf("outside if\n");
      //printf("LOOP: %d \r\n", rw_flag_h_c);
      if(!(rw_flag_h_c <=0)) {
        //printf("inside if\n");
        //if(buffer==NULL)
        //    printf("buffer null\n");
        //printf("[H] Wrote: %s\n",buffer);
        //printf("[H] Wrote\n",buffer);
        
        int denyTermOK;
        if(blacklistOK != whitelisted)
          denyTermOK = verifyDenyTerms(buffer,deny_terms_log_content);
        if(inspection_neeeded && !want_to_send_response() && packet==0)
          send_response=false;
        if(send_response){
          
          if(denyTermOK != denied_term){ //DenyTerm not found
            if(packet == 1 && pagename != NULL){
              //printf("[H] First packet content:\"\n%s\"\n", buffer);
              is_cached = is_in_cache(pagename);
              printf("[*] Cached: %d\n",is_cached);
              if(is_cached == false){
                //printf("\n******ENTROU AQUI\n");
                printf("[*] Writing data to cache\n");
                new_cr_entry(available_cache_index,pagename);
             //   getchar();
             //   getchar();
             //   printf("there\n");
                add_data_to_cd(available_cache_index,buffer,rw_flag_h_c);
             //   printf("here\n");
              }
              else{
                printf("[*] Data is cached\n");
              }

            }else{
              if(is_cached == false && pagename != NULL){ 
                printf("[*] Writing data to cache\n");
                add_data_to_cd(available_cache_index,buffer,rw_flag_h_c);
              }else{
                printf("[*] Data is cached\n");
              }
            }
            if(is_cached == false){ 
              send(clientSocket,buffer,rw_flag_h_c,MSG_DONTWAIT);
              packet ++; 
              total += rw_flag_h_c;
              printf("[H] Packet #%d . Wrote %d bytes on client socket so far\n", packet, total);
              alarm(2); 
            } else{
              send_from_cache(clientSocket, pagename);
            }
          }else { //DenyTerm found
            rw_flag_h_c = 0; //sair do loop de packets dessa requisição pois um denyterm foi encontrado
            gtfo_flag=true;
            stop_receiving_denied_pages=true;
          }
        }else{
          rw_flag_h_c = 0; //sair do loop de packets dessa requisição pois um denyterm foi encontrado
          gtfo_flag=true;
        }
      }
    } while((rw_flag_h_c > 0));
  } while(!(gtfo_flag));
  printf("----TOTAL = %d ----\n\n",total);

  //show_cache();
  write_cache();
  show_cached_index();
  //getchar();
  //sleep(8);
}

void connection_setup(long int port){
    /*BEGIN - CONNECTIONS SETUP*/
    struct sockaddr_in echoServerAddress; //local adress

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
}

int client_connect(){
    int rw_flag = 0;
    unsigned int clientLen; //length of client address data structure
    struct sockaddr_in echoClientAddress; //client address

    clientLen = sizeof(echoClientAddress);

    alarm(TIMEOUT);
    if((clientSocket = accept(listenSocket,(struct sockaddr *) &echoClientAddress,&clientLen)) < 0 )
      handle_error("[!] accept() failed");
    printf("[*] Connection accepted \n");
    printf("[*] Client socket created \n");

    printf("------------------------------------\n");
    bzero(buffer,BUFFER_SIZE);//clears the message buffer
    
    alarm(TIMEOUT);
    if((rw_flag = read(clientSocket,buffer,BUFFER_SIZE))<0)//read message sent from the client
      handle_error("[!] read() failed");
    printf("[S] Received the following message from client:\n %s", buffer);
    //header_content(buffer);
    //printf("[*] Sending to the final host\n");

    return rw_flag;
}

void host_connect(int rw_flag){
    //char* destination_host = NULL;
    char init_message[BUFFER_SIZE];
    struct hostent *final_host;
    struct sockaddr_in server_addr; //sock_addr não possui sin_family, sin_addr ou sin_port
    long int host_port = 80;
        
    bzero(init_message,BUFFER_SIZE);
    memcpy(init_message,buffer,BUFFER_SIZE);
        
    if((hostSocket = socket(AF_INET,SOCK_STREAM,0)) < 0)
    handle_error("[!] socket() failed\n");
    printf("[*] Host socket created \n");

    printf("[*] Extracting hostname\n");
    get_final_host();

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
}

void close_client_and_host_sockets(){
      printf("[*] Communication ended\n");
      printf("[*] Cleaning buffer\n");
      bzero(buffer,BUFFER_SIZE); 
        
      close(clientSocket);//close the client socket
      printf("[*] Client socket closed\n");

      close(hostSocket);//close the host socket
      printf("[*] Host socket closed\n");
      //system("clear");
}