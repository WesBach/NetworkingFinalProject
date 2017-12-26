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
	Buffer* userBuffer;
	std::vector<int> requests;

	bool isLobbyHost;

	bool findRequestId(int& id);
	bool removeRequest(int& id);
};
#endif // !_UserInfo_HG_