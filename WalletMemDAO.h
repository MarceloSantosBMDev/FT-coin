#ifndef WALLET_MEM_DAO_H
#define WALLET_MEM_DAO_H

#include "AbstractWalletDAO.h"
#include <vector>
#include <memory>
#include <string>

using namespace std;

class WalletMemDAO : public AbstractWalletDAO
{
public:
    WalletMemDAO();
    void insert(Wallet& wallet) override;
    void update(Wallet& wallet) override;
    void remove(int id) override;
    unique_ptr<Wallet> findById(int id) override;
    vector<unique_ptr<Wallet>> findAll() override;
    unique_ptr<Wallet> findByHolder(const string& holderName) override;

private:
    vector<unique_ptr<Wallet>> wallets;
    int nextId;
};

#endif 