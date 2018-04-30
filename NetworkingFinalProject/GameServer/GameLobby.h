#ifndef _GameLobby_HG_
#define _GameLobby_HG_
#include <string>
#include <vector>

class UserInfo;

class GameLobby {
public:
	GameLobby();
	~GameLobby();

	std::string lobbyName;
	std::string mapName;
	std::string gameMode;
	int numSpots = 0;
	int numCurPlayers;
	std::string hostName;
	int getNumOpenings();

	//vector or players
	std::vector<UserInfo*> thePlayers;
};
#endif // !_GameLobby_HG_

