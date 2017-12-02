#include "../include/inspecao.h"
#include <ncurses.h>

bool inspectsHeader(const char* buffer) {
	FILE* fpointer;
	char opcao[2];

	printf("%s\n[!!!] Deseja inspecionar cabeçalho? (S/N): ", buffer);
	
	scanf ("%s",opcao);
	//getchar();
	while(!valid_option(opcao[0])){
		getchar();
		printf("Please insert a valid option (S/N)\n");
		scanf ("%s",opcao);
	}
	if (opcao[0] == 's' || opcao[0] == 'S') {
		fpointer = fopen ("proxy_server_-TR2-/packet.txt", "w+");//creates the file if it doesn't exist
		if (fpointer == NULL)
			return false;
		fprintf(fpointer, "%.*s",BUFFER_SIZE, buffer);//writes a maximum of BUFFER_SIZE caracters. https://stackoverflow.com/questions/2239519/is-there-a-way-to-specify-how-many-characters-of-a-string-to-print-out-using-pri and http://www.cplusplus.com/reference/cstdio/fprintf/ (12/01/2017)
		fclose(fpointer);
		system("nano proxy_server_-TR2-/packet.txt"); //open buffer in nano to be modified
		return true;
	} else {
		system("rm -f proxy_server_-TR2-/packet.txt"); //removes the file to keep buffer untouched
		return false;
	}
}

bool recoverHeader(char* buffer) { //updates buffer with the file content
	
	FILE * pFile;
  	int sizeFile;
  	size_t result;
	pFile = fopen ("proxy_server_-TR2-/packet.txt", "rb");
	if(pFile!=NULL){
		/*File exists*/
		printf("BUFFER VELHO: %s/ACABOU\n", buffer);
		printf("Iniciar atualizacao do buffer\n");
		bzero(buffer,BUFFER_SIZE); //zerar buffer
		//SEEK_END may not  have real standard portability
		//as we have a constant size buffer, we will read the max size 
		//of buffer. It may be a good ideia to use while(!feof(pFile))
		//to read all the chars until end of file
		/*
		//obtain file size
		fseek (pFile , 0 , SEEK_END);
	  	sizeFile = ftell (pFile);
	  	rewind (pFile);
		*/
	  	/*copy to buffer*/
	  	result = fread (buffer,sizeof(char),(unsigned int)BUFFER_SIZE,pFile); //copy from the file to buffer

		printf("BUFFER NOVO: %s/ACABOU\n", buffer);
		fclose(pFile);
		system("rm -f packet.txt"); //removes the file to later iterations
		return true;
	}else{
		printf("File packet.txt not found. It's impossible to update buffer\n");
		return false;
	}
}
bool valid_option(char option){
	return (option=='s'|| option=='S' || 
			option=='n' || option=='N');
}