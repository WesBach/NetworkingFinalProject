#include "UserInfo.h"

UserInfo::UserInfo()
{
	this->isLobbyHost = false;
}

UserInfo::~UserInfo()
{
}

bool UserInfo::findRequestId(int& id) {
	for (int i = 0; i < this->requests.size(); i++)
	{
		if (id == requests[i])
			return true;
	}

	return false;
}

bool UserInfo::removeRequest(int& id) {
	for (std::vector<int>::iterator it = this->requests.begin(); it != requests.end(); ++it)
	{
		if (*it == id)
		{
			//remove the request id from the vector
			it = requests.erase(it);
			return true;
		}
	}

	return false;
}

