#include "AuthenticationCommunicationManager.h"
#include "UserInfo.h"
#include "Buffer.h"
#include "SQLManager.h"
#include "OpenSSLUtilities.h"

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
	int sendResult = send(*theUser->userSocket, this->theBuffer->getBufferAsCharArray(), this->theBuffer->GetBufferLength() + 1, 0);
	//check for error
	if (sendResult == SOCKET_ERROR)
	{
		printf("Send failed with error: %s", sendResult);
	}
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
		results.second = "Account creation failed: User already exists!";
		return results;
	}
	else
	{
		//get salt
		//create the salt 
		std::string salt = getSalt();
		//add the salt to the password
		std::string tempPass = password + salt;
		//hash the password
		std::string password = hashPassword(tempPass.c_str());
		//add the user to the db
		std::string insert = "INSERT INTO accounts (email,salt,password,last_login) values('" + email + "','" + salt +"','" + password + ",NOW()');";

		bool success = this->theSQLManager->execute(insert);

		if (success)
		{
			//succeeded
			results.first = true;
			results.second = "Successfully created account!";
			return results;
		}
		else {
			results.first = false;
			results.second = "Account creation failed: Server Error!";
			return results;
		}

	}

	return results;
}

std::pair<bool, std::string> AuthenticationCommunicationManager::authenticateUser(std::string & email, std::string & password)
{
	std::pair<bool, std::string> results(false,"");
	//authenticate user with email and password

	std::string selectUserByEmail = "SELECT * FROM accounts WHERE email ='" + email + "';";
	sql::ResultSet* userResult = this->theSQLManager->executeSelect(selectUserByEmail);

	if (userResult->rowsCount == 1)
	{
		//set it to the first item
		userResult->next();
		//get the user salt
		std::string salt = userResult->getString("salt").c_str();
		//get the user hash
		std::string storedHash = userResult->getString("hashed_password").c_str();

		std::string tempPass = password + salt;
		//hash the password
		std::string hashedPassword = hashPassword((char*)tempPass.c_str());

		if (storedHash == hashedPassword)
		{

			results.first = true;
			results.second = "Account authentication Success!";
			return results;
		}
		else{

			results.first = false;
			results.second = "Account authentication failed: Invalid Password!";
			return results;
		}

	}
	else {
		results.first = false;
		results.second = "Account authentication failed: Account doesnt exist!";
		return results;
	}

	return results;
}


