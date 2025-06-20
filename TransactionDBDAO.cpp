#include "TransactionDBDAO.h"
#include <stdexcept>
#include <mariadb/conncpp.hpp>

using namespace std;

TransactionDBDAO::TransactionDBDAO(ConnectDB* conn) : connection(conn) {}

unique_ptr<Transaction> TransactionDBDAO::rowToTransaction(sql::ResultSet* res)
{
    return make_unique<Transaction>(
        res->getInt("IdMovimento"), 
        res->getInt("IdCarteira"), 
        string(res->getString("TipoOperacao")), 
        res->getDouble("Quantidade"), 
        string(res->getString("Data"))
    );
}

void TransactionDBDAO::insert(Transaction& transaction)
{
    try
    {
        unique_ptr<sql::PreparedStatement> stmnt(connection->getConnection()->prepareStatement("INSERT INTO MOVIMENTACAO (IdCarteira, Data, TipoOperacao, Quantidade) VALUES (?, ?, ?, ?)"));
        stmnt->setInt(1, transaction.getWalletId());
        stmnt->setString(2, transaction.getDate());
        stmnt->setString(3, transaction.getOperationType());
        stmnt->setDouble(4, transaction.getQuantity());
        stmnt->executeUpdate();
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error inserting transaction: " + string(e.what()));
    }
}

void TransactionDBDAO::update(Transaction& transaction)
{
    try
    {
        unique_ptr<sql::PreparedStatement> stmnt(connection->getConnection()->prepareStatement("UPDATE MOVIMENTACAO SET IdCarteira = ?, Data = ?, TipoOperacao = ?, Quantidade = ? WHERE IdMovimento = ?"));
        stmnt->setInt(1, transaction.getWalletId());
        stmnt->setString(2, transaction.getDate());
        stmnt->setString(3, transaction.getOperationType());
        stmnt->setDouble(4, transaction.getQuantity());
        stmnt->setInt(5, transaction.getId());
        stmnt->executeUpdate();
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error updating transaction: " + string(e.what()));
    }
}

void TransactionDBDAO::remove(int id)
{
    try
    {
        unique_ptr<sql::PreparedStatement> stmnt(connection->getConnection()->prepareStatement("DELETE FROM MOVIMENTACAO WHERE IdMovimento = ?"));
        stmnt->setInt(1, id);
        stmnt->executeUpdate();
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error deleting transaction: " + string(e.what()));
    }
}

void TransactionDBDAO::removeByWalletId(int walletId)
{
    try
    {
        unique_ptr<sql::PreparedStatement> stmnt(connection->getConnection()->prepareStatement("DELETE FROM MOVIMENTACAO WHERE IdCarteira = ?"));
        stmnt->setInt(1, walletId);
        stmnt->executeUpdate();
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error deleting transactions by wallet_id: " + string(e.what()));
    }
}

unique_ptr<Transaction> TransactionDBDAO::findById(int id)
{
    try
    {
        unique_ptr<sql::PreparedStatement> stmnt(connection->getConnection()->prepareStatement("SELECT * FROM MOVIMENTACAO WHERE IdMovimento = ?"));
        stmnt->setInt(1, id);
        sql::ResultSet* res = stmnt->executeQuery();
        if (res->next())
        {
            auto trans = rowToTransaction(res);
            delete res;
            return trans;
        }
        delete res;
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error finding transaction: " + string(e.what()));
    }
    return nullptr;
}

vector<unique_ptr<Transaction>> TransactionDBDAO::findByWalletId(int walletId)
{
    vector<unique_ptr<Transaction>> transactions;
    try
    {
        unique_ptr<sql::PreparedStatement> stmnt(connection->getConnection()->prepareStatement("SELECT * FROM MOVIMENTACAO WHERE IdCarteira = ?"));
        stmnt->setInt(1, walletId);
        sql::ResultSet* res = stmnt->executeQuery();
        while (res->next())
        {
            transactions.push_back(rowToTransaction(res));
        }
        delete res;
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error finding transactions by wallet_id: " + string(e.what()));
    }
    return transactions;
}

vector<unique_ptr<Transaction>> TransactionDBDAO::findAll()
{
    vector<unique_ptr<Transaction>> transactions;
    try
    {
        unique_ptr<sql::Statement> stmnt(connection->getConnection()->createStatement());
        sql::ResultSet* res = stmnt->executeQuery("SELECT * FROM MOVIMENTACAO");
        while (res->next())
        {
            transactions.push_back(rowToTransaction(res));
        }
        delete res;
    }
    catch (sql::SQLException& e)
    {
        throw runtime_error("Error finding all transactions: " + string(e.what()));
    }
    return transactions;
} 