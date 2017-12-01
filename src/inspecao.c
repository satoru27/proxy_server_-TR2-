#include "../include/inspecao.h"
#include <ncurses.h>

bool inspectsHeader(const char* buffer) {
	FILE* fpointer;
	char opcao;

	printf("%s\n[!!!] Deseja inspecionar cabeçalho? (S/N): ", buffer);
	
	scanf ("%c", &opcao);
	getchar();

	if (opcao == 's' || opcao == 'S') {
		fpointer = fopen ("packet.txt", "w");
		if (fpointer == NULL)
			return false;
		fprintf(fpointer, "%s", buffer);
		fclose(fpointer);
		system("nano packet.txt"); //open buffer in nano to be modified
		return true;
	} else {
		system("rm -f packet.txt"); //removes the file to keep buffer untouched
		return false;
	}
}

bool recoverHeader(char* buffer) { //updates buffer with the file content
	
	FILE * pFile;
  	int sizeFile;
  	size_t result;
	pFile = fopen ("packet.txt", "r");

	/*File exists*/
	printf("BUFFER VELHO: %s/ACABOU\n", buffer);
	printf("Iniciar atualizacao do buffer\n");
	bzero(buffer,BUFFER_SIZE); //zerar buffer
	
	//obtain file size
	fseek (pFile , 0 , SEEK_END);
  	sizeFile = ftell (pFile);
  	rewind (pFile);

  	/*copy to buffer*/
  	result = fread (buffer,sizeof(char),sizeFile,pFile); //copy from the file to buffer

	printf("BUFFER NOVO: %s/ACABOU\n", buffer);
	system("rm -f packet.txt"); //removes the file to later iterations
	return true;
}