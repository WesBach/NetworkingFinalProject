#ifndef _UserInfo_HG_
#define _UserInfo_HG_

#include <WinSock2.h>
#include <vector>
class Buffer;

class UserInfo {
public:
	UserInfo();
	~UserInfo();
	SOCKET* userSocket;
	SOCKET* sendSocket;
	Buffer* userBuffer;
	Buffer* carryOverBuffer;

	std::vector<int> requests;

	bool isLobbyHost;
	std::string currentLobby;
	std::string userName;

	bool findRequestId(int& id);
	bool removeRequest(int& id);
};
#endif // !_UserInfo_HG_
