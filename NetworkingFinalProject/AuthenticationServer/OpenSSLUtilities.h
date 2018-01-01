#ifndef _OpenSSLUtilites_HG_
#define _OpenSSLUtilites_HG_
#include <string>

namespace OpenSSLUtilities {
	std::string hashPassword(const char* pass);
	std::string getSalt();
	char getRandCharacerFromAlphaNumeric();
};


#endif // !_OpenSSLUtilites_HG_

