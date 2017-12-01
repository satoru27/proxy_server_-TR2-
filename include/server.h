#include "common.h"
#include "caching.h"

int run_tcp_server(long int port);

bool check_end(char* buffer);

int get_length(char* buffer);

char* get_final_host(char* buffer);

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

void client_host_communication(char * buffer, char* deny_terms_log_content, int blacklistOK);

void connection_setup(long int port);