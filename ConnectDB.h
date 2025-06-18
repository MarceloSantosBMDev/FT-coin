#ifndef CONNECT_DB_H
#define CONNECT_DB_H

#include <mariadb/conncpp.hpp>
#include <string>

using namespace std;

class ConnectDB {
private:
	string _serverPort = "52167";
	string _serverIP = "turntable.proxy.rlwy.net";
	string _serverDatabase = "railway";
	string _serverUser = "railway";
	string _serverPassword = "_PCMaGJ_o90FKpBkSI4elWSWKDZzDnAJ";

	sql::Driver* _driver = nullptr;
	sql::Connection* _conn = nullptr;

public:
	ConnectDB();
	~ConnectDB();
	sql::Connection* getConnection();
};

#endif /* CONNECT_DB_H */
