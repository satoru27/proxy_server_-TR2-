#include "common.h"

typedef struct deniedTerm deniedTerm;

int verifyGET(const char* buffer);
char* save_deny_term_log(char* buffer);
int verifyDenyTerms(const char* buffer,char* log_content);
void writeLogDeniedTerms(char* log_content, char* term);
void writeLogBlacklist(const char* buffer);
void writeLogWhitelist(const char* buffer);
void init_deniedTerm(deniedTerm* dterm);
void loadDenyTerms();