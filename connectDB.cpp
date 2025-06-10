#include <iostream>
#include <iomanip>
#include "connectDB.h"

ServerDBConnection::ServerDBConnection()
{
    try
    {
        this->driver = sql::mariadb::get_driver_instance();
        std::string connString = "jdbc:mariadb://" + serverIP + ":" + serverPort + "/" + serverDatabase;
        sql::SQLString url(connString);
        sql::Properties properties(
            {
                {"user", this->serverUser},
                {"password", this->serverPassword}
            });

        this->connection = driver->connect(url, properties);
    }
    catch (sql::SQLException &myException)
    {
        std::cerr << "Error Connecting to MariaDB Platform: \n" << myException.what() << std::endl;
        exit(1);
    }

    const int width = 50;
	std::cout << "\n" << std::endl;
    std::cout << std::setfill('*') << std::setw(width) << "" << std::endl;
    std::cout << std::setfill(' ') << std::setw(width) << std::right << "*" << std::endl;
    std::cout << "*"
              << std::setw(width - 2) << std::left << " Connected to MariaDB RDBMS "
              << "*" << std::endl;
    std::string address = serverIP + ":" + serverPort + "/" + serverDatabase;
    std::cout << "*"
              << std::setw(width - 2) << std::left << (" At: " + address)
              << "*" << std::endl;
    std::cout << std::setfill(' ') << std::setw(width) << std::right << "*" << std::endl;
    std::cout << std::setfill('*') << std::setw(width) << "" << std::endl;
}

ServerDBConnection::~ServerDBConnection()
{
    if (this->connection != nullptr)
    {
        this->connection->close();
    }
}

sql::Connection* ServerDBConnection::getConnection() const
{
    return this->connection;
}