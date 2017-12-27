#include "CommunicationManager.h"
#include "Buffer.h"
#include "UserInfo.h"
#include "GameLobby.h"

CommunicationManager::CommunicationManager()
{
	this->theBuffer = new Buffer();
}

CommunicationManager::~CommunicationManager()
{
	delete this->theBuffer;
}

//Name:			sendToClient
//Purpose:		Populates the buffer with the message and sends it to the client.
//Return:		void 
void CommunicationManager::sendToClient(UserInfo* theUser, std::string & message)
{
	createMessage(message);

	int res = send(*theUser->userSocket, this->theBuffer->getBufferAsCharArray(), this->theBuffer->GetBufferLength(), 0);
	if (res == SOCKET_ERROR)
	{
		printf("Send failed with error: %ld\n", res);
	}
}

//Name:			createMessage
//Purpose:		Populates the buffer with the message.
//Return:		void 
void CommunicationManager::createMessage(std::string& message) {
	this->theBuffer = new Buffer();
	this->theBuffer->WriteInt32BE(message.size());
	this->theBuffer->WriteStringBE(message);
}

//Name:			sendToRoom
//Purpose:		Populates the buffer with the message and send the packet to each client in the lobby.
//Return:		void 
void CommunicationManager::sendToRoom(std::string& roomName, std::string& message) {
	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		if (roomName == this->theLobbies[i]->lobbyName)
		{
			//send the message to every player in the lobby
			for (int clientIndex = 0; clientIndex < theLobbies[i]->thePlayers.size(); clientIndex++)
			{
				sendToClient(theLobbies[i]->thePlayers[clientIndex], message);
			}
		}
	}
}

//Name:			sendToServer
//Purpose:		Populates the buffer with the message and send the packet to the server.
//Return:		void 
void CommunicationManager::sendToServer(SOCKET* theSocket, std::string& message) {
	//TODO::
	//May have to change this for different behavior
	createMessage(message);

	int res = send(*theSocket, this->theBuffer->getBufferAsCharArray(), this->theBuffer->GetBufferLength(), 0);
	if (res == SOCKET_ERROR)
	{
		printf("Send failed with error: %ld\n", res);
	}
}

//Name:			closeRoom
//Purpose:		Given a lobby name a message of the host leaving will be sent to each client. Then the lobby will be removed.
//Return:		void
void CommunicationManager::closeRoom(std::string& roomName) {
	std::string message = "The host has left, You have been removed from the Lobby!";

	for (std::vector<GameLobby*>::iterator it = this->theLobbies.begin(); it < this->theLobbies.end(); it++)
	{
		if (roomName == (*it)->lobbyName)
		{
			//send the message to every player in the lobby
			for (int clientIndex = 0; clientIndex < (*it)->thePlayers.size(); clientIndex++)
			{
				sendToClient((*it)->thePlayers[clientIndex], message);
			}
			//clear the vector
			(*it)->thePlayers.clear();
			//remove the lobby
			it = this->theLobbies.erase(it);
		}
	}
}

void CommunicationManager::recieveMessage(UserInfo& theUser) {

	int bytesReceived = recv(*theUser.userSocket, this->theBuffer->getBufferAsCharArray(), this->theBuffer->GetBufferLength() + 1, 0);
	if (bytesReceived >= 4)
	{
		//get the packet length
		int packetLength = theUser.userBuffer->ReadInt32BE();

		//if the entire packet arrived otherwise do nothing
		if (packetLength == bytesReceived)
		{
			//read the rest of the info
			int commandLength = theUser.userBuffer->ReadInt32BE();
			std::string command = theUser.userBuffer->ReadStringBE(commandLength);

			if (command == "")
			{

			}
		}

	}
	else if (bytesReceived == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {//print error message
		printf("receive failed with error: %s", WSAGetLastError());
	}
}

void CommunicationManager::joinLobby(UserInfo* theUser, std::string& lobbyName) {
	//search through the lobbies
	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		//find the lobby
		if (this->theLobbies[i]->lobbyName == lobbyName)
		{
			int openSpots = this->theLobbies[i]->getNumOpenings();
			if (openSpots > 0)
			{
				//add the player to the lobby increment the counter
				this->theLobbies[i]->thePlayers.push_back(theUser);
				this->theLobbies[i]->numCurPlayers++;
			}
			else {
				//send the failure message
				sendToClient(theUser, lobbyName + " is currently full!");
			}

			//found the lobby no need to go any farther
			return;
		}
	}

	//didnt find the lobby 
	sendToClient(theUser, lobbyName + " does not exist!");
}

void CommunicationManager::leaveLobby(UserInfo* theUser, std::string& lobbyName) {
	std::string tempMessage = "User " + theUser->userName + " has left the lobby!";

	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		if (this->theLobbies[i]->lobbyName == lobbyName)
		{
			//find the player
			for (std::vector<UserInfo*>::iterator it = this->theLobbies[i]->thePlayers.begin(); it < this->theLobbies[i]->thePlayers.end(); i++)
			{
				//if the player is here remove it
				if ((*it)->userSocket == theUser->userSocket)
				{
					it = this->theLobbies[i]->thePlayers.erase(it);
					return;
				}
			}

			//send a message to the people in the lobby
			for (int playerIndex = 0; playerIndex < this->theLobbies[i]->thePlayers.size(); playerIndex++)
			{
				sendToClient(this->theLobbies[i]->thePlayers[playerIndex], tempMessage);
			}
		}
	}

}

void CommunicationManager::createLobby(UserInfo* theUser, std::string& mapName, std::string& lobbyName, std::string& gameMode) {
	//populate the new lobby
	GameLobby* theLobby = new GameLobby();
	theLobby->gameMode = gameMode;
	theLobby->hostName = theUser->userName;
	theLobby->mapName = mapName;
	theLobby->lobbyName = lobbyName;

	std::string lobbyExists = "This lobby name is already taken! Please try again.";

	//check to see if the lobby name is already taken
	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		if (theLobby->lobbyName == this->theLobbies[i]->lobbyName)
		{
			//lobby already exists
			sendToClient(theUser, lobbyExists);
			return;
		}
	}

	//lobby doesnt exist so create it
	this->theLobbies.push_back(theLobby);
	theLobby->numCurPlayers++;
}
