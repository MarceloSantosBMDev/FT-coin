#ifndef TRANSACTION_MEM_DAO_H
#define TRANSACTION_MEM_DAO_H

#include "AbstractTransactionDAO.h"
#include <vector>
#include <memory>

using namespace std;

class TransactionMemDAO : public AbstractTransactionDAO
{
public:
    TransactionMemDAO();
    void insert(Transaction& transaction) override;
    void update(Transaction& transaction) override;
    void remove(int id) override;
    void removeByWalletId(int walletId) override;
    unique_ptr<Transaction> findById(int id) override;
    vector<unique_ptr<Transaction>> findByWalletId(int walletId) override;
    vector<unique_ptr<Transaction>> findAll() override;

private:
    vector<unique_ptr<Transaction>> transactions;
    int nextId;
};

#endif /* TRANSACTION_MEM_DAO_H */ 