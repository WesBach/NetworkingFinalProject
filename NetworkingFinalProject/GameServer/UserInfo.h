#ifndef _UserInfo_HG_
#define _UserInfo_HG_

#include <WinSock2.h>
#include <vector>
class Buffer;
class GameLobby;

class UserInfo {
public:
	UserInfo();
	~UserInfo();
	SOCKET userSocket;
	Buffer* userBuffer;
	Buffer* carryOverBuffer;
	//holds the current requests for the user
	std::vector<int> requests;

	bool isLobbyHost;
	//std::string currentLobby;
	GameLobby* currentLobby;
	std::string userName;

	bool findRequestId(int& id);
	bool removeRequest(int& id);
};
#endif // !_UserInfo_HG_
