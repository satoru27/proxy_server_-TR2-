#include "common.h"

int run_tcp_server(long int port);



int get_length(char* buffer);

char* get_final_host(char* buffer);
bool close_connection(char* buffer);

bool check_end(char* buffer);