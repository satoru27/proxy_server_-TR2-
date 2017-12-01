#include "common.h"
//#include "server.h" //usado somente para testar

typedef struct cr{ //cache_reg
	char* index; //nome da pagina requisitada
	struct cd* data; // aponta para os dados carregados em memória
}cr;

typedef struct cd{ //cached_data
	char* content; //basicamente o buffer recebido
	struct cd* next; //caso haja fragmentacao aponta para o proximo, assim mantem
							 //da mesma forma que foi enviado pelo servidor de origem (simplificacao)
}cd;

void hash_cache_index(); //faz o hash da url do arquivo

void load_cache_index(); //carrega em memória os indices de cache

void save_cache_index(); //salva os indices de cache no disco

/*typedef struct cache_reg{
	int index; //hashed index, obtido a partir de um hash simples do endereco que identifica a pagina
	struct cached_data* data; // aponta para os dados carregados em memória
};*/


char* get_page_name(char* buffer, char* hostname);

void init_cache_reg(cr* cache, int index);

void init_cache_data(cd* cache);

void new_cr_entry(cr* reg, char* index_name);

void add_data_to_cd(cr* reg, char* content_pointer);


/*
Estrutura para guardar o arquivos (ignorar <>):
<hash referente ao arquivo>\n --- pegar usando fgets para simplificar ?
<1500 bytes fixos referentes ao pacote, independente do tamanho original> --- pegar diretamente por numero de bytes
<se houver mais dados referentes ao mesmo pacote coloca-los aqui no mesmo formato 1500 bytes>
<--END_PCKT--> //identificador de fim do pacote
<hash do outro arquivo> //inicia outro pacote
<data...>
<--END_PCKT-->
<--END_FILE--\0>
*/


/*Codigo da funcao a seguir foi desenvolvido por Paul Hsieh como softaware livre e é utilizado dentro dos termos da sua licensa LGPL 2.1
	Disponivel em http://www.azillionmonkeys.com/qed/hash.html
*/
//uint32_t SuperFastHash (const char * data, int len);
