#ifndef ABSTRACT_WALLET_DAO_H
#define ABSTRACT_WALLET_DAO_H

#include "Wallet.h"
#include <vector>
#include <string>
#include <memory>

using namespace std;

/**
 * @class AbstractWalletDAO
 * @brief Defines the interface for data access operations related to Wallets.
 *
 * This abstract class provides a contract for Wallet Data Access Objects (DAOs),
 * ensuring that concrete implementations (like memory or database) follow a
 * standard set of operations for wallet management.
 */
class AbstractWalletDAO
{
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~AbstractWalletDAO() = default;

    /**
     * @brief Inserts a new wallet into the data source.
     * @param wallet The Wallet object to insert.
     */
    virtual void insert(Wallet& wallet) = 0;

    /**
     * @brief Updates an existing wallet in the data source.
     * @param wallet The Wallet object to update.
     */
    virtual void update(Wallet& wallet) = 0;

    /**
     * @brief Removes a wallet from the data source by its ID.
     * @param id The ID of the wallet to remove.
     */
    virtual void remove(int id) = 0;

    /**
     * @brief Finds a wallet by its unique ID.
     * @param id The ID of the wallet to find.
     * @return A unique_ptr to the Wallet if found, otherwise nullptr.
     */
    virtual unique_ptr<Wallet> findById(int id) = 0;

    /**
     * @brief Retrieves all wallets from the data source.
     * @return A vector of unique_ptrs to all Wallet objects.
     */
    virtual vector<unique_ptr<Wallet>> findAll() = 0;

    /**
     * @brief Finds a wallet by its holder's name.
     * @param holderName The name of the wallet holder.
     * @return A unique_ptr to the Wallet if found, otherwise nullptr.
     */
    virtual unique_ptr<Wallet> findByHolder(const string& holderName) = 0;
};

#endif // ABSTRACT_WALLET_DAO_H 