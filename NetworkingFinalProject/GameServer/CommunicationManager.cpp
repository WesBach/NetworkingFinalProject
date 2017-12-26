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

void CommunicationManager::sendToClient(UserInfo* theUser, std::string & message)
{
	createMessage(message);

	int res = send(*theUser->userSocket, this->theBuffer->getBufferAsCharArray(), this->theBuffer->GetBufferLength(), 0);
	if (res == SOCKET_ERROR)
	{
		printf("Send failed with error: %ld\n", res);
	}
}

void CommunicationManager::createMessage(std::string& message) {
	this->theBuffer = new Buffer();
	this->theBuffer->WriteInt32BE(message.size());
	this->theBuffer->WriteStringBE(message);
}

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

//Name::		closeRoom
//Purpose::		Given a lobby name a message of the host leaving will be sent to each client. Then the lobby will be removed.
//Return::		void
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

