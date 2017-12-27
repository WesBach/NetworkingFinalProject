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

	void createLobby(UserInfo* theUser,std::string& mapName, std::string& mode,std::string& gameMode );
	void joinLobby(UserInfo* theUser, std::string& lobbyName);
	void leaveLobby(UserInfo* theUser, std::string& lobbyName);

	//void sendToClient(UserInfo* theUser, std::string& message);
	void sendToClient(UserInfo* theUser, std::string & message, const int& messageId, int& packetLength);
	void sendToRoom(std::string& roomName, std::string& message);
	void sendToServer(SOCKET* theSocket, std::string& message);

	void recieveMessage(UserInfo& theUser);

	void closeRoom(std::string& roomName);
	void createMessage(std::string& message);

	std::vector<std::string> CommunicationManager::getLobbyInfo();
	int& getPacketSize(std::vector<std::string> theMassage);
};

#endif // !_Communication_Manager_HG_

