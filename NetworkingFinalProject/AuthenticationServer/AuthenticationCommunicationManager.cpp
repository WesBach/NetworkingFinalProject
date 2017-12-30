#include "AuthenticationCommunicationManager.h"
#include "UserInfo.h"
#include "Buffer.h"
#include "SQLManager.h"

#include <WinSock2.h>
#include <iostream>

AuthenticationCommunicationManager::AuthenticationCommunicationManager()
{
	this->theBuffer = new Buffer();
	this->theSQLManager = new SQLManager();
}

AuthenticationCommunicationManager::~AuthenticationCommunicationManager()
{
	delete this->theBuffer;
	delete this->theSQLManager;
}


void AuthenticationCommunicationManager::receiveMessage(UserInfo* theUser) {
	theUser->userBuffer->clearBuffer();
	theUser->userBuffer->resizeBuffer(512);

	int bytesReceived = recv(*theUser->userSocket, theUser->userBuffer->getBufferAsCharArray(), theUser->userBuffer->GetBufferLength() + 1, 0);
	if (bytesReceived >= 4)
	{
		//get the packet length
		int packetLength = theUser->userBuffer->ReadInt32BE();
		//make sure the whole message was delivered
		if (bytesReceived >= packetLength)
		{
			//get the message id 
			int id = theUser->userBuffer->ReadInt32BE();
			//get the info from the buffer

			int requestId = theUser->userBuffer->ReadInt32BE();
			int emailLength = theUser->userBuffer->ReadInt32BE();
			std::string email = theUser->userBuffer->ReadStringBE(emailLength);
			int passLength = theUser->userBuffer->ReadInt32BE();
			std::string password = theUser->userBuffer->ReadStringBE(passLength);
		


			if (id == 1)
			{
				//register
				this->registerUser(email, password);
			}
			else if (id == 2)
			{
				//authenticate
				this->authenticateUser(email, password);
			}
		}
	}
}


void AuthenticationCommunicationManager::sendMessage(UserInfo* theUser) {

}

std::pair<bool, std::string> AuthenticationCommunicationManager::registerUser(std::string & email, std::string & password)
{
	std::pair<bool, std::string> results(false, "");
	//check to see if user already exists
	//find the user by it's email
	std::string selectUserByEmail = "SELECT * FROM accounts WHERE email ='" + email + "';";
	sql::ResultSet* userResult = this->theSQLManager->executeSelect(selectUserByEmail);

	//if it does exist return false
	if (userResult->next())
	{
		results.first = false;
		results.second = "User already exists!";
		return results;
	}
	else
	{
		//get salt
		//create the salt 
		std::string salt = createSalt();
		//add the salt to the password
		std::string tempPass = password + salt;
		//hash the password

		//add the user to the db

	}

	return results;
}

std::pair<bool, std::string> AuthenticationCommunicationManager::authenticateUser(std::string & email, std::string & password)
{
	std::pair<bool, std::string> results(false,"");
	//authenticate user with email and password
	std::string selectUserByEmail = "SELECT * FROM accounts WHERE email ='" + email + "';";
	sql::ResultSet* userResult = this->theSQLManager->executeSelect(selectUserByEmail);


	return results;
}


