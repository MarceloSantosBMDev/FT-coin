#ifndef CONNECT_DB_H
#define CONNECT_DB_H

#include <mariadb/conncpp.hpp>
#include <string>

using namespace std;

// Manages the connection to the MariaDB database.
class ConnectDB
{
private:
	const string serverPort;
	const string serverIP;
	const string serverDatabase;
	const string serverUser;
	const string serverPassword;

	sql::Driver* driver = nullptr;
	sql::Connection* conn = nullptr;

public:
	ConnectDB();
	~ConnectDB();
	sql::Connection* getConnection();
};

#endif /* CONNECT_DB_H */
