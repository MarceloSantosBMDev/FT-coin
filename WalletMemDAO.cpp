#include "WalletMemDAO.h"
#include <algorithm>

using namespace std;

WalletMemDAO::WalletMemDAO() : nextId(1) {}

void WalletMemDAO::insert(Wallet& wallet)
{
    wallet.setId(wallets.size() + 1);
    wallets.push_back(unique_ptr<Wallet>(new Wallet(wallet)));
}

void WalletMemDAO::update(const Wallet& wallet)
{
    for (auto& w : wallets)
    {
        if (w && w->getId() == wallet.getId())
        {
            *w = wallet;
            return;
        }
    }
}

void WalletMemDAO::remove(int id)
{
    wallets.erase(
        remove_if(wallets.begin(), wallets.end(),
                       [id](const unique_ptr<Wallet>& w) {
                           return w && w->getId() == id;
                       }),
        wallets.end());
}

unique_ptr<Wallet> WalletMemDAO::findById(int id)
{
    for (const auto& w : wallets)
    {
        if (w && w->getId() == id)
        {
            return unique_ptr<Wallet>(new Wallet(*w));
        }
    }
    return nullptr;
}

vector<unique_ptr<Wallet>> WalletMemDAO::findAll()
{
    vector<unique_ptr<Wallet>> result;
    for (const auto& w : wallets)
    {
        result.push_back(unique_ptr<Wallet>(new Wallet(*w)));
    }
    return result;
} 