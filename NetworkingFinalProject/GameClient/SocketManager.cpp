#include "SocketManager.h"
#include "Utility.h"

SocketManager::SocketManager() {

}

SocketManager::~SocketManager() {

}

//1. Must use TCP as its primary communication method. (1 mark)
//2. Must use length - prefixing for message framing. (1 mark)
//3. Uses Binary for serialization. (2 marks)
//4. Network requests should be asynchronous, or run in a different thread. (2 marks)
//5. Deserialize using length - prefixing properly
//1. Handle multiple messages in one receive. (2 marks)
//2. Handles partially received messages(waits for more data
//	based on the length - prefix). (2 marks)
//3. Checks if there is enough data to read the header first. (2 marks)

void SocketManager::buildMessage(std::vector<std::string>& theMessage)
{
	//build the message in the buffer
	if (theMessage.size() > 0)
	{
		//id = 1
		if (theMessage[0] == "REGISTER" || theMessage[0] == "register")
		{
			//2. Register an account. (2 marks)
			//write packet size;
			this->theBuffer->WriteInt32BE(this->getPacketSize(theMessage));
			this->theBuffer->WriteInt32BE(1);
			this->theBuffer->WriteInt32BE(theMessage[0].size());
			this->theBuffer->WriteStringBE(theMessage[0]);
			this->theBuffer->WriteInt32BE(theMessage[1].size());
			this->theBuffer->WriteStringBE(theMessage[1]);
			this->theBuffer->WriteInt32BE(theMessage[2].size());
			this->theBuffer->WriteStringBE(theMessage[2]);
		}
		else if (theMessage[0] == "AUTHENTICATE" || theMessage[0] == "authenticate")
		{
			//id = 2
			//2. authenticate an account. (2 marks)
			this->theBuffer->WriteInt32BE(this->getPacketSize(theMessage));
			this->theBuffer->WriteInt32BE(2);
			this->theBuffer->WriteInt32BE(theMessage[0].size());
			this->theBuffer->WriteStringBE(theMessage[0]);
			this->theBuffer->WriteInt32BE(theMessage[1].size());
			this->theBuffer->WriteStringBE(theMessage[1]);
			this->theBuffer->WriteInt32BE(theMessage[2].size());
			this->theBuffer->WriteStringBE(theMessage[2]);
		}
		else if (theMessage[0] == "CREATE" || theMessage[0] == "create")
		{
			//id = 3
			//3. Create a game lobby. (2 marks)
			this->theBuffer->WriteInt32BE(this->getPacketSize(theMessage));
			this->theBuffer->WriteInt32BE(3);
			this->theBuffer->WriteInt32BE(theMessage[0].size());
			this->theBuffer->WriteStringBE(theMessage[0]);
			this->theBuffer->WriteInt32BE(theMessage[1].size());
			this->theBuffer->WriteStringBE(theMessage[1]);
		}
		else if (theMessage[0] == "VIEW" || theMessage[0] == "view")
		{
			//id = 4
			//4. View the game lobby list. (3 mark)
			this->theBuffer->WriteInt32BE(4);
			this->theBuffer->WriteInt32BE(theMessage[0].size());
			this->theBuffer->WriteStringBE(theMessage[0]);
		}
		else if (theMessage[0] == "REFRESH" || theMessage[0] == "refresh")
		{
			//id = 5
			//5. Refresh the game lobby list.This can be done automatically, or by client
			this->theBuffer->WriteInt32BE(5);
			this->theBuffer->WriteInt32BE(theMessage[0].size());
			this->theBuffer->WriteStringBE(theMessage[0]);
		}
		else if (theMessage[0] == "JOIN" || theMessage[0] == "join")
		{
			//id = 6
			//6. Join the game lobby
			this->theBuffer->WriteInt32BE(6);
			this->theBuffer->WriteInt32BE(theMessage[0].size());
			this->theBuffer->WriteStringBE(theMessage[0]);
			this->theBuffer->WriteInt32BE(theMessage[1].size());
			this->theBuffer->WriteStringBE(theMessage[1]);
		}
		else if (theMessage[0] == "LEAVE" || theMessage[0] == "leave")
		{
			//id = 7
			//7. Leave the game lobby
			this->theBuffer->WriteInt32BE(6);
			this->theBuffer->WriteInt32BE(theMessage[0].size());
			this->theBuffer->WriteStringBE(theMessage[0]);
		}
	}
}

void SocketManager::sendMessage()
{
	//use the internal buffer to send a message 
	int sendResult = send(*this->theSocket, this->theBuffer->getBufferAsCharArray(), this->theBuffer->GetBufferLength() + 1, 0);
	//check for error
	if (sendResult == SOCKET_ERROR)
	{
		print_text("Send failed with error: %s", sendResult);
	}
}

int& SocketManager::getPacketSize(std::vector<std::string> theMessage) {
	int commandLength = 0;
	int firstTextSize = 0;
	int secondTextSize = 0;
	int tempSize = 0;

	if (theMessage.size() == 2)
	{
		commandLength = theMessage[0].size();
		firstTextSize = theMessage[1].size();
		secondTextSize = theMessage[2].size();
		//12 is for the 3 lengths being written in fron of the messages. 3 ints = 12 bytes;
		tempSize = commandLength + firstTextSize + secondTextSize + 12;
		return  tempSize;
	}
	else if (theMessage.size() == 1)
	{
		commandLength = theMessage[0].size();
		firstTextSize = theMessage[1].size();
		//12 is for the 3 lengths being written in fron of the messages. 2 ints = 8 bytes;
		tempSize = commandLength + firstTextSize + 8;
		return  tempSize;
	}
	else
	{
		commandLength = theMessage[0].size();
		tempSize = commandLength + 4;
		return tempSize;
	}

	return tempSize;
}

std::string& SocketManager::parseMessage(int& bytesReceived)
{
	Header* tempHeader;
	int messageLength = 0;
	std::string theMessage = "";
	//check to see if there is enough bytes to read the packet header
	if (bytesReceived >= 8)// enough bytes to read the header
	{
		tempHeader = new Header();
		//read the packet and do something with it 
		tempHeader->packet_length = this->theBuffer->ReadInt32BE();
		tempHeader->message_id = this->theBuffer->ReadInt32BE();
		if (bytesReceived == tempHeader->packet_length)
		{
			//continue reading from the buffer
			messageLength = this->theBuffer->ReadInt32BE();
			theMessage = this->theBuffer->ReadStringBE(messageLength);
			//return the message

			//TODO::
			//based on message id do specific things. 

			/////////////////////////////////////////////////////////////////////
			//		USE comma separated list for more than one thing coming back?
			/////////////////////////////////////////////////////////////////////
			if (tempHeader->message_id == 1) // normal message
			{

			}
			if (tempHeader->message_id == 2)//refresh should give list lobbies
			{

			}
			if (tempHeader->message_id == 3)//view the game lobby list 
			{

			}

			return theMessage;
		}
	}
	return theMessage;
}

void SocketManager::receiveMessage(std::vector<std::string>& theScreenInfo)
{
	int bytesReceived = recv(*this->theSocket, this->theBuffer->getBufferAsCharArray(), this->theBuffer->GetBufferLength() + 1, 0);
	if (bytesReceived > 0)
	{
		//do the conversion
		std::string receivedPhrase = parseMessage(bytesReceived);
		if (receivedPhrase.size() > 0)
		{
			//add to the screen "buffer"
			theScreenInfo.push_back(receivedPhrase);
		}
	}
	else if (bytesReceived == -1) {//print error message

	}
	else if (bytesReceived == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {//print error message
		print_text("receive failed with error: %s", WSAGetLastError());
	}

}