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


