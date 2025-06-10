#include <mariadb/conncpp.hpp>

#ifndef SERVERDBCONNECTION_H_
#define SERVERDBCONNECTION_H_

using namespace std;

class ServerDBConnection final
	{
	private:
		string serverPort = "52167";
		string serverIP = "turntable.proxy.rlwy.net";
		string serverDatabase = "railway";
		string serverUser = "railway";
		string serverPassword = "_PCMaGJ_o90FKpBkSI4elWSWKDZzDnAJ";

		sql::Driver *driver = NULL;
		sql::Connection *connection;

	public:
		ServerDBConnection();
		virtual ~ServerDBConnection();
		sql::Connection* getConnection() const;
	};

#endif /* SERVERDBCONNECTION_H_ */
