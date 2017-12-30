#ifndef _SQL_MANAGER_HG_
#define _SQL_MANAGER_HG_
#include <string>
#include <cppconn\driver.h>
#include <cppconn\exception.h>
#include <cppconn\resultset.h>
#include <cppconn\statement.h>
#include <cppconn\prepared_statement.h>

class SQLManager {
public:

	SQLManager();
	~SQLManager();

	bool execute(const std::string& statement);
	int executeUpdate(const std::string& statement);
	sql::ResultSet* executeSelect(const std::string& statement);
private:
	sql::Statement* statement;
	sql::PreparedStatement* prepState;
	sql::Driver* driver;
	sql::Connection* connection;
};
#endif // !_SQL_MANAGER_HG_

