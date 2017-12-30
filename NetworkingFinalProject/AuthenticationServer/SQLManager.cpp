#include "SQLManager.h"

SQLManager::SQLManager() {
	try {
		driver = get_driver_instance();
		connection = driver->connect("127.0.0.1:3306", "root", "SQL123");
		connection->setSchema("authentication");
	}
	catch (sql::SQLException &exception)
	{
		std::cout << "# ERR: SQLException in " << __FILE__ << std::endl;
		std::cout << "(" << ")" << std::endl;
	}
}

SQLManager::~SQLManager() {

}

bool SQLManager::execute(const std::string & statement)
{
	try
	{
		this->prepState = connection->prepareStatement(statement.c_str());
		return this->prepState->execute();
	}
	catch (sql::SQLException &exception)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << exception.what();
		std::cout << " (MySQL error code: " << exception.getErrorCode();
		std::cout << ", SQLState: " << exception.getSQLState() << " )" << std::endl;
		return false;
	}
	return false;
}

int SQLManager::executeUpdate(const std::string & statement)
{
	try
	{
		this->prepState = this->connection->prepareStatement(statement.c_str());
		return this->prepState->executeUpdate();
	}
	catch (sql::SQLException &exception)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << exception.what();
		std::cout << " (MySQL error code: " << exception.getErrorCode();
		std::cout << ", SQLState: " << exception.getSQLState() << " )" << std::endl;
		return false;
	}
	return false;
}


//Name:			executeSelect
//Purpose:		Execute a passed in SQL select statement.
//Return:		sql::ResultSet*
sql::ResultSet* SQLManager::executeSelect(const std::string& statement) {
	try
	{
		this->prepState = this->connection->prepareStatement(statement.c_str());
		return this->prepState->executeQuery();
	}
	catch (sql::SQLException &exception)
	{
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << exception.what();
		std::cout << " (MySQL error code: " << exception.getErrorCode();
		std::cout << ", SQLState: " << exception.getSQLState() << " )" << std::endl;
		return false;
	}
}