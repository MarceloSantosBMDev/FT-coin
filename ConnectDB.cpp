#include "ConnectDB.h"
#include <stdexcept>
#include <iostream>

using namespace std;

ConnectDB::ConnectDB()
    : serverPort("52167"),
      serverIP("turntable.proxy.rlwy.net"),
      serverDatabase("railway"),
      serverUser("railway"),
      serverPassword("_PCMaGJ_o90FKpBkSI4elWSWKDZzDnAJ")
{
    try
    {
        driver = sql::mariadb::get_driver_instance();
        string connString = "jdbc:mariadb://" + serverIP + ":" + serverPort + "/" + serverDatabase;
        sql::SQLString url(connString);
        sql::Properties properties({
            {"user", serverUser},
            {"password", serverPassword}
        });
        
        conn = driver->connect(url, properties);
    }
    catch (sql::SQLException& e)
    {
        cerr << "Error connecting to MariaDB Platform: \n" << e.what() << endl;
        throw runtime_error("Error connecting to database");
    }
}

ConnectDB::~ConnectDB()
{
    if (conn)
    {
        conn->close();
    }
}

sql::Connection* ConnectDB::getConnection()
{
    return conn;
}