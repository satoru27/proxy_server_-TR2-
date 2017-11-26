#include "blacklist.h"

int verifyGET(const char* buffer){
	//Retorns 1 if its not a GET message
	//Returns 1 case in whitelist
	//Returns -1 case in blacklist

	/*Verifica primeiramente a whitelist, em seguida a blacklist*/
	
	char* newbuffer = (char*) calloc(BUFFER_SIZE, sizeof(char));
	
	memcpy(newbuffer,buffer,BUFFER_SIZE); //copy to 'url' to keep 'buffer' stable

	char* url = strstr(newbuffer, "GET ");

	if (url != NULL) {
		url += sizeof("GET ") - 1;
		char* end = strstr(url,"HTTP")-1;
		url[end-url] = '\0';
		//printf("GET's URL: %s\n",url);
	}
	else {
		printf("url==NULL and newbuffer=%s\n",newbuffer );
		return 1;
	}

	FILE* fwhite;
	FILE* fblack;
	fwhite = fopen ("./proxy_server_-TR2-/whitelist.txt", "r");
	fblack = fopen ("./proxy_server_-TR2-/blacklist.txt", "r");
	char line[256];
	
	if (fwhite == NULL) { //arquivo nao existe
		printf("Whitelist not found\n");
	}
	else { //verifying whitelist
		while (fgets(line, sizeof(line), fwhite)) {
			int size = strlen (line);
			line[size-1] = '\0'; //apagar o \n do fgets
			//printf("@%s@\n", line);
			if (strstr(url,line)) { //TERM FOUND
				printf("Term in whitelist: %s\n", line);
				free(newbuffer);
				return 1;
			}
		}
		fclose (fwhite);
	}

	if (fblack == NULL) {
		printf("Blacklist not found\n");
	}
	else {
		while (fgets(line, sizeof(line), fblack)) {
			int size = strlen(line);
			line[size-1] = '\0';
			//printf("#%s\n#", line);
			if (strstr(url, line)) {
				printf ("Term in blacklist: %s\n", line);
				free(newbuffer);
				return -1;
			}
		}
	}


	free(newbuffer);
	return 1;
}
