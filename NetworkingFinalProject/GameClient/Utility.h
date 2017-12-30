#ifndef _Utility_HG_
#define _Utility_HG_
#include <map>
#include <vector>

void start_text();
void end_text();

void print_text(const char* text, ...);
std::map<std::string, std::vector<std::string>> getGameInfo(std::string fileName);

void ReadFileToToken(std::ifstream &file, std::string token);
#endif // !_Utility_HG_
