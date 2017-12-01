#include "../include/caching.h"

/*Codigo da funcao a seguir foi desenvolvido por Paul Hsieh como softaware livre e é utilizado dentro dos termos da sua licensa LGPL 2.1
	Disponivel em http://www.azillionmonkeys.com/qed/hash.html
*/
/*BEGIN*/
//#include "pstdint.h" /* Replace with <stdint.h> if appropriate */
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
    char* temp = (char*)malloc(sizeof(char)*(size+1));
    strncpy(temp,beginning,size);
    temp[size] = '\0';

    //considerando que o hostname ja foi extraido pela funcao get_final_host

    char* fullpagename = (char *)malloc(sizeof(char)*(size + strlen(hostname) + 1));
    strncpy(fullpagename,hostname,strlen(hostname));
    strcat(fullpagename,temp);

    return fullpagename;
}

void init_cache_reg(cr* cache, int index){
    for(int i = 0; i < index; i++){
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

void new_cr_entry(cr* reg, char* index_name){
    reg->index = index_name;
    reg->data = (cd*) malloc(sizeof(cd));
    init_cache_data(reg->data);
    return;
}

void add_data_to_cd(cr* reg, char* content_pointer){
    cd* position = reg->data;
    while(position->content != NULL){//encontra a ultima posicao sem dados
        position = position->next;
    }
    position->content = content_pointer;
    position->next = (cd*) malloc(sizeof(cd));
    init_cache_data(position->next);
    return;
}