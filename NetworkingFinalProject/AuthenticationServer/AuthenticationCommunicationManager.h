#ifndef _AuthenticationCommunicationManager_HG_
#define _AuthenticationCommunicationManager_HG_
#include <string>

class UserInfo;
class Buffer;
class SQLManager;

class AuthenticationCommunicationManager {
public:
	AuthenticationCommunicationManager();
	~AuthenticationCommunicationManager();

	Buffer* theBuffer;
	SQLManager* theSQLManager;

	void receiveMessage(UserInfo* theUser);
	void sendMessage(UserInfo* theUser);

	std::pair<bool,std::string> registerUser(std::string& email, std::string& password);
	std::pair<bool, std::string> authenticateUser(std::string& email, std::string& password);
};
#endif // !_AuthenticationCommManager_HG_
