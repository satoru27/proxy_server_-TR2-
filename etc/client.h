#include "common.h"

int run_tcp_client(char *host, long int port);

bool check_end(char* buffer);

int get_length(char* buffer);

bool have_content(char* buffer);