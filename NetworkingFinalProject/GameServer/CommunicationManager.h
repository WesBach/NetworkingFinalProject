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
	
	void sendToClient(UserInfo* theUser, std::string& message);
	void sendToRoom(std::string& roomName, std::string& message);
	void receiveFromClient();
	void sendToServer(SOCKET* theSocket, std::string& message);
	void closeRoom(std::string& roomName);
	void receiveFromServer();

	void createMessage(std::string& message);
};

#endif // !_Communication_Manager_HG_

