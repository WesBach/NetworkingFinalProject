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
	Buffer* carryOverBuffer;
	SOCKET* theServerSocket;
	
	//map of lobbies with the users
	std::vector<GameLobby*> theLobbies;
	std::vector<UserInfo*> theUsers;
	std::vector<UserInfo*> theLoggedInUsers;

	void createLobby(UserInfo* theUser,std::string& mapName, std::string& mode,std::string& gameMode, int& numPlayers);
	void joinLobby(UserInfo* theUser, std::string& lobbyName);
	void leaveLobby(UserInfo* theUser, GameLobby* lobby);
	void closeLobby(std::string& roomName);

	//void sendToClient(UserInfo* theUser, std::string& message);
	void sendToClient(UserInfo* theUser, std::string & message, const int& messageId, int& packetLength);
	void sendToClient(UserInfo* theUser);

	void sendToRoom(std::string& roomName, std::string& message);
	void sendToServer(SOCKET* theSocket);

	void recieveMessage(UserInfo& theUser);

	void createMessage(std::string& message);

	void userDisconnected(UserInfo& theUser,std::string reason);

	std::vector<std::string> CommunicationManager::getLobbyInfo();
	int& getPacketSize(std::vector<std::string> theMassage);
	UserInfo* CommunicationManager::getUserByRequestId(int id);
};

#endif // !_Communication_Manager_HG_

