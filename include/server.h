#include "common.h"

//test

int run_tcp_server(long int port, bool inspection_neeeded);

bool check_end(char* buffer);

int get_length(char* buffer);

void get_final_host();

bool close_connection(char* buffer);

bool have_content(char* buffer);

void send_denied_access_message(int type);

char* load_html_error_page(char* html_error_code,char* path_to_html_file);

char* build_header_error_message(char* html_error_code);

char* include_html_data(char* http_message,FILE* html_file);

void header_content(char* buffer);
//int get_header_size(char* buffer);

bool is_blacklisted(char* hostname);

bool is_whitelisted(char* hostname);

bool has_denied_terms(char* buffer);

void log_entry(char* buffer);

void client_host_communication(char* deny_terms_log_content, int blacklistOK, bool inspection_neeeded);

void connection_setup(long int port);

int client_connect();

void host_connect(int rw_flag);

void close_client_and_host_sockets();