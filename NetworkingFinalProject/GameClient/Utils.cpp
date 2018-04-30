#include "Utility.h"

#include <stdio.h>
#include <Windows.h>
#include <fstream>
#include <sstream>

HANDLE hStdout;
CONSOLE_CURSOR_INFO lpCursor;
COORD coord = { 0,0 };
int max_number_of_rows = 30;
char buffer[1024];

//Name:			start_text
//Purpose:		Tells the print text function where to start writing.
//Return:		void
void start_text() {
	GetConsoleCursorInfo(hStdout, &lpCursor);
	lpCursor.bVisible = false;
	SetConsoleCursorInfo(hStdout, &lpCursor);

	coord.Y = 22;
}


void end_text() {
	coord.Y = 22;
}

//Name:			print_text
//Purpose:		Prints text to the console using the STD_OUTPUT_HANDLE.
//Return:		void
void print_text(const char* text, ...) {

	va_list argptr;
	va_start(argptr, text);
	vsprintf(buffer, text, argptr);
	va_end(argptr);

	int len = strlen(buffer);

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorPosition(hStdout, coord);
	coord.Y++;
	WriteConsole(hStdout, "", 1, NULL, NULL);
	WriteConsole(hStdout, buffer, len, NULL, NULL);
}

//Name:			getGameInfo
//Purpose:		Gets the create lobby options from file.
//Return:		std::map<std::string, std::vector<std::string>>
std::map<std::string, std::vector<std::string>> getGameInfo(std::string filename) {
	//open the file
	std::ifstream file(filename);
	std::map<std::string, std::vector<std::string>> gameInfo;

	std::vector<std::string> mapNames;
	std::vector<std::string> numPlayers;
	std::vector<std::string> gameMode;

	std::string tempString;

	if (file.is_open())
	{	// Didn't open file, so return
		ReadFileToToken(file, "MAPS");
		for (int i = 0; i < 4; i++)
		{
			file >> tempString;
			mapNames.push_back(tempString);
		}
		//put the map names in the map
		gameInfo["MAPS"] = mapNames;

		ReadFileToToken(file, "GAMEMODE");
		for (int i = 0; i < 3; i++)
		{
			file >> tempString;
			gameMode.push_back(tempString);
		}
		//put the game modes in the map
		gameInfo["GAMEMODE"] = gameMode;

		ReadFileToToken(file, "NUMPLAYERS");
		for (int i = 0; i < 3; i++)
		{
			file >> tempString;
			numPlayers.push_back(tempString);
		}	
		//put the num player options in the map
		gameInfo["NUMPLAYERS"] = numPlayers;
	}

	return gameInfo;
}

//Name:			ReadFileToToken
//Purpose:		Reads up to and just past a "token" given a file and a token.
//Return:		void
void ReadFileToToken(std::ifstream &file, std::string token)
{
	bool bKeepReading = true;
	std::string garbage;
	do
	{
		//parse the file from top to bottom looking for our token
		file >> garbage;
		//find the token we are looking for
		if (garbage == token)
		{
			return;
		}
	} while (bKeepReading);
	return;
}