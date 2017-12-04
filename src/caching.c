#include "../include/caching.h"

/*Codigo da funcao a seguir foi desenvolvido por Paul Hsieh como softaware livre e é utilizado dentro dos termos da sua licensa LGPL 2.1
	Disponivel em http://www.azillionmonkeys.com/qed/hash.html
*/
/*BEGIN*/
// #include "pstdint.h" /* Replace with <stdint.h> if appropriate */
// #undef get16bits
// #if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
//   || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
// #define get16bits(d) (*((const uint16_t *) (d)))
// #endif

// #if !defined (get16bits)
// #define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
//                        +(uint32_t)(((const uint8_t *)(d))[0]) )
// #endif

// uint32_t SuperFastHash (const char * data, int len) {
// uint32_t hash = len, tmp;
// int rem;

//     if (len <= 0 || data == NULL) return 0;

//     rem = len & 3;
//     len >>= 2;

//     /* Main loop */
//     for (;len > 0; len--) {
//         hash  += get16bits (data);
//         tmp    = (get16bits (data+2) << 11) ^ hash;
//         hash   = (hash << 16) ^ tmp;
//         data  += 2*sizeof (uint16_t);
//         hash  += hash >> 11;
//     }

//     /* Handle end cases */
//     switch (rem) {
//         case 3: hash += get16bits (data);
//                 hash ^= hash << 16;
//                 hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
//                 hash += hash >> 11;
//                 break;
//         case 2: hash += get16bits (data);
//                 hash ^= hash << 11;
//                 hash += hash >> 17;
//                 break;
//         case 1: hash += (signed char)*data;
//                 hash ^= hash << 10;
//                 hash += hash >> 1;
//     }

//     /* Force "avalanching" of final 127 bits */
//     hash ^= hash << 3;
//     hash += hash >> 5;
//     hash ^= hash << 4;
//     hash += hash >> 17;
//     hash ^= hash << 25;
//     hash += hash >> 6;

//     return hash;
// }
/*END*/

struct cr{ //cache_reg
    char* index; //nome da pagina requisitada
    struct cd* data; // aponta para os dados carregados em memória
};

struct cd{ //cached_data
    char* content; //basicamente o buffer recebido
    struct cd* next; //caso haja fragmentacao aponta para o proximo, assim mantem
                             //da mesma forma que foi enviado pelo servidor de origem (simplificacao)
};

//cr cache[CACHE_LIMIT];
cr cache[CACHE_LIMIT];

char* get_page_name(char* buffer, char* hostname){
    //pegar pagina
    char* beginning = strstr(buffer,"GET ");
    if(beginning == NULL){
        printf("[!] Not a GET request\n");
        return NULL;
    }
    beginning += strlen("GET ");
    char* end = strstr(buffer," HTTP");
    int size = end - beginning;
    char* temp = (char*)malloc(sizeof(char)*(size+2));
    strncpy(temp,beginning,size);
    temp[size] = '\0';

    //considerando que o hostname ja foi extraido pela funcao get_final_host

    char* fullpagename = (char *)malloc(sizeof(char)*(size + strlen(hostname) + 1));
    strncpy(fullpagename,hostname,strlen(hostname));
    fullpagename[strlen(hostname)] = '\0';
    strcat(fullpagename,temp);

    return fullpagename;
}

void init_cache_reg(){
    for(int i = 0; i < CACHE_LIMIT; i++){
        cache[i].index = NULL;
        cache[i].data = NULL;
    }
    return;
}

void init_cache_data(cd* cache){
    cache->content = NULL;
    cache->next = NULL;
    return;
}



//itens da cache podem ser guardados em um vetor cr cache[n];
void new_cr_entry(int i, char* index_name){
    printf("ENTROU0\n");
    int nameSize = strlen(index_name) + 1;
    printf("ENTROU1 - namesize %d\n", nameSize);
    printf("ENTROU2 - cache_index %s\n", cache[i].index);
    cache[i].index = (char*) malloc(sizeof(char) * nameSize); //ENCRENCA
    printf("ENTROU3 - namesize %d\n", nameSize);
    //cache[i].index = index_name;
    strcpy(cache[i].index,index_name);
    cache[i].data = (cd*) malloc(sizeof(cd));
    init_cache_data(cache[i].data);
    //printf("\n****TEST*****\n");
    //char* test = cache[i].data->content;
    return;
}

void add_data_to_cd(int i, char* content_pointer,int size){
    cd* position = cache[i].data;
    //printf("[%s]",position->content);
    while(position->content != NULL){//encontra a ultima posicao sem dados
        position = position->next;
        //printf("ENTROU_3\n");
    }
    position->content = (char*)malloc(size);
    strcpy(position->content,content_pointer);
    
    position->next = (cd*) malloc(sizeof(cd));
    init_cache_data(position->next);
    printf("Cache string [%s]\n",position->content);
    sleep(5);
    return;
}

void write_cache(){
    FILE *fp = NULL;
    
    /*if((fp = fopen("proxy_server_-TR2-/cache.txt","r+")) == NULL){
        printf("[!] Error opening the cache file\n");
        return; 
    }*/

    //utilizando o modo w+ os dados da cache antiga sao sobrescritos
    //isso é util no caso de que, caso a cache sempre seja carregada no inicio da execucao
    //nao haverao dados duplicados
	if((fp = fopen("proxy_server_-TR2-/cache.txt","w+")) == NULL){
		printf("[!] Error opening the cache file\n");
		return; 
    }   

    if(fseek(fp,0L,SEEK_END) != 0){
        printf("[!] File seek error");
        return;
    }

    if(ftell(fp) != 0){ //se nao e arquivo novo entao tem o <CACHE_END> no final
        if(fseek(fp,-(SIZE_OF_CACHE_END),SEEK_END) != 0){
            printf("[!] File seek error");
            return;
        }   
    }   

    for(int i=0; i < CACHE_LIMIT; i++){
        if(cache[i].index == NULL)
            break;

        fprintf(fp,"<BEGIN_CACHE_ENTRY>\n");    
        //timestamp
        time_t ltime; /* calendar time */
        ltime=time(NULL); /* get current cal time */
        fprintf(fp, "%s",asctime( localtime(&ltime)));
        cd* position = cache[i].data;
        fprintf(fp,"<INDEX>%s\n", cache[i].index);//colocar um size ? usando sizeof
        while(position != NULL){
            if(position->content != NULL)
                fprintf(fp, "<PACKET>%s\n", position->content);
            position = position -> next;
        }
        fprintf(fp,"<END_CACHE_ENTRY>\n\n");        

    }

    fprintf(fp,"<CACHE_END>");

    printf("[*] Cache saved to disk\n");
    fclose(fp); 
}

void load_cache(){
    FILE *fp = NULL;
    
    //utilizando o modo r+ os dados sao adicionados no final do txt existente
    if((fp = fopen("proxy_server_-TR2-/cache.txt","r+")) == NULL){
        printf("[!] Error opening the cache file\n");
        return; 
    }

    fseek(fp, 0, SEEK_END);
    //printf("LAST POSITION: %ld\n",(long int)fp);
    int size = ftell(fp); //get file size
    rewind(fp);

    char* file_buffer = (char *) malloc(sizeof(char) * size);
    if(file_buffer == NULL){
        printf("[!] Memory Error, cache load failed\n");
        return;
    }

    if(fread(file_buffer,1,size,fp) != size){
        printf("Reading error, cache load failed\n");
    }

    //printf("--BUFFER:\n %s \n--END\n", file_buffer);

    char* buffer_pointer = file_buffer;

    //char* buffer_end = strstr(file_buffer,"<CACHE_END>");

    char* temp_end = NULL;

    int i = 0;

    long int field_size = 0;

    char* packet_pointer = NULL;
    char* packet_pointer_2 = NULL;
    char* end_cache_entry_pointer = NULL;
    cd* cache_pointer = NULL;

    //while(((long int)buffer_pointer + SIZE_OF_END_CACHE_ENTRY) < (long int)buffer_end){
    while((buffer_pointer = strstr(buffer_pointer,"<INDEX>")) != NULL){
        //printf("ENTROU\n"); 
        buffer_pointer += SIZE_OF_INDEX;
        temp_end = strstr(buffer_pointer,"<PACKET>");
        field_size = (long int)temp_end - (long int)buffer_pointer - 2;
        cache[i].index = (char *)malloc(sizeof(char)*(field_size+2));
        strncpy(cache[i].index,buffer_pointer, field_size+1);
        
        //printf("[%s]\n", cache[i].index);
        
        packet_pointer = strstr(buffer_pointer,"<PACKET>");
        end_cache_entry_pointer = strstr(buffer_pointer,"<END_CACHE_ENTRY>");

        cache[i].data = (cd *)malloc(sizeof(cd));
        cache_pointer = cache[i].data;
        //printf("---------\n %s \n--------\n", buffer_pointer);
        while((packet_pointer != NULL) && (packet_pointer < end_cache_entry_pointer)){
            //printf("ENTROU2\n"); 
            packet_pointer += SIZE_OF_PACKET;
            //printf("packet_pointer = %ld\n", (long int)packet_pointer);
            packet_pointer_2 = strstr(packet_pointer,"<PACKET>");
            //printf("packet_pointer2 = %ld\n", (long int)packet_pointer_2);
            if((packet_pointer_2 > end_cache_entry_pointer) || packet_pointer_2 == NULL)
                packet_pointer_2 = end_cache_entry_pointer;
            //printf("packet_pointer2 [if] = %ld\n", (long int)packet_pointer_2);
            //printf("buffer_pointer = %ld\n", (long int)buffer_pointer);
            field_size = (long int)packet_pointer_2 - (long int)packet_pointer -2;
            //printf("[[%d]]\n",field_size);
            cache_pointer->content = (char *)malloc(sizeof(char)*(field_size+2));
            strncpy(cache_pointer->content,packet_pointer,field_size+1);

            //printf("[%s]\n",cache_pointer->content);

            cache_pointer->next = (cd *)malloc(sizeof(cd));
            init_cache_data(cache_pointer->next);
            cache_pointer = cache_pointer->next;
            packet_pointer = strstr(packet_pointer,"<PACKET>");
        }

        i++;
    }
    printf("[*] Cache copied to memory\n");
    fclose(fp);
}

void show_cache(){
    printf("[*] Showing cache contents\n");
    cd *pointer = NULL;
    for(int i = 0; i < CACHE_LIMIT; i++){
        if(cache[i].index == NULL){
            printf("[*] No more cache contents\n");
            break;
        }
        printf("Index: %s\n",cache[i].index);
        pointer = cache[i].data;
        do{
            printf("%s\n",pointer->content);
            pointer = pointer->next;
        }while(pointer->next != NULL);

        printf("\n___________________\n");
    }
}

int find_empty_cr_index(){
    int i = 0;
    while((cache[i].index != NULL) && i < CACHE_LIMIT)
        i++;
    if(i >= CACHE_LIMIT)
        return -1;
    else
        return i;
}

bool is_in_cache(char* name){
    for(int i = 0; i < CACHE_LIMIT; i++){
        if((cache[i].index != NULL)){
            printf("[DB] CACHED: %zu\n",strlen(cache[i].index)); //%zu pois strlen retorna tipo size_t
            printf("[DB] STRING: %zu\n",strlen(name));
            if((strcmp(cache[i].index,name) == 0))
                return true;
        }
    }
    return false;
}

void show_cached_index(){
    for(int i = 0; i< CACHE_LIMIT; i++){
        if(cache[i].index == NULL){
            printf("[Cache] NULL\n");
            break;
        }
        else{
            printf("[Cache] %s\n", cache[i].index);
        }
    }
}
int find_cache_index(char* name){
    for(int i = 0; i < CACHE_LIMIT; i++){
        if((cache[i].index != NULL)){
            //printf("[DB] CACHED: %d\n",strlen(cache[i].index));
            //printf("[DB] STRING: %d\n",strlen(name));
            if((strcmp(cache[i].index,name) == 0))
                return i;
        }
    }
    return -1;
}

void send_from_cache(int socket, char* name){
    int index_n = find_cache_index(name);
    
    if(index_n<0){
        printf("Deu ruim producao,vazei\n");
        return;
    }

    cd* position = cache[index_n].data;
    if(position  == NULL){
        printf("Deu ruim producao,vazei de novo\n");
        return;
    }

    int size = 0;
    while(position->content != NULL){
        printf("[DB] Enviado pela cache\n");
        int size = strlen(position->content) + 1;
        send(socket,position->content,size,0); //blocante
        position = position->next;
    }
    printf("[DB] Finalizado envio pela cache\n");

}