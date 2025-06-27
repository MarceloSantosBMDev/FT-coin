#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "AbstractWalletDAO.h"
#include "AbstractTransactionDAO.h"
#include "DataBaseSelector.h"
#include "ConnectDB.h"
#include "Transaction.h"
#include "Colors.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <iostream>
#include <limits>
#include <algorithm>

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

	void listWallet();
	void deleteWallet();
	void findWallet();

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
	void listWallets();
	void findByHolder();
	void listAllWallets();

	// Transaction Operations
	void newPurchase();
	void newSale();
	void listTransactionsByWallet();

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
	string getValidatedDate(const string& prompt);
	bool isValidDate(int d, int m, int y);

	// Member variables
	unique_ptr<ConnectDB> dbConnection;
	unique_ptr<AbstractWalletDAO> walletDAO;
	unique_ptr<AbstractTransactionDAO> transactionDAO;
	DataBaseSelector type;
	map<string, double> oracleMem; // In-memory storage for quotes

	void clearScreen();

	template <typename T>
	T getValidatedInput(const string& prompt);
};

// Template definitions
template <typename T>
T Controller::getValidatedInput(const string& prompt) {
	T value;
	while (true) {
		cout << prompt;
		cin >> value;
		if (cin.good()) {
			char c = cin.peek();
			if (c == '\n' || c == EOF) {
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				break;
			}
		}
		cout << endl << RED << "Invalid input. Please try again." << RESET << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	return value;
}

// Template specialization for std::string
template <>
inline string Controller::getValidatedInput<string>(const string& prompt) {
	string value;
	while (true) {
		cout << prompt;
		getline(cin >> ws, value);

		size_t first = value.find_first_not_of(" \t\n\r\f\v");
		if (string::npos == first) {
			value.clear();
		} else {
			size_t last = value.find_last_not_of(" \t\n\r\f\v");
			value = value.substr(first, (last - first + 1));
		}

		if (!value.empty()) {
			break;
		}
		cout << endl << RED << "Invalid input. Please try again." << RESET << endl;
	}
	return value;
}

#endif // CONTROLLER_H