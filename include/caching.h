#include "common.h"
//#include "server.h" //usado somente para testar
#include <time.h>
#define CACHE_LIMIT 100
#define SIZE_OF_BEGIN_CACHE_ENTRY 20
#define SIZE_OF_INDEX 7
#define SIZE_OF_END_CACHE_ENTRY 19
#define SIZE_OF_PACKET 8
#define SIZE_OF_CACHE_END 11



typedef struct cr{ //cache_reg
	char* index; //nome da pagina requisitada
	struct cd* data; // aponta para os dados carregados em memória
}cr;

typedef struct cd{ //cached_data
	char* content; //basicamente o buffer recebido
	struct cd* next; //caso haja fragmentacao aponta para o proximo, assim mantem
							 //da mesma forma que foi enviado pelo servidor de origem (simplificacao)
}cd;

cr cache[CACHE_LIMIT];
//VARIAVEL GLOBAL CACHE DEFINIDA NO SERVER.C

char* get_page_name(char* buffer, char* hostname);

void init_cache_reg();

void init_cache_data(cd* cache);

//itens da cache podem ser guardados em um vetor cr cache[n];
void new_cr_entry(cr* reg, char* index_name);

void add_data_to_cd(cr* reg, char* content_pointer);

void write_cache();

void load_cache();

void show_cache();

int find_empty_cr_index();

bool is_in_cache(char* name);

/*Codigo da funcao a seguir foi desenvolvido por Paul Hsieh como softaware livre e é utilizado dentro dos termos da sua licensa LGPL 2.1
	Disponivel em http://www.azillionmonkeys.com/qed/hash.html
*/
//uint32_t SuperFastHash (const char * data, int len);