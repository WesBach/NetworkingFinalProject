#include "Utility.h"

#include <stdio.h>
#include <Windows.h>
#include <fstream>
#include <sstream>

HANDLE hStdout;
CONSOLE_CURSOR_INFO lpCursor;
COORD coord = { 0,0 };
int max_number_of_rows = 20;
char buffer[1024];

void start_text() {
	GetConsoleCursorInfo(hStdout, &lpCursor);
	lpCursor.bVisible = false;
	SetConsoleCursorInfo(hStdout, &lpCursor);

	coord.Y = 20;
}

void end_text() {
	coord.Y = 20;
}

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


void ReadFileToToken(std::ifstream &file, std::string token)
{
	bool bKeepReading = true;
	std::string garbage;
	do
	{
		file >> garbage;		// Title_End??
		if (garbage == token)
		{
			return;
		}
	} while (bKeepReading);
	return;
}

int getRandRequestId() {

}