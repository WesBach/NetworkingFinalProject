#include "UserInfo.h"
#include "Buffer.h"

UserInfo::UserInfo()
{
	carryOverBuffer = new Buffer();
	this->isLobbyHost = false;
	this->currentLobby = NULL;
	this->userName = "";
}

UserInfo::~UserInfo()
{

}

//Name:			findRequestId
//Purpose:		Checks to see if the user has the reqest id.
//Return:		bool
bool UserInfo::findRequestId(int& id) {
	for (int i = 0; i < this->requests.size(); i++)
	{
		if (id == requests[i])
			return true;
	}

	return false;
}


//Name:			removeRequest
//Purpose:		Remove the request id from the user.
//Return:		bool
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