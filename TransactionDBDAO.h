#ifndef TRANSACTION_DB_DAO_H
#define TRANSACTION_DB_DAO_H

#include "AbstractTransactionDAO.h"
#include "ConnectDB.h"
#include <vector>
#include <memory>
#include <string>

using namespace std;

class TransactionDBDAO : public AbstractTransactionDAO
{
public:
    TransactionDBDAO(ConnectDB* connection);
    void insert(Transaction& transaction) override;
    void update(Transaction& transaction) override;
    void remove(int id) override;
    void removeByWalletId(int walletId) override;
    unique_ptr<Transaction> findById(int id) override;
    vector<unique_ptr<Transaction>> findByWalletId(int walletId) override;
    vector<unique_ptr<Transaction>> findAll() override;

private:
    ConnectDB* connection;
    unique_ptr<Transaction> rowToTransaction(sql::ResultSet* res);
};

#endif // TRANSACTION_DB_DAO_H 