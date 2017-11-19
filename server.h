#include "common.h"

int run_tcp_server(long int port);


int check_end(char* buffer);
int get_length(char* buffer);

char* get_final_host(char* buffer);
bool close_connection(char* buffer);