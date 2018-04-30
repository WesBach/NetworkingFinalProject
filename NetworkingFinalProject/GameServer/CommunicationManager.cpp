#include "CommunicationManager.h"
#include "Buffer.h"
#include "UserInfo.h"
#include "GameLobby.h"
#define HEADER_SIZE 8
extern fd_set master;

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
	int res = send(theUser->userSocket, this->theBuffer->getBufferAsCharArray(), this->theBuffer->GetBufferLength() + 1, 0);
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

	int res = send(theUser->userSocket, this->theBuffer->getBufferAsCharArray(), this->theBuffer->GetBufferLength(), 0);
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
	int packetLength = getPacketSize(theMessages);

	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		if (roomName == this->theLobbies[i]->lobbyName)
		{
			//send the message to every player in the lobby
			for (int clientIndex = 0; clientIndex < theLobbies[i]->thePlayers.size(); clientIndex++)
			{
				sendToClient(theLobbies[i]->thePlayers[clientIndex], message, 10, packetLength);
			}
		}
	}
}

//Name:			sendToServer
//Purpose:		Populates the buffer with the message and send the packet to the server.
//Return:		void 
void CommunicationManager::sendToServer(SOCKET* theSocket) {
	int res = send(*theSocket, this->theBuffer->getBufferAsCharArray(), this->theBuffer->GetBufferLength()+ 1, 0);
	if (res == SOCKET_ERROR)
	{
		printf("Send failed with error: %ld\n", res);
	}
}

//Name:			closeRoom
//Purpose:		Given a lobby name a message of the host leaving will be sent to each client. Then the lobby will be removed.
//Return:		void
void CommunicationManager::closeLobby(std::string& roomName) {
	std::string message = "The host has left, You have been removed from the Lobby!";
	std::vector<std::string> theMessages;
	theMessages.push_back(message);
	int packetSize = getPacketSize(theMessages);

	for (std::vector<GameLobby*>::iterator it = this->theLobbies.begin(); it != this->theLobbies.end(); it++)
	{
		if (roomName == (*it)->lobbyName)
		{
			//send the message to every player in the lobby
			for (int clientIndex = 0; clientIndex < (*it)->thePlayers.size(); clientIndex++)
			{
				sendToClient((*it)->thePlayers[clientIndex], message, 10, packetSize);
			}
			//clear the vector
			(*it)->thePlayers.clear();
			//remove the lobby
			it = this->theLobbies.erase(it);
			break;
		}
	}
}

//Name:			recieveMessage	
//Purpose:		Receives a packet fo the current user,gets the message id and does the corresponding task.
//Return:		void
void CommunicationManager::recieveMessage(UserInfo& theUser) {

	theUser.userBuffer->clearBuffer();
	theUser.userBuffer->resizeBuffer(512);
	theUser.userBuffer->resetReadWriteIndex();
	//for getting packet length
	std::vector<std::string> tempVector;

	int bytesReceived = recv(theUser.userSocket, theUser.userBuffer->getBufferAsCharArray(), theUser.userBuffer->GetBufferLength() + 1, 0);
	if (bytesReceived >= 4)
	{
		//get the packet length
		int packetLength = theUser.userBuffer->ReadInt32BE();

		while (bytesReceived < packetLength) {
			bytesReceived += recv(theUser.userSocket, theUser.carryOverBuffer->getBufferAsCharArray(), theUser.carryOverBuffer->GetBufferLength(), 0);

			std::vector<char> tempCharVec = theUser.carryOverBuffer->getBuffer();
			std::vector<char> initialBuffer = theUser.userBuffer->getBuffer();

			for (int i = 0; i < tempCharVec.size(); i++)
			{
				initialBuffer.push_back(tempCharVec[i]);
			}
			//set the buffer to the new concatenated buffer and set the read index to the correct location
			theUser.userBuffer->setBuffer(initialBuffer);
			theUser.userBuffer->setReadIndex(8);
		}

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
				//get the request id
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
				//add the string to the vector in case the user already logged in
				std::string alreadyLoggedIn = "This user is already logged in!";
				tempVector.push_back(alreadyLoggedIn);
				int packetSize = getPacketSize(tempVector);
				//get all the info from the user buffer
				int requestId = theUser.userBuffer->ReadInt32BE();

				//add the request id to the users vector for later use
				theUser.requests.push_back(requestId);
				//get the info from the buffer
				int emailLength = theUser.userBuffer->ReadInt32BE();
				std::string email = theUser.userBuffer->ReadStringBE(emailLength);
				int passLength = theUser.userBuffer->ReadInt32BE();
				std::string password = theUser.userBuffer->ReadStringBE(passLength);

				//check to see if the user is already logged in.
				//(if it is send the message to the user trying to log in then return)
				for (int i = 0; i < theLoggedInUsers.size(); i++) {
					if (theLoggedInUsers[i]->userName == email)
					{
						//the user already exists
						this->sendToClient(&theUser, alreadyLoggedIn, 10, packetSize);
						return;
					}
				}

				//write the data to the server buffer and send it 
				this->theBuffer->WriteInt32BE(packetLength);
				this->theBuffer->WriteInt32BE(commandId);
				this->theBuffer->WriteInt32BE(requestId);
				this->theBuffer->WriteInt32BE(emailLength);
				this->theBuffer->WriteStringBE(email);
				this->theBuffer->WriteInt32BE(passLength);
				this->theBuffer->WriteStringBE(password);

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
				int playerLength = theUser.userBuffer->ReadInt32BE();
				int playerNum = theUser.userBuffer->ReadInt32BE();
				//create the lobby
				this->createLobby(&theUser, map, gameMode, lobbyName, playerNum);
			}
			else if (commandId == 4) {
				//VIEW
				//get all the lobby info and send it back
				tempVector = this->getLobbyInfo();

				//write id and packet size
				this->theBuffer->WriteInt32BE(getPacketSize(tempVector) + HEADER_SIZE);
				this->theBuffer->WriteInt32BE(3);

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
				this->theBuffer->WriteInt32BE(getPacketSize(tempVector) + HEADER_SIZE);
				this->theBuffer->WriteInt32BE(4);

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
				UserInfo* foundUser = getUserByRequestId(requestId);
				tempVector.push_back(response);
				//send the message
				this->sendToClient(foundUser, response, 10, getPacketSize(tempVector));
			}
			else if (commandId == 11)
			{
				//generic response from the auth server
				int requestId = theUser.userBuffer->ReadInt32BE();
				int responseLength = theUser.userBuffer->ReadInt32BE();
				std::string response = theUser.userBuffer->ReadStringBE(responseLength);
				int userNameLength = theUser.userBuffer->ReadInt32BE();
				std::string userName = theUser.userBuffer->ReadStringBE(userNameLength);

				//get the request id and find the corresponding user
				UserInfo* foundUser = getUserByRequestId(requestId);
				//populate the username
				foundUser->userName = userName;

				this->theLoggedInUsers.push_back(foundUser);
				//add 4 so that the length accounts for the integer in front of it
				responseLength += 4;
				//send the message
				this->sendToClient(foundUser, response, 10, responseLength);
			}
			else if (commandId == 12)
			{
				//generic response from the auth server
				int requestId = theUser.userBuffer->ReadInt32BE();
				int responseLength = theUser.userBuffer->ReadInt32BE();
				std::string response = theUser.userBuffer->ReadStringBE(responseLength);
				//get the request id and find the corresponding user
				UserInfo* foundUser = getUserByRequestId(requestId);
				//add 4 so that the length accounts for the integer in front of it
				responseLength += 4;
				//send the message
				this->sendToClient(foundUser, response, 10, responseLength);
			}
		}
	}
	else if (bytesReceived == 0) {
		//user has reset the connection
		//disconnect
		this->userDisconnected(theUser, "User: " + theUser.userName + " has disconnected and has been removed from the lobby!");
		FD_CLR(theUser.userSocket, &master);
	}
	else if (bytesReceived == 0 && WSAGetLastError() == WSAECONNRESET) {
		//user has reset the connection
		//disconnect
		this->userDisconnected(theUser, "User: " + theUser.userName + " has reset the connection and has been removed from the lobby!");
	}
	else if (bytesReceived == 0 && WSAGetLastError() == WSAENETRESET) {
		//user has reset the connection
		//disconnect Network dropped connection on reset.
		this->userDisconnected(theUser, "User: " + theUser.userName + "'s connection was dropped in network reset, and has been removed from the lobby!");
	}
	else if (bytesReceived == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {//print error message
		printf("receive failed with error: %s", WSAGetLastError());
	}
	
}


//Name:			joinLobby	
//Purpose:		Given a user and a lobby name, will put the user in the lobby unless the lobby doesn't exist or the user is already in.
//Return:		void
void CommunicationManager::joinLobby(UserInfo* theUser, std::string& lobbyName) {
	std::string theFullMessage = lobbyName + " is currently full!";
	std::string theFailureMessage = lobbyName + " does not exist!";
	std::string userJoinedTheRoom = theUser->userName + " joined the room!";
	std::string successfullyJoinedTheRoom = "You have successfully joined the room!";
	int packetLength = 0;
	std::vector<std::string> theMessages;

	//clear the buffer
	this->theBuffer->clearBuffer();
	this->theBuffer->resizeBuffer(512);
	this->theBuffer->resetReadWriteIndex();

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
				packetLength = this->getPacketSize(theMessages);
				this->sendToClient(theUser, successfullyJoinedTheRoom, 10, packetLength);

				//populate the users lobby
				theUser->currentLobby = this->theLobbies[i];
				return;
			}
			else {
				//send the failure message
				theMessages.push_back(theFullMessage);
				packetLength = this->getPacketSize(theMessages);
				this->sendToClient(theUser, theFullMessage, 10, packetLength);
				return;
			}
		}
	}

	//didnt find the lobby 
	theMessages.push_back(theFailureMessage);
	packetLength = this->getPacketSize(theMessages);
	this->sendToClient(theUser, theFailureMessage, 10, packetLength);
}


//Name:			leaveLobby	
//Purpose:		Given a user and lobby, checks to see if the user is in the lobby and removes the user.
//Return:		void
void CommunicationManager::leaveLobby(UserInfo* theUser, GameLobby* lobby) {
	std::string tempMessage = "User " + theUser->userName + " has left the lobby!";
	std::string hostLeft = "Host has left the Lobby! You have been removed !";
	std::vector<std::string> theMessages;
	int packetSize;
	bool removeLobby = false;
	GameLobby* theLobbyBeingUsed = new GameLobby();;

	//get the correct lobby
	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		if (this->theLobbies[i]->lobbyName == lobby->lobbyName)
		{
			//get the lobby
			theLobbyBeingUsed = this->theLobbies[i];
		}
	}

	//find the player
	for (std::vector<UserInfo*>::iterator it = theLobbyBeingUsed->thePlayers.begin(); it < theLobbyBeingUsed->thePlayers.end(); it++)
	{
		//if the player is the host and is leaving
		if (theLobbyBeingUsed->hostName == theUser->userName)
		{
			//push back the leavig message
			theMessages.push_back(hostLeft);
			//get the packet size
			packetSize = getPacketSize(theMessages);
			//send a message to all users in the lobby and close the lobby
			for (int pIndex = 0; pIndex < theLobbyBeingUsed->thePlayers.size(); pIndex++)
			{
				sendToClient(theLobbyBeingUsed->thePlayers[pIndex], tempMessage, 10, packetSize);
			}
			//indicate that the lobby needs to be removed
			removeLobby = true;
			break;
		}
		//if the player is here remove it
		if ((*it)->userSocket == theUser->userSocket)
		{
			it = theLobbyBeingUsed->thePlayers.erase(it);
			break;
		}
	}

	//if the host left remove the lobby
	if (removeLobby)
	{
		this->closeLobby(theLobbyBeingUsed->lobbyName);
	}
	else {
		//populate the messages vector
		theMessages.push_back(tempMessage);
		int packetSize = getPacketSize(theMessages);
		//send a message to the people in the lobby
		for (int playerIndex = 0; playerIndex < theLobbyBeingUsed->thePlayers.size(); playerIndex++)
		{
			sendToClient(theLobbyBeingUsed->thePlayers[playerIndex], tempMessage, 10, packetSize);
		}
	}

}

//Name:			createLobby	
//Purpose:		Creates a lobby with the mapname gamemode lobbyname and num players. Sets the user as the lobby leader.
//Return:			void
void CommunicationManager::createLobby(UserInfo* theUser, std::string& mapName, std::string& gameMode, std::string& lobbyName, int& numPlayers) {
	//populate the new lobby
	GameLobby* theLobby = new GameLobby();
	theLobby->gameMode = gameMode;
	theLobby->hostName = theUser->userName;
	theLobby->mapName = mapName;
	theLobby->lobbyName = lobbyName;
	theLobby->numSpots = numPlayers;

	std::string lobbyCreated = "Successfully created Lobby:" + lobbyName;
	std::string lobbyExists = "This lobby name is already taken! Please try again.";
	std::vector<std::string> theMessages;
	theMessages.push_back(lobbyExists);

	//clear the buffer
	this->theBuffer->clearBuffer();
	this->theBuffer->resizeBuffer(512);
	this->theBuffer->resetReadWriteIndex();
	//packet size
	int packetsize = getPacketSize(theMessages);
	//check to see if the lobby name is already taken
	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		if (theLobby->lobbyName == this->theLobbies[i]->lobbyName)
		{
			//lobby already exists
			sendToClient(theUser, lobbyExists, 10, packetsize);
			return;
		}
	}

	//lobby doesnt exist so create it
	theUser->currentLobby = theLobby;
	//add the creator to the lobby
	theLobby->thePlayers.push_back(theUser);
	//add the lobby
	this->theLobbies.push_back(theLobby);
	theLobby->numCurPlayers++;

	theMessages.clear();
	theMessages.push_back(lobbyCreated);
	packetsize = getPacketSize(theMessages);
	sendToClient(theUser, lobbyCreated, 10, packetsize);
}

//Name:			getLobbyInfo	
//Purpose:		Adds all individual lobby info as a string to a vector to be  sent.
//Return:		std::vector<std::string>
std::vector<std::string> CommunicationManager::getLobbyInfo() {
	std::string lobbyInfo = "";
	std::vector<std::string> theLobbyInfo;
	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		lobbyInfo = "//";
		lobbyInfo += this->theLobbies[i]->mapName;
		lobbyInfo += " || " + this->theLobbies[i]->lobbyName;
		lobbyInfo += " || " + this->theLobbies[i]->gameMode;
		lobbyInfo += " || " + std::to_string(this->theLobbies[i]->numCurPlayers);
		lobbyInfo += "/" + std::to_string(this->theLobbies[i]->numSpots);
		lobbyInfo += " || " + this->theLobbies[i]->hostName;
		lobbyInfo += "//,";
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

//Name:			getUserByRequestId	
//Purpose:		Gets a user by its corresponding request id.
//Return:		UserInfo*
UserInfo* CommunicationManager::getUserByRequestId(int id)
{
	//go through the users
	for (int i = 0; i < this->theUsers.size(); i++)
	{
		//go through the currernt request vector to find the matching id
		for (int requestIndex = 0; requestIndex < this->theUsers[i]->requests.size(); requestIndex++)
		{
			//if theres a match return the current user
			if (this->theUsers[i]->requests[requestIndex] == id)
				return this->theUsers[i];
		}
	}
	//if the request isn't found
	return new UserInfo();
}

//Name:			userDisconnected
//Purpose:		If a user has disconnected it will send a message to all other users in the same lobby with the reason.
//				If the user was the lobby leader the lobby will be closed and all users in the lobby will be notified.
//Return:		void
void CommunicationManager::userDisconnected(UserInfo& theUser, std::string reason) {
	std::string tempMessage = reason;
	std::string hostLeft = "The Host disconnected from the lobby! You have been removed!";
	std::vector<std::string> theMessages;
	int packetSize;
	bool removeLobby = false;
	GameLobby* theLobbyBeingUsed = new GameLobby();;

	//get the correct lobby
	for (int i = 0; i < this->theLobbies.size(); i++)
	{
		if (this->theLobbies[i]->lobbyName == theUser.currentLobby->lobbyName)
		{
			//get the lobby
			theLobbyBeingUsed = this->theLobbies[i];
		}
	}

	//find the player
	for (std::vector<UserInfo*>::iterator it = theLobbyBeingUsed->thePlayers.begin(); it < theLobbyBeingUsed->thePlayers.end(); it++)
	{
		//if the player is the host and is leaving
		if (theLobbyBeingUsed->hostName == theUser.userName)
		{
			//push back the leavig message
			theMessages.push_back(hostLeft);
			//get the packet size
			packetSize = getPacketSize(theMessages);
			//send a message to all users in the lobby and close the lobby
			for (int pIndex = 0; pIndex < theLobbyBeingUsed->thePlayers.size(); pIndex++)
			{
				sendToClient(theLobbyBeingUsed->thePlayers[pIndex], tempMessage, 10, packetSize);
			}
			//indicate that the lobby needs to be removed
			removeLobby = true;
			break;
		}
		//if the player is here remove it
		if ((*it)->userSocket == theUser.userSocket)
		{
			it = theLobbyBeingUsed->thePlayers.erase(it);
			break;
		}
	}

	//if the host left remove the lobby
	if (removeLobby)
	{
		this->closeLobby(theLobbyBeingUsed->lobbyName);
	}
	else {
		//populate the messages vector
		theMessages.push_back(tempMessage);
		int packetSize = getPacketSize(theMessages);
		//send a message to the people in the lobby
		for (int playerIndex = 0; playerIndex < theLobbyBeingUsed->thePlayers.size(); playerIndex++)
		{
			sendToClient(theLobbyBeingUsed->thePlayers[playerIndex], tempMessage, 10, packetSize);
		}
	}

}