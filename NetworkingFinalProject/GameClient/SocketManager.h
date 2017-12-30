#ifndef _SocketManager_HG_
#define _SocketManager_HG_

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <vector>

#include "Buffer.h"


class SocketManager {
public:
	SocketManager();
	~SocketManager();

	class Header {
	public:
		//[packet_length][message_id]
		int32_t packet_length;			//in bytes
		int32_t message_id;				//What user is trying to do
	};

	Buffer* theBuffer;
	SOCKET* theSocket;
	std::vector<int> requestIds;


	int& getPacketSize(std::vector<std::string> theMassage);
	void buildMessage(std::vector<std::string>& theMessage);
	void sendMessage();
	std::vector<std::string> parseMessage(int& bytesReceived );
	void receiveMessage(std::vector<std::string>& theScreenInfo);
	std::vector<std::string>& parseStringBySpace(std::vector<std::string>& container, std::string& message);
};

#endif // 

