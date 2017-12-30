#include <WinSock2.h>
#include <WS2tcpip.h>
#include "SQLManager.h"
#include "Buffer.h"

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "6000"
#define DEFAULT_CHAT_SERVER_PORT "5000"

void initListening();
void connectToServer();
bool running = true;

SQLManager* pTheSQLManager;
SOCKET ListenSocket;
SOCKET ConnectSocket;
fd_set master;

int main() {
	//connects to the db 
	pTheSQLManager = new SQLManager();

	initListening();
	ULONG nonBlock = 1;
	//zero out master
	FD_ZERO(&master);
	FD_SET(ListenSocket, &master);

	while (running)
	{
		fd_set copy = master;
		int socketCount = select(0, &copy, nullptr, nullptr, 0);

		if (socketCount > 0)
		{
			for (int i = 0; i < socketCount; i++)
			{
				SOCKET sock = copy.fd_array[i];

				// Is it an inbound communication?
				if (sock == ListenSocket)
				{
					//TODO::
					//Create the CommunicationManager to handle incoming and outgoing messages


				}//end if
			}
		}
	}

}

void initListening() {

	WSADATA wsaData;
	struct addrinfo* result = 0;
	struct addrinfo addressInfo;
	int iResult = 0;
	//create a socket for the server with the port 8899
	ZeroMemory(&addressInfo, sizeof(addressInfo));
	addressInfo.ai_family = AF_INET;
	addressInfo.ai_socktype = SOCK_STREAM;
	addressInfo.ai_protocol = IPPROTO_TCP;
	addressInfo.ai_flags = AI_PASSIVE;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
	}

	// Socket()
	ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
	}
	printf("Created Listen Socket\n");

	// Bind()
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &addressInfo, &result);
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind() failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
	}
	printf("Bind Listen Socket\n");

	// Listen()
	if (listen(ListenSocket, 5)) {
		printf("listen() failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
	}
	printf("Listen for incoming requests\n");
	freeaddrinfo(result);
}

void connectToServer() {

	int iResult = 0;
	struct addrinfo* result = 0;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//get the address info for the authentication server
	iResult = getaddrinfo(NULL, DEFAULT_CHAT_SERVER_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	//connect to the authentication server  
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket() failed with error: %d\n", iResult);
			WSACleanup();
			return;
		}

		//connect to the socket
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);
	//Check if the Connected socket is valid
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server\n");
		WSACleanup();
	}
}