/*
*
* UnB - Universidade de Brasilia
* Trabalho 1 - Implementação de uma aplicação de Proxy Server
* Disciplina: Teleinformática e Redes 2
* Turma: A
* Professor: Joao Jose Costa Gondim
* Alunos: Vitor Satoru Machi Matsumine, Pedro Aurélio Coelho de Almeia
* Matriculas: 14/0033262, 14/0158103
*
 */

#include "server.h"
#include "client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
//
//   char *mode,*protocol = "-u";
//   long int temp,port = 33333;
//   int return_flag;
//
//   switch(argc){
//
//     case 1:
//       printf("[!] More arguments are needed, exiting...\n");
//       return -1;
//
//     case 2:
//       /*
//       if(strcmp(argv[1],"-S")!=0 && strcmp(argv[1],"host")!=0){
//
//
//         printf("[!] Invalid mode, exiting...\n");
//         return -1;
//       }
//       */
//         mode=argv[1];
//         break;
//
//     case 3:
// /*
//       if(strcmp(argv[1],"-S")!=0 && strcmp(argv[1],"host")!=0 ){
//         printf("[!] Invalid mode, exiting...\n");
//         return -1;
//       }
// */
//       mode=argv[1];
//       temp = strtol(argv[2],NULL,0);
//       if(temp < 1025){
//         printf("[!] Invalid port, default port (33333) was selected\n");
//       }
//       else{
//       port=temp;
//       }
//       break;
//
//     default:
//       if(argc > 4){
//         printf("[!] Too many arguments (max = 3), exceeding arguments were ignored\n");
//       }
//       /*
//       if(strcmp(argv[1],"-S")!=0 && strcmp(argv[1],"host")!=0 ){
//         printf("[!] Invalid mode, exiting...\n");
//         return -1;
//       }
//       */
//       mode=argv[1];
//
//       temp = strtol(argv[2],NULL,0);
//
//       if(temp < 1025){
//         printf("[!] Invalid port, default port (33333) was selected\n");
//       }
//       else{
//       port=temp;
//       }
//
//       if(strcmp(argv[3],"-t")!=0 && strcmp(argv[3],"-u")!=0){
//         printf("[!] Invalid selected protocol, using default protocol (UDP)\n");
//       }
//       else{
//         protocol = argv[3];
//       }
//       break;
//
//     }
//
//   if(strcmp(mode,"-S")==0){
//     if(strcmp(protocol,"-t")==0){
//       printf("[.] Operating mode: server\n");
//       printf("[.] Port: %li\n",port);
//       printf("[.] Protocol: TCP\n");
//       printf("------------------------------------\n");
//       return_flag = run_tcp_server(port);
//     }
//     else{
//       printf("[.] Operating mode: server\n");
//       printf("[.] Port: %li\n",port);
//       printf("[.] Protocol: UDP\n");
//       printf("------------------------------------\n");
//       return_flag = run_udp_server(port);
//     }
//   }
//   else{
//     if(strcmp(protocol,"-t")==0){
//       printf("[.] Operating mode: client\n");
//       printf("[.] Port: %li\n",port);
//       printf("[.] Protocol: TCP\n");
//       printf("------------------------------------\n");
//       return_flag = run_tcp_client(mode,port);
//     }
//     else{
//       printf("[.] Operating mode: client\n");
//       printf("[.] Port: %li\n",port);
//       printf("[.] Protocol: UDP\n");
//       printf("------------------------------------\n");
//       return_flag = run_udp_client(mode,port);
//     }
//   }
//
//   printf("[*] Program exited with flag: %d\n",return_flag);
//   return return_flag;


spinning_thing(1);

}
