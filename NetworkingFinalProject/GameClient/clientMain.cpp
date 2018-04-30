#include <string>
#include <iostream>
#include <conio.h>
#include "Utility.h"
#include "SocketManager.h"

#define DEFAULT_PORT "5000"
#pragma comment (lib, "Ws2_32.lib")

//globals
SocketManager* g_ptheManager;
std::vector<std::string> g_screenInfo;
SOCKET ConnectSocket;
bool run;
std::map<std::string, std::vector<std::string>> gameInfo;
//function declarations
int connectToServer(std::string port);
void setMainInstructions();
void printScreen();
std::string& keyboardInput(std::string& input);
std::vector<std::string> populateCommands(std::string& commands,const int& numCommands);

int main() {
	//get rid of the ability to hit x(must type QUIT to close window)
	HWND hwnd = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(hwnd, FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
	//TODO:: Change this to take in an ip and port number
	g_ptheManager = new SocketManager();
	//1. Connect to the game server with an ip and port number(1 mark)
	connectToServer(DEFAULT_PORT);
	setMainInstructions();

	//run the loop
	run = true;
	//container for user input
	std::string userInput;

	while (run)
	{
		//get keyboard input and use the Socket Manager to deal with the messages.
		userInput = keyboardInput(userInput);
		printScreen();
		std::string textOnScreen = "->" + userInput;

		print_text("%-75s", textOnScreen.c_str());
		end_text();
	}

	//cleanup
	closesocket(ConnectSocket);
	WSACleanup();
}

//Name:			connectToServer
//Purpose:		Connect to the Game Server.
//Return:		int
int connectToServer(std::string port) {
	WSADATA wsaData;
	ConnectSocket = INVALID_SOCKET;
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

//Name:			setMainInstructions
//Purpose:		Prints the main instructions to the screen.
//Return:		void
void setMainInstructions() {
	gameInfo = getGameInfo("LobbyInfo.txt");
	std::string tempString = "";

	//display commands before loop
	print_text("=======================================");
	print_text("		Chat Commands:              ");
	print_text("=======================================");
	print_text("Register: REGISTER email password");
	print_text("Authenticate: AUTHENTICATE email password");
	print_text("Join Room: JOIN lobbyname");
	print_text("Leave Room: LEAVE");
	print_text("View Lobbies: VIEW");
	print_text("Create Lobbies: CREATE (Map Name, Game Mode, Lobby Name, Num Players)");
	print_text("Quit: QUIT:");
	print_text("Create Options:");
	print_text("===============");
	//print the map names
	print_text("MAPS");
	for (int i = 0; i < gameInfo["MAPS"].size(); i++)
		tempString += "(" + gameInfo["MAPS"][i]+")";
	print_text(tempString.c_str());
	tempString.clear();
	//print game modes
	print_text("GAME MODE");
	for (int i = 0; i < gameInfo["GAMEMODE"].size(); i++)
		tempString += "(" + gameInfo["GAMEMODE"][i] + ")";
	print_text(tempString.c_str());
	tempString.clear();

	print_text("LOBBY NAME");
	print_text("Your chosen Lobby Name");

	//print num player options
	print_text("NUM PLAYERS");
	for (int i = 0; i < gameInfo["NUMPLAYERS"].size(); i++)
		tempString += "(" + gameInfo["NUMPLAYERS"][i] + ")";
	print_text(tempString.c_str());
}


//Name:			printScreen
//Purpose:		Prints the data to the screen.
//Return:		void
void printScreen()
{
	//print all items in the vector to the screen
	for (int i = 0; i < g_screenInfo.size(); i++)
	{
		print_text("->%s", g_screenInfo[i].c_str());
	}
}

//Name:			keyboardInput
//Purpose:		Gets the input form the keyboard and once the input is valid sends it to be processed.
//Return:		std::string&
std::string& keyboardInput(std::string& input) {
	//index variables for string find
	int authenticateUserIndex = -1;
	int createIndex = -1;
	int viewIndex = -1;
	int refreshLobbiesIndex = -1;
	int registerUserIndex = -1;
	int quitIndex = -1;
	int joinIndex = -1;
	int leaveIndex = -1;
	//vector to hold command and messages
	std::vector<std::string> commands;

	if (_kbhit()) {
		char c = _getch();
		if (c == '\r')
		{
			if (input.size() >= 4)
			{
				registerUserIndex = input.find("REGISTER ");//Send Message
				authenticateUserIndex = input.find("AUTHENTICATE ");//Join Room
				createIndex = input.find("CREATE ");//Leave Room
				viewIndex = input.find("VIEW"); //View lobbies
				refreshLobbiesIndex = input.find("REFRESH");	//Refresh the lobbies
				quitIndex = input.find("QUIT");
				joinIndex = input.find("JOIN ");
				leaveIndex = input.find("LEAVE");

				if (registerUserIndex >= 0)
				{
					//create the command and message and send it to the server
					commands = populateCommands(input, 3);
					input = "";
				}
				if (leaveIndex >= 0)
				{
					//create the command and message and send it to the server
					commands = populateCommands(input, 1);
					input = "";
				}
				if (joinIndex >= 0)
				{
					//create the command and message and send it to the server
					commands = populateCommands(input, 2);
					input = "";
				}
				else if (createIndex >= 0)
				{
					//create the command and message and send it to the server
					commands = populateCommands(input, 5);
					input = "";
				}
				else if (authenticateUserIndex >= 0)
				{
					//create the command and message and send it to the server
					commands = populateCommands(input, 3);
					input = "";
				}
				else if (viewIndex >= 0)
				{
					//create the command and message and send it to the server
					commands = populateCommands(input, 1);
					input = "";
				}
				else if (refreshLobbiesIndex >= 0)
				{
					//create the command and message and send it to the server
					commands = populateCommands(input, 1);
					input = "";
				}
				else if (quitIndex >= 0)
				{
					run = false;
				}

				//build the message and send it
				if (commands.size() > 0)
				{
					g_ptheManager->buildMessage(commands);
					g_ptheManager->sendMessage();
				}
			}
		}
		else if (c == '\b')
		{
			input = input.substr(0, input.length() - 1);
		}
		else
		{
			input += c;
		}
	}

	//check to see if messages are being received
	g_ptheManager->receiveMessage(g_screenInfo);
	return input;
}

//Name:			populateCommands
//Purpose:		Separates the input into strings
//Return:		std::vector<std::string>
std::vector<std::string> populateCommands(std::string& commands,const int& numCommands) {	
	std::string tempString = "";
	std::vector<std::string> theCommands;
	//parse the string for commands and password
	for (int i = 0; i < commands.size(); i++)
	{
		//if its a space skip
		if (i == commands.size() - 1 && tempString != "")
		{
			tempString += commands[i];
			theCommands.push_back(tempString);
		}
		
		if (commands[i] != ' ')
		{
			tempString += commands[i];
			//if the last one was a single letter or number
			if( i == commands.size() -1)
				theCommands.push_back(tempString);
		}
		else {
			theCommands.push_back(tempString);
			//clear the string 
			tempString = "";
		}
	}

	return theCommands;
}