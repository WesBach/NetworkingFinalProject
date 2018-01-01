#include "OpenSSLUtilities.h"
#include <openssl\conf.h>
#include <openssl\evp.h>
#include <openssl\err.h>
#include <openssl\sha.h>
#include <cstdlib>
SHA256_CTX ctx;

static const char alphaNumeric[] =
"0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";

std::string OpenSSLUtilities::hashPassword(const char * pass)
{
		//use sha_256 to generate a hash of the passed in string
		unsigned char digest[SHA256_DIGEST_LENGTH];
	
		//SHA256((unsigned char*)&pass, strlen(pass), (unsigned char*)&digest);
	
		SHA256_Init(&ctx);
		SHA256_Update(&ctx, pass, strlen(pass));
		SHA256_Final(digest, &ctx);
	
		char mdString[SHA256_DIGEST_LENGTH * 2 + 1];
		for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
			sprintf(&mdString[i * 2], "%02x", (unsigned int)digest[i]);
	
		//return the hash
		return mdString;
}

std::string OpenSSLUtilities::getSalt() {
	std::string tempStr;
	for (unsigned int i = 0; i < 20; ++i)
	{
		tempStr += getRandCharacerFromAlphaNumeric();
	}
	return tempStr;
}

char OpenSSLUtilities::getRandCharacerFromAlphaNumeric()
{
	int stringLength = sizeof(alphaNumeric) - 1;
	return alphaNumeric[rand() % stringLength];
}