#ifndef ABSTRACT_TRANSACTION_DAO_H
#define ABSTRACT_TRANSACTION_DAO_H

#include "Transaction.h"
#include <vector>
#include <string>
#include <memory>

using namespace std;

/**
 * @class AbstractTransactionDAO
 * @brief Defines the interface for data access operations related to Transactions.
 *
 * This abstract class provides a contract for Transaction Data Access Objects (DAOs),
 * ensuring that concrete implementations (like memory or database) follow a
 * standard set of operations for transaction management.
 */
class AbstractTransactionDAO
{
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~AbstractTransactionDAO() = default;

    /**
     * @brief Inserts a new transaction into the data source.
     * @param transaction The Transaction object to insert.
     */
    virtual void insert(Transaction& transaction) = 0;

    /**
     * @brief Updates an existing transaction in the data source.
     * @param transaction The Transaction object to update.
     */
    virtual void update(Transaction& transaction) = 0;

    /**
     * @brief Removes a transaction from the data source by its ID.
     * @param id The ID of the transaction to remove.
     */
    virtual void remove(int id) = 0;

    /**
     * @brief Removes all transactions associated with a specific wallet ID.
     * @param walletId The ID of the wallet whose transactions are to be removed.
     */
    virtual void removeByWalletId(int walletId) = 0;

    /**
     * @brief Finds a transaction by its unique ID.
     * @param id The ID of the transaction to find.
     * @return A unique_ptr to the Transaction if found, otherwise nullptr.
     */
    virtual unique_ptr<Transaction> findById(int id) = 0;

    /**
     * @brief Retrieves all transactions for a specific wallet.
     * @param walletId The ID of the wallet.
     * @return A vector of unique_ptrs to the Wallet's Transaction objects.
     */
    virtual vector<unique_ptr<Transaction>> findByWalletId(int walletId) = 0;

    /**
     * @brief Retrieves all transactions from the data source.
     * @return A vector of unique_ptrs to all Transaction objects.
     */
    virtual vector<unique_ptr<Transaction>> findAll() = 0;
};

#endif // ABSTRACT_TRANSACTION_DAO_H 