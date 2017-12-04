#include "../include/blacklist.h"
#include "../include/common.h"
#include <time.h>

deniedTerm* firstTerm = NULL;

struct deniedTerm{ //deny_term
    char* content; //texto do denyTerm
    struct deniedTerm* next; //proximo term, caso exista
};

void timestamp(FILE* fp)
{
    time_t ltime; /* calendar time */
    ltime=time(NULL); /* get current cal time */
    fprintf(fp, "%s",asctime( localtime(&ltime)));
}

int verifyGET(const char* buffer){
	//Returns 1 case in whitelist
	//Returns -1 case in blacklist
	//Returns 0 otherwise

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
		if(newbuffer==NULL)
			printf("newbuffer null!\n");

		printf("url==NULL and newbuffer=%s\n",newbuffer );
		return something_else;
	}

	FILE* fwhite;
	FILE* fblack;
	fwhite = fopen ("proxy_server_-TR2-/whitelist.txt", "r");
	fblack = fopen ("proxy_server_-TR2-/blacklist.txt", "r");
	char line[256];
	
	if (fwhite == NULL) { //arquivo nao existe
		printf("Whitelist not found\n");
	}
	else { //verifying whitelist
		while (fgets(line, sizeof(line), fwhite) && strlen(line) > 1) {
				int size = strlen (line);
				line[size-1] = '\0'; //apagar o \n do fgets
				//printf("%s\n", line);
				if (strstr(url,line) && size > 0) { //TERM FOUND AND VALID
					printf("Term in whitelist: %s\n", line);
					writeLogWhitelist(buffer);
					free(newbuffer);
					return whitelisted;
				}
		}
		fclose (fwhite);
	}

	if (fblack == NULL) {
		printf("Blacklist not found\n");
	}
	else {
		while (fgets(line, sizeof(line), fblack) && strlen(line) > 1) {
			int size = strlen(line);
			//printf("Initial blackterm #%s#\n", line);
			while (line[size]=='\0' || line[size]=='\n') { //enquanto houver finalizador invalido
				size--;
			}
			line[size+1] = '\0';
			//printf("Final blackterm #%s#\n", line);
			if (strstr(url, line)) {
				printf ("Term in blacklist: %s\n", line);
				writeLogBlacklist(buffer);
				free(newbuffer);
				return blacklisted;
			}
		}
		fclose (fblack);
	}

	free(newbuffer);
	return something_else;
}
char* save_deny_term_log(char* buffer){
//It looks like strcpy REALLY NEEDS to put the final '\0', so let's allocate space for that
	char* buffer_copy = (char*)malloc(sizeof(char)*(strlen(buffer)+1) );
	if(buffer_copy!=NULL){
		buffer_copy=strcpy(buffer_copy,buffer);
	}
	return buffer_copy;
}

void loadDenyTerms() {
	FILE* fterms;
	fterms = fopen ("proxy_server_-TR2-/denyTerms.txt", "r");
	char line[1500]; //variable to current denyterm
	deniedTerm* tempTerm = NULL;
	bool first = true;

	if (fterms == NULL) { //arquivo nao existe
		printf("Deny Terms file not found\n");
	} else {
		firstTerm = (deniedTerm*) malloc(sizeof(deniedTerm)); //cria o primeiro termo
		init_deniedTerm(firstTerm);
		tempTerm = firstTerm; //variavel auxiliar do iterador

		while (fgets(line, sizeof(line), fterms) && strlen(line) > 1) {
			int size = strlen (line);
			line[size-1] = '\0'; //apagar o \n capturado pelo fgets
			tempTerm->content = (char*) malloc(sizeof(char)*(size+1));

			strncpy(tempTerm->content,line, size); //copia line para o termo
			printf("\nDENY TERM %s LOADED", tempTerm->content);
			tempTerm->next = (deniedTerm*) malloc(sizeof(deniedTerm)); //cria o termo da proxima iteracao
			init_deniedTerm(tempTerm->next);
			tempTerm = tempTerm->next;
		}
	}

	printf("\n\nDenied terms loaded\n");
	fclose (fterms);
}

int verifyDenyTerms(const char* buffer, char* log_content) {
	/*returns -1 in case of deny term
	  returns  1 otherwise*/
	
	deniedTerm* tempTerm = firstTerm;

	while (tempTerm->content != NULL) {

		if (strstr(buffer,tempTerm->content)) { //TERM FOUND IN BUFFER
		printf("\n\n\n\nDeny Term found: %s\n\n\n", tempTerm->content);
		writeLogDeniedTerms(log_content,tempTerm->content);
		return denied_term;
		}
		else {
			printf("Deny Term %s not found\n", tempTerm->content);
		}
		tempTerm = tempTerm->next;
	}

	printf("Clean buffer: No deny terms\n\n");
	return something_else;
}

void writeLogDeniedTerms(char* log_content,char* term){
	FILE* logDenied;
	logDenied = fopen ("proxy_server_-TR2-/logs/logDenied.txt", "a"); //Appends to a file. Writing operations append data at the end of the file. The file is created if it does not exist.
	if (logDenied == NULL) {
		printf("Error in logBlack file\n");
	}else{
		timestamp(logDenied);
		fprintf(logDenied, "[C] The following request: %s", log_content);
		fprintf(logDenied, "Contains denied term: %s\n\n",term );
		fclose(logDenied);
	}
}
void writeLogBlacklist(const char* buffer) {
	FILE* logBlack;
	logBlack = fopen ("proxy_server_-TR2-/logs/logBlack.txt", "a"); //Appends to a file. Writing operations append data at the end of the file. The file is created if it does not exist.
	if (logBlack == NULL) {
		printf("Error in logBlack file\n");
	}else{
		timestamp(logBlack);
		fprintf(logBlack, "[C] Client tried to request blacklisted URL: %s", buffer);
		fclose(logBlack);
	}
}

void writeLogWhitelist(const char* buffer) {
	FILE* logWhite;
	logWhite = fopen ("./proxy_server_-TR2-/logs/logWhite.txt", "a"); //Appends to a file. Writing operations append data at the end of the file. The file is created if it does not exist.
	if (logWhite == NULL) {
		printf("Error in logWhite file\n");
	}else{
		timestamp(logWhite);
		fprintf(logWhite, "[C] Client tried to request whitelisted URL: %s", buffer);
		fclose(logWhite);	
	}
	
}

void init_deniedTerm(deniedTerm* dterm){
    dterm->content = NULL;
    dterm->next = NULL;
    return;
}