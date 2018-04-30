#include "SocketManager.h"
#include "Utility.h"
#include <stdlib.h>    
#include <time.h> 

#define HEADER_SIZE 8

SocketManager::SocketManager() {
	this->theBuffer = new Buffer();
	this->carryOverBuffer = new Buffer();
	srand(time(NULL));
	setMinMax();
}

SocketManager::~SocketManager() {

}


//Name:			buildMessage		
//Purpose:		Given the vector of input strings builds the message.
//Return:		void
void SocketManager::buildMessage(std::vector<std::string>& theMessage)
{
	//build the message in the buffer
	if (theMessage.size() >= 1)
	{
		//create buffer with the correct size
		this->theBuffer->clearBuffer();
		this->theBuffer->resizeBuffer(this->getPacketSize(theMessage) + HEADER_SIZE);
		this->theBuffer->resetReadWriteIndex();

		//id = 1
		if (theMessage[0] == "REGISTER" || theMessage[0] == "register")
		{
			//2. Register an account. (2 marks)
			//write packet size add header size and request id size
			this->theBuffer->WriteInt32BE(this->getPacketSize(theMessage) + HEADER_SIZE);
			//message id
			this->theBuffer->WriteInt32BE(1);
			//write the random request id between min and max
			this->theBuffer->WriteInt32BE(rand() % this->requestIdMax + this->requestIdMin);
			//email length
			this->theBuffer->WriteInt32BE(theMessage[1].size());
			//email
			this->theBuffer->WriteStringBE(theMessage[1]);
			//password length
			this->theBuffer->WriteInt32BE(theMessage[2].size());
			//password
			this->theBuffer->WriteStringBE(theMessage[2]);
		}
		else if (theMessage[0] == "AUTHENTICATE" || theMessage[0] == "authenticate")
		{
			//id = 2
			//2. authenticate an account. (2 marks)
			//write packet size add header size and request id size
			this->theBuffer->WriteInt32BE(this->getPacketSize(theMessage) + HEADER_SIZE);
			this->theBuffer->WriteInt32BE(2);
			//write the random request id between min and max
			this->theBuffer->WriteInt32BE(rand()% this->requestIdMax + this->requestIdMin);
			//email
			this->theBuffer->WriteInt32BE(theMessage[1].size());
			this->theBuffer->WriteStringBE(theMessage[1]);
			//pass
			this->theBuffer->WriteInt32BE(theMessage[2].size());
			this->theBuffer->WriteStringBE(theMessage[2]);
		}
		else if (theMessage[0] == "CREATE" || theMessage[0] == "create")
		{
			//id = 3
			//3. Create a game lobby. (2 marks)
			this->theBuffer->WriteInt32BE(this->getPacketSize(theMessage) + HEADER_SIZE);
			this->theBuffer->WriteInt32BE(3);
			//map
			this->theBuffer->WriteInt32BE(theMessage[1].size());
			this->theBuffer->WriteStringBE(theMessage[1]);
			//game mode 
			this->theBuffer->WriteInt32BE(theMessage[2].size());
			this->theBuffer->WriteStringBE(theMessage[2]);
			//lobby name
			this->theBuffer->WriteInt32BE(theMessage[3].size());
			this->theBuffer->WriteStringBE(theMessage[3]);
			//num players(one int will be 4 bits)
			this->theBuffer->WriteInt32BE(4);
			int num;
			//convert to int
			sscanf(theMessage[4].c_str(), "%d", &num);
			this->theBuffer->WriteInt32BE(num);
		}
		else if (theMessage[0] == "VIEW" || theMessage[0] == "view")
		{
			//id = 4
			//4. View the game lobby list. (3 mark)
			//write packet length first
			this->theBuffer->resizeBuffer(HEADER_SIZE);

			this->theBuffer->WriteInt32BE(HEADER_SIZE);
			this->theBuffer->WriteInt32BE(4);
		}
		else if (theMessage[0] == "REFRESH" || theMessage[0] == "refresh")
		{
			//id = 5
			this->theBuffer->resizeBuffer(HEADER_SIZE);
			//5. Refresh the game lobby list.This can be done automatically, or by client
			this->theBuffer->WriteInt32BE(HEADER_SIZE);
			this->theBuffer->WriteInt32BE(5);
		}
		else if (theMessage[0] == "JOIN" || theMessage[0] == "join")
		{
			//id = 6
			//6. Join the game lobby
			this->theBuffer->WriteInt32BE(this->getPacketSize(theMessage) + HEADER_SIZE);
			this->theBuffer->WriteInt32BE(6);
			this->theBuffer->WriteInt32BE(theMessage[1].size());
			this->theBuffer->WriteStringBE(theMessage[1]);

		}
		else if (theMessage[0] == "LEAVE" || theMessage[0] == "leave")
		{
			//id = 7
			//7. Leave the game lobby
			this->theBuffer->resizeBuffer(this->getPacketSize(theMessage)+ HEADER_SIZE);
			this->theBuffer->WriteInt32BE(this->getPacketSize(theMessage));
			this->theBuffer->WriteInt32BE(7);
		}
	}

	//clear the vector for the next set of commands
	theMessage.clear();
}

//Name:			sendMessage		
//Purpose:		Sends the message in the buffer.
//Return:		void
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

//Name:			getPacketSize
//Purpose:		Determines the final size of the packet based on its items. With pre determined lenght prefixing.
//Return:		int&
int& SocketManager::getPacketSize(std::vector<std::string> theMessage) {
	int commandLength = 0;
	int firstTextSize = 0;
	int secondTextSize = 0;
	int tempSize = 0;

	//accumulate all the sizes (start at one to ignore the command string)
	for (int i = 1; i < theMessage.size(); i++)
	{
		tempSize += theMessage[i].size();
	}

	//get the sizes for the integers written in between as well
	tempSize += (theMessage.size() * 4);
	return tempSize;

}


//Name:			parseMessage
//Purpose:		Parses the message based on the amount of bytes received.
//Return:		std::vector<std::string>
std::vector<std::string> SocketManager::parseMessage(int& bytesReceived)
{
	Header* tempHeader;
	int messageLength = 0;
	std::vector<std::string> theMessages;
	std::string message = "";
	this->theBuffer->resetReadWriteIndex();
	//check to see if there is enough bytes to read the packet header
	if (bytesReceived >= 8)// enough bytes to read the header
	{
		tempHeader = new Header();
		//read the packet and do something with it 	
		tempHeader->packet_length = this->theBuffer->ReadInt32BE();

		tempHeader->message_id = this->theBuffer->ReadInt32BE();
		//check to make sure the whole packet has arrived

		while (bytesReceived < tempHeader->packet_length) {
			bytesReceived += recv(*this->theSocket, this->carryOverBuffer->getBufferAsCharArray(), this->carryOverBuffer->GetBufferLength(), 0);

			std::vector<char> tempCharVec = this->carryOverBuffer->getBuffer();
			std::vector<char> initialBuffer = this->theBuffer->getBuffer();

			for (int i = 0; i < tempCharVec.size(); i++)
			{
				initialBuffer.push_back(tempCharVec[i]);
			}
			//set the buffer to the new concatenated buffer and set the read index to the correct location
			this->theBuffer->setBuffer(initialBuffer);
			this->theBuffer->setReadIndex(8);

			if (tempHeader->packet_length > 1000)
				break;
		}

		//make sure there are enough bits to read the whole packet
		if (bytesReceived >= tempHeader->packet_length)
		{
			//continue reading from the buffer
			messageLength = this->theBuffer->ReadInt32BE();
			message = this->theBuffer->ReadStringBE(messageLength);
			//return the message

			if (tempHeader->message_id == 1) // normal message
			{
				//single message
				theMessages.push_back(message);
				return theMessages;
			}
			else if (tempHeader->message_id == 2)//refresh should give list lobbies
			{
				parseStringByComma(theMessages,message);
			}
			else if (tempHeader->message_id == 3)//view the game lobby list 
			{
				//string split on spaces and get the data
				parseStringByComma(theMessages, message);
			}
			else if (tempHeader->message_id == 4)//view the game lobby list 
			{
				//string split on spaces and get the data
				parseStringByComma(theMessages, message);
			}
			else if (tempHeader->message_id == 10) {
				//simple message
				theMessages.push_back(message);
			}

			return theMessages;
		}	
	}
	return theMessages;
}

//Name:			receiveMessage
//Purpose:		Recieves messages from the buffer, uses the sent message id to parse the message.
//Return:		void
void SocketManager::receiveMessage(std::vector<std::string>& theScreenInfo)
{
	int bytesReceived = recv(*this->theSocket, this->theBuffer->getBufferAsCharArray(), this->theBuffer->GetBufferLength(), 0);
	if (bytesReceived >= 4)
	{
		//do the conversion
		std::vector<std::string> receivedPhrase = parseMessage(bytesReceived);
		if (receivedPhrase.size() > 0)
		{
			//add to the screen "buffer"
			//theScreenInfo.clear();
			for(unsigned int i = 0; i < receivedPhrase.size(); i++)
			{
				theScreenInfo.push_back(receivedPhrase[i]);
			}
		}
	}
	else if (bytesReceived == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {//print error message
		int error = WSAGetLastError();
		print_text("receive failed with error: %i", WSAGetLastError());
	}

}

//Name:			parseStringByComma
//Purpose:		Parses comma seperated string of characters into a vector of strings.
//Return:		std::vector<std::string>&
std::vector<std::string>& SocketManager::parseStringByComma(std::vector<std::string>& container,std::string& message)
{
	std::string tempString;
	//string split on spaces and get the data
	for (int i = 0; i < message.size(); i++)
	{
		//check for spaces
		if (message[i] == ',')
		{
			//if the string has letters in it add it to the vector
			if(tempString.size() > 0)
				container.push_back(tempString);
			//go to next letter
			continue;
		}
		else //add the letter to the string
			tempString += message[i];
	}

	return container;
}

//Name:			setMinMax	
//Purpose:		Sets the min and max range for randomly generating id's.
//Return:		void
void SocketManager::setMinMax() {
	//number between 1 and 1000
	int min = rand() % 10000 + 1;
	this->requestIdMin = min * 100;
	this->requestIdMax = requestIdMin + 1000;
}