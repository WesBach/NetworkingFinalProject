#include "CommunicationManager.h"
#include "Buffer.h"
#include "UserInfo.h"
#include "GameLobby.h"
#define HEADER_SIZE 8

CommunicationManager::CommunicationManager()
{
	this->theBuffer = new Buffer();
}

CommunicationManager::~CommunicationManager()
{
	delete this->theBuffer;
}

//Name:			sendToClient
//Purpose:		Sends the populated buffer to the client.
//Return:		void 
void CommunicationManager::sendToClient(UserInfo* theUser)
{
	int res = send(*theUser->userSocket, this->theBuffer->getBufferAsCharArray(), this->theBuffer->GetBufferLength(), 0);
	if (res == SOCKET_ERROR)
	{
		printf("Send failed with error: %ld\n", res);
	}
}

//Name:			sendToClient
//Purpose:		Populates the buffer with the message and sends it to the client.
//Return:		void 
void CommunicationManager::sendToClient(UserInfo* theUser, std::string & message, const int& messageId, int& packetLength)
{
	//create the new buffer with its new size(+8 is for leading two ints)
	this->theBuffer = new Buffer(packetLength + HEADER_SIZE);
	//add the packet length and id to the buffer
	this->theBuffer->WriteInt32BE(theBuffer->GetBufferLength());
	this->theBuffer->WriteInt32BE(messageId);
	//add the message to the buffer
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
	//creates a simple message with its length and message
	this->theBuffer->WriteInt32BE(message.size());
	this->theBuffer->WriteStringBE(message);
}

//Name:			sendToRoom
//Purpose:		Populates the buffer with the message and send the packet to each client in the lobby.
//Return:		void 
void CommunicationManager::sendToRoom(std::string& roomName, std::string& message) {
	std::vector<std::string> theMessages;
	theMessages.push_back(message);
	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		if (roomName == this->theLobbies[i]->lobbyName)
		{
			//send the message to every player in the lobby
			for (int clientIndex = 0; clientIndex < theLobbies[i]->thePlayers.size(); clientIndex++)
			{
				sendToClient(theLobbies[i]->thePlayers[clientIndex], message, 10, getPacketSize(theMessages));
			}
		}
	}
}

//Name:			sendToServer
//Purpose:		Populates the buffer with the message and send the packet to the server.
//Return:		void 
void CommunicationManager::sendToServer(SOCKET* theSocket) {
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
	std::vector<std::string> theMessages;
	theMessages.push_back(message);

	for (std::vector<GameLobby*>::iterator it = this->theLobbies.begin(); it < this->theLobbies.end(); it++)
	{
		if (roomName == (*it)->lobbyName)
		{
			//send the message to every player in the lobby
			for (int clientIndex = 0; clientIndex < (*it)->thePlayers.size(); clientIndex++)
			{
				sendToClient((*it)->thePlayers[clientIndex], message, 10, getPacketSize(theMessages));
			}
			//clear the vector
			(*it)->thePlayers.clear();
			//remove the lobby
			it = this->theLobbies.erase(it);
		}
	}
}

void CommunicationManager::recieveMessage(UserInfo& theUser) {

	theUser.userBuffer->clearBuffer();
	theUser.userBuffer->resizeBuffer(512);
	//for getting packet length
	std::vector<std::string> tempVector;

	int bytesReceived = recv(*theUser.userSocket, theUser.userBuffer->getBufferAsCharArray(), theUser.userBuffer->GetBufferLength() + 1, 0);
	if (bytesReceived >= 4)
	{
		//get the packet length
		int packetLength = theUser.userBuffer->ReadInt32BE();

		//if the entire packet arrived otherwise do nothing
		if (bytesReceived >= packetLength)
		{
			//read the rest of the info
			int commandId = theUser.userBuffer->ReadInt32BE();

			//clear the buffer and resize before populating it again
			this->theBuffer->clearBuffer();
			this->theBuffer->resizeBuffer(512);
			//set the read and write index 
			this->theBuffer->resetReadWriteIndex();

			if (commandId == 1) {
				//Register
				
				//get alld theinfo from the user buffer
				int requestId = theUser.userBuffer->ReadInt32BE();

				//add the request id to the users vector for later use
				theUser.requests.push_back(requestId);

				int emailLength = theUser.userBuffer->ReadInt32BE();
				std::string email = theUser.userBuffer->ReadStringBE(emailLength);
				int passLength = theUser.userBuffer->ReadInt32BE();
				std::string password = theUser.userBuffer->ReadStringBE(passLength);

				//write the data to the server buffer and send it 
				this->theBuffer->WriteInt32BE(packetLength);
				this->theBuffer->WriteInt32BE(commandId);
				this->theBuffer->WriteInt32BE(requestId);
				this->theBuffer->WriteInt32BE(emailLength);
				this->theBuffer->WriteStringBE(email);
				this->theBuffer->WriteInt32BE(passLength);
				this->theBuffer->WriteStringBE(password);
				//TODO:
				//Popualte the server socket immediately after connecting
				this->sendToServer(this->theServerSocket);
			}
			else if (commandId == 2) {
				//Authenticate
				//get all the info from the user buffer
				int requestId = theUser.userBuffer->ReadInt32BE();

				//add the request id to the users vector for later use
				theUser.requests.push_back(requestId);

				int emailLength = theUser.userBuffer->ReadInt32BE();
				std::string email = theUser.userBuffer->ReadStringBE(emailLength);
				int passLength = theUser.userBuffer->ReadInt32BE();
				std::string password = theUser.userBuffer->ReadStringBE(passLength);

				//write the data to the server buffer and send it 
				this->theBuffer->WriteInt32BE(packetLength);
				this->theBuffer->WriteInt32BE(commandId);
				this->theBuffer->WriteInt32BE(requestId);
				this->theBuffer->WriteInt32BE(emailLength);
				this->theBuffer->WriteStringBE(email);
				this->theBuffer->WriteInt32BE(passLength);
				//send the message
				this->sendToServer(this->theServerSocket);
			}
			else if (commandId == 3) {
				//Create the server with a name, mode,map and playernumber
				//get the map name
				int mapLength = theUser.userBuffer->ReadInt32BE();
				std::string map = theUser.userBuffer->ReadStringBE(mapLength);
				//get the game mode 
				int modeLength = theUser.userBuffer->ReadInt32BE();
				std::string gameMode = theUser.userBuffer->ReadStringBE(modeLength);
				//get the lobby name
				int lobbyLength = theUser.userBuffer->ReadInt32BE();
				std::string lobbyName = theUser.userBuffer->ReadStringBE(lobbyLength);
				//get the number of players allowed
				int playerNumLength = theUser.userBuffer->ReadInt32BE();
				std::string playerNum = theUser.userBuffer->ReadStringBE(playerNumLength);
				//create the lobby
				this->createLobby(&theUser, map, gameMode, lobbyName);
			}
			else if (commandId == 4) {
				//VIEW
				//get all the lobby info and send it back
				tempVector = this->getLobbyInfo();

				//write id and packet size
				this->theBuffer->WriteInt32BE(4);
				this->theBuffer->WriteInt32BE(getPacketSize(tempVector) + HEADER_SIZE);

				//write the lobby info
				for (int i = 0; i < tempVector.size(); i++)
				{
					this->theBuffer->WriteInt32BE(tempVector[i].size());
					this->theBuffer->WriteStringBE(tempVector[i]);
				}

				//send the lobby info back
				this->sendToClient(&theUser);
			}
			else if (commandId == 5)
			{
				//REFRESH
				tempVector = this->getLobbyInfo();

				//write id and packet size
				this->theBuffer->WriteInt32BE(4);
				this->theBuffer->WriteInt32BE(getPacketSize(tempVector) + HEADER_SIZE);

				//write the lobby info
				for (int i = 0; i < tempVector.size(); i++)
				{
					this->theBuffer->WriteInt32BE(tempVector[i].size());
					this->theBuffer->WriteStringBE(tempVector[i]);
				}

				//send the lobby info back
				this->sendToClient(&theUser);
			}
			else if (commandId == 6) {
				//JOIN
				//have the user leave the lobby it's currently in
				int lobbyLength = theUser.userBuffer->ReadInt32BE();
				std::string lobbyName = theUser.userBuffer->ReadStringBE(lobbyLength);
				this->joinLobby(&theUser, lobbyName);
			}
			else if (commandId == 7) {
				//have the user leave the lobby it's currently in
				this->leaveLobby(&theUser, theUser.currentLobby);
			}
			else if (commandId == 10)
			{
				//generic response from the auth server
				int requestId = theUser.userBuffer->ReadInt32BE();
				int responseLength = theUser.userBuffer->ReadInt32BE();
				std::string response = theUser.userBuffer->ReadStringBE(responseLength);
				//get the request id and find the corresponding user
				UserInfo* foundUser = getUserWithByRequestId(requestId);
				tempVector.push_back(response);
				//send the message
				this->sendToClient(foundUser, response, 10, getPacketSize(tempVector));
			}
		}
	}
	else if (bytesReceived == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {//print error message
		printf("receive failed with error: %s", WSAGetLastError());
	}
}

void CommunicationManager::joinLobby(UserInfo* theUser, std::string& lobbyName) {
	//search through the lobbies
	std::string theFullMessage = lobbyName + " is currently full!";
	std::string theFailureMessage = lobbyName + " does not exist!";
	std::string userJoinedTheRoom = theUser->userName + " joined the room!";
	std::string successfullyJoinedTheRoom = "You have successfully joined the room!";

	std::vector<std::string> theMessages;

	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		//find the lobby
		if (this->theLobbies[i]->lobbyName == lobbyName)
		{
			int openSpots = this->theLobbies[i]->getNumOpenings();
			if (openSpots > 0)
			{
				//send the message to the room saying the user joined it before they join so they dont receive the message
				this->sendToRoom(lobbyName, userJoinedTheRoom);
				//add the player to the lobby increment the counter
				this->theLobbies[i]->thePlayers.push_back(theUser);
				this->theLobbies[i]->numCurPlayers++;

				//send a success message to the user that joined the room
				theMessages.push_back(successfullyJoinedTheRoom);
				this->sendToClient(theUser, successfullyJoinedTheRoom, 10, this->getPacketSize(theMessages));
				return;
			}
			else {
				//send the failure message
				theMessages.push_back(theFullMessage);
				this->sendToClient(theUser, theFullMessage, 10, this->getPacketSize(theMessages));
				return;
			}
		}
	}

	//didnt find the lobby 
	theMessages.push_back(theFailureMessage);
	this->sendToClient(theUser, theFailureMessage, 10, this->getPacketSize(theMessages));
}

void CommunicationManager::leaveLobby(UserInfo* theUser, std::string& lobbyName) {
	std::string tempMessage = "User " + theUser->userName + " has left the lobby!";
	std::vector<std::string> theMessages;
	theMessages.push_back(tempMessage);
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
				sendToClient(this->theLobbies[i]->thePlayers[playerIndex], tempMessage, 10, getPacketSize(theMessages));
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
	std::vector<std::string> theMessages;
	theMessages.push_back(lobbyExists);
	//check to see if the lobby name is already taken
	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		if (theLobby->lobbyName == this->theLobbies[i]->lobbyName)
		{
			//lobby already exists
			sendToClient(theUser, lobbyExists, 10, getPacketSize(theMessages));
			return;
		}
	}

	//lobby doesnt exist so create it
	this->theLobbies.push_back(theLobby);
	theLobby->numCurPlayers++;
}

std::vector<std::string> CommunicationManager::getLobbyInfo() {
	std::string lobbyInfo = "";
	std::vector<std::string> theLobbyInfo;
	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		lobbyInfo = "//";
		//Id	Map Name	Lobby Name			Game Mode		Players		Host
		//3		New York	Test your might!	Free For All	2|6			sous_chief
		lobbyInfo = this->theLobbies[i]->mapName;
		lobbyInfo += " || " + this->theLobbies[i]->lobbyName;
		lobbyInfo += " || " + this->theLobbies[i]->gameMode;
		lobbyInfo += " || " + this->theLobbies[i]->numCurPlayers;
		lobbyInfo += "/" + this->theLobbies[i]->numSpots;
		lobbyInfo += " || " + this->theLobbies[i]->hostName;
		lobbyInfo += "//";
		theLobbyInfo.push_back(lobbyInfo);
	}
	return theLobbyInfo;
}


//Name:			getPacketSize
//Purpose:		Determines the final size of the packet based on its items. With pre determined lenght prefixing.
//Return:		int&
int& CommunicationManager::getPacketSize(std::vector<std::string> theMessage) {
	int tempSize = 0;

	//accumulate all the sizes 
	for (int i = 0; i < theMessage.size(); i++)
	{
		tempSize += theMessage[i].size();
	}

	//get the sizes for the integers written in between as well
	tempSize += theMessage.size() * 4;
	return tempSize;
}

UserInfo* CommunicationManager::getUserWithByRequestId(int id)
{
	//go through the users
	for (int i = 0; i < this->theUsers.size(); i++)
	{
		//go through the currernt request vector to find the matching id
		for (int requestIndex = 0; requestIndex < this->theUsers[i]->requests.size(); i++)
		{
			//if theres a match return the current user
			if (this->theUsers[i]->requests[requestIndex] == id)
				return this->theUsers[i];
		}
	}
	//if the request isn't found
	return new UserInfo();
}