/*
*
* UnB - Universidade de Brasilia
* Trabalho 1 - Implementação de uma aplicação de Proxy Server
* Disciplina: Teleinformática e Redes 2
* Turma: A
* Professor: Joao Jose Costa Gondim
* Alunos: Vitor Satoru Machi Matsumine, Pedro Aurélio Coelho de Almeia, Jonas Prado Soares
* Matriculas: 14/0033262, 14/0158103, 13/0117277
*
 */

#include "server.h"
#include "client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

  long int temp,port = 33333;
  int return_flag;

  if(argc < 2){
    printf("[!] More arguments are needed, exiting...\n");
    return -1;
  }
  
  if(argc > 2){
    printf("[!] Too many arguments (max = 1), exceeding arguments were ignored\n");
  }

  temp = strtol(argv[1],NULL,0);

  if(temp < 1025){
    printf("[!] Invalid port, default port (33333) was selected\n");
  }
  else{
    port=temp;
  }

  printf("[.] Operating mode: server\n");
  printf("[.] Port: %li\n",port);
  printf("[.] Protocol: TCP\n");
  printf("------------------------------------\n");
  return_flag = run_tcp_server(port);

  printf("[*] Program exited with flag: %d\n",return_flag);
  return return_flag;

//spinning_thing(1);
}
