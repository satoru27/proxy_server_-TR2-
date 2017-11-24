#include "common.h"

void hash_cache_index(); //faz o hash da url do arquivo

void load_cache_index(); //carrega em memória os indices de cache

void save_cache_index(); //salva os indices de cache no disco



typedef struct cached_data{
	char* data; //basicamente o buffer recebido
	struct cached_data* next; //caso haja fragmentacao aponta para o proximo, assim mantem
							 //da mesma forma que foi enviado pelo servidor de origem (simplificacao)
};


typedef struct cache_reg{
	int index; //hashed index, obtido a partir de um hash simples do endereco que identifica a pagina
	struct cached_data* data; // aponta para os dados carregados em memória
};

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