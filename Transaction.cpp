#include "Transaction.h"

using namespace std;

Transaction::Transaction() : id(0), walletId(0), operationType(""), quantity(0.0), date("") {}

Transaction::Transaction(int walletId, const string& operationType, double quantity, const string& date)
    : id(0), walletId(walletId), operationType(operationType), quantity(quantity), date(date) {}

Transaction::Transaction(int id, int walletId, const string& operationType, double quantity, const string& date)
    : id(id), walletId(walletId), operationType(operationType), quantity(quantity), date(date) {}

int Transaction::getId() const
{
    return id;
}

void Transaction::setId(int id)
{
    this->id = id;
}

int Transaction::getWalletId() const
{
    return walletId;
}

void Transaction::setWalletId(int walletId)
{
    this->walletId = walletId;
}

const string& Transaction::getOperationType() const
{
    return operationType;
}

void Transaction::setOperationType(const string& operationType)
{
    this->operationType = operationType;
}

double Transaction::getQuantity() const
{
    return quantity;
}

void Transaction::setQuantity(double quantity)
{
    this->quantity = quantity;
}

const string& Transaction::getDate() const
{
    return date;
}

void Transaction::setDate(const string& date)
{
    this->date = date;
} 