#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "AbstractWalletDAO.h"
#include "AbstractTransactionDAO.h"
#include "DataBaseSelector.h"
#include "ConnectDB.h"
#include "Transaction.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

using namespace std;

/**
 * @struct WalletCashFlowDetails
 * @brief Holds calculated financial details for a wallet.
 */
struct WalletCashFlowDetails
{
	double totalPurchases;
	double totalSales;
	double netFlow;
};

/**
 * @class Controller
 * @brief Main application logic handler.
 *
 * Orchestrates the user interface, data access, and business logic.
 * It connects the menu actions to the corresponding data operations.
 */
class Controller
{
public:
	/**
	 * @brief Initializes the controller and DAOs based on the selected database type.
	 * @param type The database type (MEMORY or DATABASE).
	 */
	Controller(DataBaseSelector type);

	/**
	 * @brief Destructor.
	 */
	~Controller();

	/**
	 * @brief Starts the main application loop.
	 */
	void start();

private:
	// Menu Action Handlers
	void walletActions();
	void transactionActions();
	void reportActions();
	void oracleReportActions();
	void helpActions();
	void specificWalletReportActions();
	void launchActions(string title, vector<string> menuItems, vector<void (Controller::*)()> functions);

	// Wallet Operations
	void newWallet();
	void editWallet();
	void deleteWallet();
	void listWallets();
	void findByHolder();
	void listAllWallets();

	// Transaction Operations
	void newPurchase();
	void newSale();

	// Report Generation
	void reportGlobalBalance();
	void reportHistory();
	void reportGainsAndLosses();
	void reportWallet();
	void reportWalletGainsAndLosses();
	void reportWalletTransactionsAndCoins();
	void listQuotesHistory();
	void generalQuoteReport();
	void listQuoteByDate();

	// UI and Helper Functions
	void showSystemHelp();
	void showCredits();
	void wait(int seconds);
	void interactiveWait(int seconds);
	double getQuote(const string& date);
	double getWalletCoinQuantity(int walletId);
	WalletCashFlowDetails getWalletCashFlowDetails(int walletId);
	void createQuote(const string& date, double quote);
	void printTransactions(const vector<unique_ptr<Transaction>>& transactions);
	void printSlowly(const string& text, int delay_ms = 30);
	bool isValidDate(int d, int m, int y);
	template<typename T>
	T getValidatedInput(const string& prompt);
	string getValidatedDate(const string& prompt);

	// Member variables
	unique_ptr<ConnectDB> dbConnection;
	unique_ptr<AbstractWalletDAO> walletDAO;
	unique_ptr<AbstractTransactionDAO> transactionDAO;
	DataBaseSelector type;
	map<string, double> oracleMem; // In-memory storage for quotes
};

#endif // CONTROLLER_H