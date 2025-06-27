#include "TransactionMemDAO.h"
#include <algorithm>

using namespace std;

TransactionMemDAO::TransactionMemDAO() : nextId(1) {}

void TransactionMemDAO::insert(Transaction& transaction)
{
    transaction.setId(transactions.size() + 1);
    transactions.push_back(unique_ptr<Transaction>(new Transaction(transaction)));
}

void TransactionMemDAO::update(Transaction& transaction)
{
    for (auto& trans : transactions)
    {
        if (trans && trans->getId() == transaction.getId())
        {
            *trans = transaction;
            return;
        }
    }
}

void TransactionMemDAO::remove(int id)
{
    transactions.erase(
        remove_if(transactions.begin(), transactions.end(),
                       [id](const unique_ptr<Transaction>& trans) {
                           return trans->getId() == id;
                       }),
        transactions.end());
}

void TransactionMemDAO::removeByWalletId(int walletId)
{
    transactions.erase(
        remove_if(transactions.begin(), transactions.end(),
                       [walletId](const unique_ptr<Transaction>& trans) {
                           return trans->getWalletId() == walletId;
                       }),
        transactions.end());
}

unique_ptr<Transaction> TransactionMemDAO::findById(int id)
{
    for (const auto& trans : transactions)
    {
        if (trans && trans->getId() == id)
        {
            return unique_ptr<Transaction>(new Transaction(*trans));
        }
    }
    return nullptr;
}

vector<unique_ptr<Transaction>> TransactionMemDAO::findByWalletId(int walletId)
{
    vector<unique_ptr<Transaction>> result;
    for (const auto& trans : transactions)
    {
        if (trans && trans->getWalletId() == walletId)
        {
            result.push_back(unique_ptr<Transaction>(new Transaction(*trans)));
        }
    }
    return result;
}

vector<unique_ptr<Transaction>> TransactionMemDAO::findAll()
{
    vector<unique_ptr<Transaction>> result;
    for (const auto& trans : transactions)
    {
        if (trans)
        {
            result.push_back(unique_ptr<Transaction>(new Transaction(*trans)));
        }
    }
    return result;
} 