#include "GameLobby.h"

GameLobby::GameLobby() {
	this->numCurPlayers = 0;
	this->hostName = "none";
	this->mapName = "none";
	this->gameMode = "none";
}

GameLobby::~GameLobby() {

}

int GameLobby::getNumOpenings() {
	return numSpots - numCurPlayers;
}