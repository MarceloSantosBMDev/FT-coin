#include "WalletMemDAO.h"
#include <algorithm>

using namespace std;

WalletMemDAO::WalletMemDAO() : nextId(1) {}

void WalletMemDAO::insert(Wallet& wallet)
{
    wallet.setId(nextId++);
    wallets.push_back(make_unique<Wallet>(wallet));
}

void WalletMemDAO::update(Wallet& wallet)
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
            return make_unique<Wallet>(*w);
        }
    }
    return nullptr;
}

vector<unique_ptr<Wallet>> WalletMemDAO::findAll()
{
    vector<unique_ptr<Wallet>> result;
    for (const auto& w : wallets)
    {
        if (w)
        {
            result.push_back(make_unique<Wallet>(*w));
        }
    }
    return result;
}

unique_ptr<Wallet> WalletMemDAO::findByHolder(const string& holderName)
{
    for (const auto& w : wallets)
    {
        if (w && w->getHolder() == holderName)
        {
            return make_unique<Wallet>(*w);
        }
    }
    return nullptr;
} 