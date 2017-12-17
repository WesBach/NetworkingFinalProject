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


}