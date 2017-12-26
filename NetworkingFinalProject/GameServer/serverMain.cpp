#define UNICODE
#define WIN_32_CHAT_APP_SERVER

//#include <Windows.h>   freaks out if i include this
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Buffer.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <map>
#include <ctime>
#include "UserInfo.h"
//#include "AccountAuthentication.pb.h"

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "5000"	//was 8899
#define DEFAULT_AUTHENTICATION_PORT "6000"	//was 8899
#define DEFAULT_BUFFER_LENGTH 512

//Globel variables
enum message_ID { JOINROOM, LEAVEROOM, SENDMESSAGE, RECEIVEMESSAGE };
std::map<char, std::vector<UserInfo*>> roomMap;
std::vector<UserInfo*> usersInServer;
fd_set master;
SOCKET ListenSocket;
Buffer* g_theBuffer = new Buffer();
//socket info structure to store all the individual socket information
int initListening();
void sendToClient(SOCKET* theSocket,std::string& message);

int main() {
	//zero out master
	FD_ZERO(&master);
	FD_SET(ListenSocket, &master);

	//start listening
	initListening();

	bool running = true;
	bool authServerConnected = false;

	while (running) {
		fd_set copy = master;

		//get number of sockets in communication at that moment
		int socketCount = select(0, &copy, nullptr, nullptr, 0);

		if (socketCount > 0)
		{
			for (int i = 0; i < socketCount; i++)
			{
				// Makes things easy for us doing this assignment
				SOCKET sock = copy.fd_array[i];

				if (sock == ListenSocket)// Is it an inbound connection?
				{
					// Accept a new connection
					SOCKET client = accept(ListenSocket, nullptr, nullptr);
					UserInfo* newUser = new UserInfo();
					//Create the userInfo struct and add them to the list of users
					newUser->userBuffer = new Buffer();
					newUser->userSocket = &client;

					//Assigns the new user to the hub room.
					usersInServer.push_back(newUser);

					////check to see if its the auth server (only works if auth server is first connection)
					//if (authServerConnected == false)
					//{
					//	authServerRec = client;
					//	authServerConnected = true;
					//}

					// Add the new connection to the list of connected clients
					FD_SET(client, &master);

					// Send a welcome message to the connected client
					std::string welcomeMsg = "Welcome to the Awesome Chat Server!";
					//send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
					sendToClient(newUser->userSocket, welcomeMsg);
				}

			}
		}

	}

}

int initListening() {
	WSADATA wsaData;
	struct addrinfo* result = 0;
	//struct addrinfo* ptr = NULL;
	struct addrinfo addressInfo;
	struct addrinfo hints;
	int iResult = 0;
	//create a socket for the server with the port 8899
	ZeroMemory(&addressInfo, sizeof(addressInfo));
	addressInfo.ai_family = AF_INET;
	addressInfo.ai_socktype = SOCK_STREAM;
	addressInfo.ai_protocol = IPPROTO_TCP;
	addressInfo.ai_flags = AI_PASSIVE;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	////get the address info for the authentication server
	//iResult = getaddrinfo(NULL, DEFAULT_AUTHENTICATION_PORT, &hints, &result);
	//if (iResult != 0) {
	//	printf("getaddrinfo failed with error: %d\n", iResult);
	//	WSACleanup();
	//	return 1;
	//}

	////connect to the authentication server  
	//for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
	//	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	//	if (ConnectSocket == INVALID_SOCKET) {
	//		printf("socket() failed with error: %d\n", iResult);
	//		WSACleanup();
	//		return 1;
	//	}

	//	//connect to the socket
	//	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	//	if (iResult == SOCKET_ERROR) {
	//		closesocket(ConnectSocket);
	//		ConnectSocket = INVALID_SOCKET;
	//		continue;
	//	}
	//	break;
	//}

	//freeaddrinfo(result);
	////Check if the Connected socket is valid
	//if (ConnectSocket == INVALID_SOCKET) {
	//	printf("Unable to connect to server\n");
	//	WSACleanup();
	//	return 1;
	//}

	// Socket()
	//socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
	ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
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
		return 1;
	}
	printf("Bind Listen Socket\n");

	// Listen()
	if (listen(ListenSocket, 5)) {
		printf("listen() failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	printf("Listen for incoming requests\n");

}

void sendToClient(SOCKET* theSocket, std::string& message) {

}