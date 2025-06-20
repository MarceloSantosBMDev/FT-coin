#include "Wallet.h"

using namespace std;

Wallet::Wallet() : id(0), holder(""), broker("") {}

Wallet::Wallet(const string& holder, const string& broker) : id(0), holder(holder), broker(broker) {}

Wallet::Wallet(int id, const string& holder, const string& broker) : id(id), holder(holder), broker(broker) {}

int Wallet::getId() const
{
    return id;
}

void Wallet::setId(int id)
{
    this->id = id;
}

const string& Wallet::getHolder() const
{
    return holder;
}

void Wallet::setHolder(const string& holder)
{
    this->holder = holder;
}

const string& Wallet::getBroker() const
{
    return broker;
}

void Wallet::setBroker(const string& broker)
{
    this->broker = broker;
}

ostream& operator<<(ostream& os, const Wallet& wallet)
{
    os << "ID: " << wallet.getId() << ", Holder: " << wallet.getHolder() << ", Broker: " << wallet.getBroker();
    return os;
} 