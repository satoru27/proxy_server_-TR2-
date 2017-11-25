#include "common.h"

int run_tcp_server(long int port);

bool check_end(char* buffer);

int get_length(char* buffer);

char* get_final_host(char* buffer);

bool close_connection(char* buffer);

bool have_content(char* buffer);

void header_content(char* buffer);
//int get_header_size(char* buffer);

bool is_blacklisted(char* hostname);

bool is_whitelisted(char* hostname);

bool has_denied_terms(char* buffer);

void log_entry(char* buffer);