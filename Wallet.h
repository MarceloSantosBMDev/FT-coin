#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <ostream>

using namespace std;

// Represents a user's wallet for holding cryptocurrency.
class Wallet
{
public:
    /**
     * @brief Default constructor.
     */
    Wallet();

    /**
     * @brief Constructs a Wallet with a holder and broker.
     * @param holder The name of the wallet holder.
     * @param broker The name of the broker.
     */
    Wallet(const string& holder, const string& broker);

    /**
     * @brief Constructs a Wallet with an ID, holder, and broker.
     * @param id The unique identifier of the wallet.
     * @param holder The name of the wallet holder.
     * @param broker The name of the broker.
     */
    Wallet(int id, const string& holder, const string& broker);

    /**
     * @brief Default destructor.
     */
    ~Wallet() = default;

    /**
     * @brief Gets the wallet's unique ID.
     * @return The wallet ID.
     */
    int getId() const;

    /**
     * @brief Sets the wallet's unique ID.
     * @param id The new wallet ID.
     */
    void setId(int id);

    /**
     * @brief Gets the holder's name.
     * @return The name of the holder.
     */
    const string& getHolder() const;

    /**
     * @brief Sets the holder's name.
     * @param holder The new name for the holder.
     */
    void setHolder(const string& holder);

    /**
     * @brief Gets the broker's name.
     * @return The name of the broker.
     */
    const string& getBroker() const;

    /**
     * @brief Sets the broker's name.
     * @param broker The new name for the broker.
     */
    void setBroker(const string& broker);

private:
    int id;
    string holder;
    string broker;
};

/**
 * @brief Overloads the << operator to print Wallet details to an ostream.
 * @param os The output stream.
 * @param wallet The Wallet object to print.
 * @return The output stream with the wallet's details.
 */
ostream& operator<<(ostream& os, const Wallet& wallet);

#endif /* WALLET_H_ */ 