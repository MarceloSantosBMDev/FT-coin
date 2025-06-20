#include "WalletDBDAO.h"
#include <stdexcept>
#include <mariadb/conncpp.hpp>

using namespace std;

WalletDBDAO::WalletDBDAO(ConnectDB* conn) : connection(conn) {}

unique_ptr<Wallet> WalletDBDAO::rowToWallet(sql::ResultSet* res)
{
    return make_unique<Wallet>(
        res->getInt("IdCarteira"), 
        string(res->getString("Titular")), 
        string(res->getString("Corretora"))
    );
}

void WalletDBDAO::insert(Wallet& wallet)
{
    try
    {
        unique_ptr<sql::PreparedStatement> stmnt(connection->getConnection()->prepareStatement("INSERT INTO CARTEIRA (Titular, Corretora) VALUES (?, ?)"));
        stmnt->setString(1, wallet.getHolder());
        stmnt->setString(2, wallet.getBroker());
        stmnt->executeUpdate();
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error inserting wallet: " + string(e.what()));
    }
}

void WalletDBDAO::update(Wallet& wallet)
{
    try
    {
        unique_ptr<sql::PreparedStatement> stmnt(connection->getConnection()->prepareStatement("UPDATE CARTEIRA SET Titular = ?, Corretora = ? WHERE IdCarteira = ?"));
        stmnt->setString(1, wallet.getHolder());
        stmnt->setString(2, wallet.getBroker());
        stmnt->setInt(3, wallet.getId());
        stmnt->executeUpdate();
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error updating wallet: " + string(e.what()));
    }
}

void WalletDBDAO::remove(int id)
{
    try
    {
        unique_ptr<sql::PreparedStatement> stmnt(connection->getConnection()->prepareStatement("DELETE FROM CARTEIRA WHERE IdCarteira = ?"));
        stmnt->setInt(1, id);
        stmnt->executeUpdate();
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error deleting wallet: " + string(e.what()));
    }
}

unique_ptr<Wallet> WalletDBDAO::findById(int id)
{
    try
    {
        unique_ptr<sql::PreparedStatement> stmnt(connection->getConnection()->prepareStatement("SELECT * FROM CARTEIRA WHERE IdCarteira = ?"));
        stmnt->setInt(1, id);
        sql::ResultSet* res = stmnt->executeQuery();
        if (res->next())
        {
            auto wallet = rowToWallet(res);
            delete res;
            return wallet;
        }
        delete res;
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error finding wallet: " + string(e.what()));
    }
    return nullptr;
}

vector<unique_ptr<Wallet>> WalletDBDAO::findAll()
{
    vector<unique_ptr<Wallet>> wallets;
    try
    {
        unique_ptr<sql::Statement> stmnt(connection->getConnection()->createStatement());
        sql::ResultSet* res = stmnt->executeQuery("SELECT * FROM CARTEIRA");
        while (res->next())
        {
            wallets.push_back(rowToWallet(res));
        }
        delete res;
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error finding all wallets: " + string(e.what()));
    }
    return wallets;
}

unique_ptr<Wallet> WalletDBDAO::findByHolder(const string& holderName)
{
    try
    {
        unique_ptr<sql::PreparedStatement> stmnt(connection->getConnection()->prepareStatement("SELECT * FROM CARTEIRA WHERE Titular = ?"));
        stmnt->setString(1, holderName);
        sql::ResultSet* res = stmnt->executeQuery();
        if (res->next())
        {
            auto wallet = rowToWallet(res);
            delete res;
            return wallet;
        }
        delete res;
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error finding wallet by holder: " + string(e.what()));
    }
    return nullptr;
} 