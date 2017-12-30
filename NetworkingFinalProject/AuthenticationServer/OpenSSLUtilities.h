#ifndef _OpenSSLUtilites_HG_
#define _OpenSSLUtilites_HG_
#include <string>

static std::string hashPassword(const char* pass);
static std::string getSalt();
char getRandCharacerFromAlphaNumeric();

#endif // !_OpenSSLUtilites_HG_

