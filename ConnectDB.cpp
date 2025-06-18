#include "ConnectDB.h"
#include <stdexcept>
#include <iostream>

using namespace std;

ConnectDB::ConnectDB() {
    try {
        _driver = sql::mariadb::get_driver_instance();
        string connString = "jdbc:mariadb://" + _serverIP + ":" + _serverPort + "/" + _serverDatabase;
        sql::SQLString url(connString);
        sql::Properties properties({
            {"user", _serverUser},
            {"password", _serverPassword}
        });
        
        _conn = _driver->connect(url, properties);
    } catch (sql::SQLException& e) {
        cerr << "Error connecting to MariaDB Platform: \n" << e.what() << endl;
        throw runtime_error("Error connecting to database");
    }
}

ConnectDB::~ConnectDB() {
    if (_conn) {
        _conn->close();
        // O conector C++ gerencia a exclusão do objeto de conexão quando ele é fechado.
    }
}

sql::Connection* ConnectDB::getConnection() {
    return _conn;
}