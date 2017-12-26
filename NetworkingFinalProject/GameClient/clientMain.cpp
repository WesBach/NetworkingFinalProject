#include <ctime>
#include <string>
#include <iostream>
#include <conio.h>
#include <list>
#include "Utility.h"
#include "SocketManager.h"

#define DEFAULT_PORT "5000"
#pragma comment (lib, "Ws2_32.lib")

SocketManager* g_ptheManager;
std::vector<std::string> g_screenInfo;

int connectToServer();
void setMainInstructions();
void printScreen();
std::string& keyboardInput(std::string& input);
std::vector<std::string> populateCommands(std::string& commands,const int& numCommands);

int main() {
	//1. Connect to the game server with an ip and port number(1 mark)
	connectToServer();
	setMainInstructions();

	bool run = true;
	std::string userInput;

	while (run)
	{

		//get keyboard input and use the Socket Manager to deal with the messages.
		userInput = keyboardInput(userInput);
		printScreen();
		std::string textOnScreen = "->" + userInput;

		print_text("%-75s", textOnScreen.c_str());
		//receive messages and send messages
		end_text();
	}
}

int connectToServer() {
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;
	int iResult;
	ULONG iMode = 0;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		//return 1;
	}
	printf("Winsock Initialized\n");


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//get the address info 
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	//set up the socket
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket() failed with error: %d\n", iResult);
			WSACleanup();
			return 1;
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
		return 1;
	}

	unsigned long nonBlock = 1;
	//set socket to non-blocking for recieving messages
	if (ioctlsocket(ConnectSocket, FIONBIO, &nonBlock) == SOCKET_ERROR) {
		printf("ioctlsocket(FIONBIO) failed with error %d\n", WSAGetLastError());
		return 1;
	}

	//get the server socket
	g_ptheManager->theSocket = &ConnectSocket;
}

void setMainInstructions() {
	//display commands before loop
	print_text("=======================================");
	print_text("		Chat Commands:              ");
	print_text("=======================================");
	print_text("Register: REGISTER email password");
	print_text("Authenticate: AUTHENTICATE email password");
	print_text("Join Room: JR (a-z)");
	print_text("Leave Room: LR (a-z)");
	print_text("View Lobbies: VIEW");
	print_text("Refresh Lobbies: REFRESH");
	print_text("Send Message: SM (followed by message)");
}

void printScreen()
{
	//print all items in the vector to the screen
	for (int i = 0; i < g_screenInfo.size(); i++)
	{
		print_text("->%s", g_screenInfo[i].c_str());
	}
}

std::string& keyboardInput(std::string& input) {

	int joinRoomIndex = -1;
	int leaveRoomIndex = -1;
	int viewIndex = -1;
	int refreshLobbiesIndex = -1;
	int sendMessageIndex = -1;

	std::vector<std::string> commands;

	if (_kbhit()) {
		char c = _getch();
		if (c == '\r')
		{
			if (input.size() >= 4)
			{
				sendMessageIndex = input.find("SM ");//Send Message
				joinRoomIndex = input.find("JR ");//Join Room
				leaveRoomIndex = input.find("LR ");//Leave Room
				viewIndex = input.find("VIEW"); //View lobbies
				refreshLobbiesIndex = input.find("REFRESH");	//Refresh the lobbies


				if (joinRoomIndex >= 0)
				{
					//create the command and message and send it to the server
					commands = populateCommands(input, 2);
				}
				else if (leaveRoomIndex >= 0)
				{
					//create the command and message and send it to the server
					commands = populateCommands(input, 2);
				}
				else if (sendMessageIndex >= 0)
				{
					//create the command and message and send it to the server
					commands = populateCommands(input, 2);
				}
				else if (viewIndex >= 0)
				{
					//create the command and message and send it to the server
					commands = populateCommands(input, 1);
				}
				else if (refreshLobbiesIndex >= 0) 
				{
					//create the command and message and send it to the server
					commands = populateCommands(input, 1);
				}

				//build the message and send it
				if (commands.size() > 0)
				{
					g_ptheManager->buildMessage(commands);
					g_ptheManager->sendMessage();
				}

			}
		}
		else
		{
			input += c;
		}


		//check to see if messages are being received
		g_ptheManager->receiveMessage(g_screenInfo);

	}
	return input;
}

std::vector<std::string> populateCommands(std::string& commands,const int& numCommands) {
	std::vector<std::string> theCommands;
	std::string command = "";
	std::string message = "";

	if (numCommands == 1)
	{
		theCommands.push_back(commands);
		return theCommands;
	}
	else {
		//get the command and the message
		command = commands.substr(0, 2);
		message = commands.substr(3, commands.size() - 3);
		theCommands.push_back(command);
		theCommands.push_back(message);
		return theCommands;
	}

	return theCommands;
}

