//#include "common.h"
//#include "server.h" //usado somente para testar
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define CACHE_LIMIT 100
#define SIZE_OF_BEGIN_CACHE_ENTRY 20
#define SIZE_OF_INDEX 7
#define SIZE_OF_END_CACHE_ENTRY 19
#define SIZE_OF_PACKET 8
#define SIZE_OF_CACHE_END 11



typedef struct cr cr;

typedef struct cd cd;

//extern cr cache[CACHE_LIMIT];

char* get_page_name(char* buffer, char* hostname);

void init_cache_reg();
void init_cache_data(cd* cache);

//itens da cache podem ser guardados em um vetor cr cache[n];
void new_cr_entry(int i, char* index_name);

void add_data_to_cd(int i, char* content_pointer, int size);

void write_cache();

void load_cache();

void show_cache();

int find_empty_cr_index();

bool is_in_cache(char* name);

void show_cached_index();

int find_cache_index(char* name);

void send_from_cache(int socket, char* name);

/*Codigo da funcao a seguir foi desenvolvido por Paul Hsieh como softaware livre e é utilizado dentro dos termos da sua licensa LGPL 2.1
	Disponivel em http://www.azillionmonkeys.com/qed/hash.html
*/
//uint32_t SuperFastHash (const char * data, int len);