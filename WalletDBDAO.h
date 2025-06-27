#ifndef WALLET_DB_DAO_H
#define WALLET_DB_DAO_H

#include "AbstractWalletDAO.h"
#include "ConnectDB.h"
#include <vector>
#include <memory>
#include <string>

using namespace std;

class WalletDBDAO : public AbstractWalletDAO
{
public:
    WalletDBDAO(ConnectDB* connection);
    void insert(Wallet& wallet) override;
    void update(const Wallet& wallet) override;
    void remove(int id) override;
    unique_ptr<Wallet> findById(int id) override;
    vector<unique_ptr<Wallet>> findAll() override;

private:
    ConnectDB* connection;
    unique_ptr<Wallet> rowToWallet(sql::ResultSet* res);
};

#endif // WALLET_DB_DAO_H 