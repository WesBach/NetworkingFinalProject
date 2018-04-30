#include "GameLobby.h"

GameLobby::GameLobby() {
	this->numCurPlayers = 0;
	this->hostName = "none";
	this->mapName = "none";
	this->gameMode = "none";
}

GameLobby::~GameLobby() {

}

//Name:		getNumOpenings
//Purpose:	return the number of openings left in the lobby.
//Return:	int
int GameLobby::getNumOpenings() {
	return numSpots - numCurPlayers;
}