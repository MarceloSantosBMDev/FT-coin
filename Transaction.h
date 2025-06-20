#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

using namespace std;

/**
 * @class Transaction
 * @brief Represents a buy or sell operation of a cryptocurrency.
 *
 * This class stores the details of a transaction, including its ID, the associated
 * wallet ID, the type of operation (buy/sell), the quantity of coins, and the date
 * of the transaction.
 */
class Transaction
{
public:
    /**
     * @brief Default constructor.
     */
    Transaction();

    /**
     * @brief Constructs a new Transaction.
     * @param walletId ID of the associated wallet.
     * @param operationType Type of operation ('C' for purchase, 'V' for sale).
     * @param quantity Amount of cryptocurrency transacted.
     * @param date Date of the transaction (YYYY-MM-DD).
     */
    Transaction(int walletId, const string& operationType, double quantity, const string& date);
    
    /**
     * @brief Constructs a Transaction with an existing ID.
     * @param id The unique identifier of the transaction.
     * @param walletId ID of the associated wallet.
     * @param operationType Type of operation ('C' for purchase, 'V' for sale).
     * @param quantity Amount of cryptocurrency transacted.
     * @param date Date of the transaction (YYYY-MM-DD).
     */
    Transaction(int id, int walletId, const string& operationType, double quantity, const string& date);
    
    /**
     * @brief Default destructor.
     */
    ~Transaction() = default;

    int getId() const;
    void setId(int id);
    int getWalletId() const;
    void setWalletId(int walletId);
    const string& getOperationType() const;
    void setOperationType(const string& operationType);
    double getQuantity() const;
    void setQuantity(double quantity);
    const string& getDate() const;
    void setDate(const string& date);

private:
    int id;
    int walletId;
    string operationType; // 'C' = Purchase, 'V' = Sale
    double quantity;
    string date;          // YYYY-MM-DD
};

#endif /* TRANSACTION_H_ */ 