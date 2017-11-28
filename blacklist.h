#include "common.h"

int verifyGET(const char* buffer);
int verifyDenyTerms(const char* buffer);
void writeLogBlacklist(const char* buffer);
void writeLogWhitelist(const char* buffer);