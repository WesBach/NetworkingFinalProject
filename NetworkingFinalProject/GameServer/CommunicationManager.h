#ifndef _CommunicationManager_HG_
#define _CommunicationManager_HG_
#include <WinSock2.h>
#include <string>
#include <map>
#include <vector>

class UserInfo;
class Buffer;
class GameLobby;

class CommunicationManager {
public:
	CommunicationManager();
	~CommunicationManager();

	Buffer* theBuffer;

	//map of lobbies with the users
	std::vector<GameLobby*> theLobbies;

	void joinLobby(UserInfo* theUser, std::string& lobbyName);
	
	void sendToClient(UserInfo* theUser, std::string& message);
	void sendToRoom(std::string& roomName, std::string& message);

	void recieveMessage(UserInfo& theUser);

	void sendToServer(SOCKET* theSocket, std::string& message);
	void closeRoom(std::string& roomName);
	void createMessage(std::string& message);
	void sendJoinFailed(UserInfo* theUser, std::string& reason);
};

#endif // !_Communication_Manager_HG_

