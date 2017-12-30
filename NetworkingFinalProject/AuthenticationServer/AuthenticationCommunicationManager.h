#ifndef _AuthenticationCommunicationManager_HG_
#define _AuthenticationCommunicationManager_HG_


class UserInfo;
class Buffer;

class AuthenticationCommunicationManager {
public:
	AuthenticationCommunicationManager();
	~AuthenticationCommunicationManager();

	Buffer* theBuffer;


	void receiveMessage(UserInfo* theUser);
	void sendMessage(UserInfo* theUser);
};
#endif // !_AuthenticationCommManager_HG_
