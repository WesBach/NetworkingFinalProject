#include "AuthenticationCommunicationManager.h"
#include "UserInfo.h"
#include "Buffer.h"

#include <WinSock2.h>
#include <iostream>

AuthenticationCommunicationManager::AuthenticationCommunicationManager()
{
}

AuthenticationCommunicationManager::~AuthenticationCommunicationManager()
{
}


void AuthenticationCommunicationManager::receiveMessage(UserInfo* theUser) {
	theUser->userBuffer->clearBuffer();
	theUser->userBuffer->resizeBuffer(512);

	int bytesReceived = recv(*theUser->userSocket, theUser->userBuffer->getBufferAsCharArray(), theUser->userBuffer->GetBufferLength() + 1, 0);
	if (bytesReceived >= 4)
	{
		//get the packet length
		int packetLength = theUser->userBuffer->ReadInt32BE();
		//make sure the whole message was delivered
		if (bytesReceived >= packetLength)
		{
			//get the message id 
			int id = theUser->userBuffer->ReadInt32BE();
			//register
			if (id == 20)
			{

			}
			else if (id == 21)//authenticate
			{

			}
		}
	}
}


void AuthenticationCommunicationManager::sendMessage(UserInfo* theUser) {

}

