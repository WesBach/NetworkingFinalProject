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

}


void AuthenticationCommunicationManager::sendMessage(UserInfo* theUser) {

}

