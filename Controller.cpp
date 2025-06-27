#include "Controller.h"

// Project Headers
#include "Menu.h"
#include "WalletMemDAO.h"
#include "WalletDBDAO.h"
#include "TransactionMemDAO.h"
#include "TransactionDBDAO.h"
#include "Colors.h"
#include "Wallet.h"
#include "Transaction.h"

// External Libraries
#include <mariadb/conncpp.hpp>

// Standard C++ Libraries
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <functional>
#include <random>
#include <limits>
#include <regex>
#include <thread>
#include <chrono>

// System Libraries (C-style)
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <cstdlib>
#include <ctime>

using namespace std;

Controller::Controller(DataBaseSelector dbType)
{
    type = dbType;
    if (dbType == DataBaseSelector::MEMORY)
    {
        walletDAO = unique_ptr<WalletMemDAO>(new WalletMemDAO());
        transactionDAO = unique_ptr<TransactionMemDAO>(new TransactionMemDAO());
    }
    else
    {
        try {
            dbConnection = unique_ptr<ConnectDB>(new ConnectDB());
            walletDAO = unique_ptr<WalletDBDAO>(new WalletDBDAO(dbConnection.get()));
            transactionDAO = unique_ptr<TransactionDBDAO>(new TransactionDBDAO(dbConnection.get()));
        } catch (const exception& e) {
            cerr << "Erro ao conectar ao banco de dados: " << e.what() << endl;
            exit(1);
        }
    }
}

Controller::~Controller() {}

void Controller::start()
{
    vector<string> welcome_art = {
        "\n  ______ _______      _____ ____ _____ _   _ ",
        " |  ____|__   __|    / ____/ __ \\_   _| \\ | |",
        " |  __|    | |______| |   | |  | || | | . ` |",
        " | |       | |      | |___| |__| || |_| |\\  |",
        " |_|       |_|       \\_____\\____/_____|_| \\_|"
    };

    cout << GREEN << BOLD;
    for(const auto& line : welcome_art) {
        printSlowly(line, 10);
    }
    cout << RESET;
    printSlowly(string(CYAN) + "\nCoin Wallet Gains and Losses Calculation System" + RESET, 15);

    string mode = (type == DataBaseSelector::MEMORY) ? "Memory" : "Database";
    printSlowly(string(CYAN) + BOLD + mode + " Version 1.2.0" + RESET, 15);

    cout << endl;

    Menu menu;
    menu.setTitle("FT-Coin - Wallet Management System");
    menu.addOption("Manage Wallets");
    menu.addOption("Buy & Sell Coins");
    menu.addOption("Reports");
    menu.addOption("Help");
    menu.addOption("Exit");

    while (true)
    {
        int option = menu.getOption();
        switch (option)
        {
        case 1: walletActions(); break;
        case 2: transactionActions(); break;
        case 3: reportActions(); break;
        case 4: helpActions(); break;
        case 5:
            {
                cout << "\n";
                for (int i = 3; i > 0; --i) {
                    cout << CYAN << "Exiting the program... " << i << flush;
                    wait(1);
                    cout << "\r                                                     \r" << flush;
                }
                cout << endl;
                return;
            }
        }
    }
}

void Controller::walletActions()
{
    launchActions(
        "Wallet Actions",
        {"New Wallet", "Edit Wallet", "Delete Wallet", "List Wallets", "Find by ID", "Back"},
        {&Controller::newWallet, &Controller::editWallet, &Controller::deleteWallet, &Controller::listWallets, &Controller::findWalletById}
    );
}

void Controller::transactionActions()
{
    vector<string> menuItems = {"Register Purchase", "Register Sale", "Back"};
    vector<void (Controller::*)()> functions = {&Controller::newPurchase, &Controller::newSale};
    launchActions("Transactions", menuItems, functions);
}

void Controller::reportActions()
{
    vector<string> menuItems = {
        "Global Wallet Report",
        "Specific Wallet Report",
        "Oracle Quote Report",
        "General Transaction History",
        "General Gains and Losses History",
        "Back"};
    vector<void (Controller::*)()> functions = {
        &Controller::reportGlobalBalance,
        &Controller::specificWalletReportActions,
        &Controller::oracleReportActions,
        &Controller::reportHistory,
        &Controller::reportGainsAndLosses
        };
    launchActions("Reports", menuItems, functions);
}

void Controller::helpActions()
{
    vector<string> menuItems = {"System Help", "Credits", "Back"};
    vector<void (Controller::*)()> functions = {
        &Controller::showSystemHelp,
        &Controller::showCredits
        };
    launchActions("Help", menuItems, functions);
}

void Controller::launchActions(string title, vector<string> menuItems, vector<void (Controller::*)()> functions)
{
    Menu menu;
    menu.setTitle(title);
    for (const string& item : menuItems)
    {
        menu.addOption(item);
    }

    while (true)
    {
        int option = menu.getOption();
        // Check if the option is to go back (last item) or invalid.
        if (option < 1 || static_cast<size_t>(option) > menuItems.size()) {
             cout << RED << "Invalid option, please try again." << RESET << endl;
             continue;
        }
        if (static_cast<size_t>(option) == menuItems.size()) {
            return; // Last option is always "Back".
        }

        // Calls the controller method corresponding to the user's menu choice.
        (this->*functions[option - 1])();
    }
}

void Controller::newWallet()
{
    string holder = getValidatedInput<string>("Enter the holder for the new wallet: ");
    string broker = getValidatedInput<string>("Enter the broker: ");

    Wallet wallet(holder, broker);
    walletDAO->insert(wallet);
    cout << GREEN << "\nWallet created successfully!\n" << RESET;
    interactiveWait(2);
}

void Controller::editWallet()
{
    cout << "\n===== Edit Wallet =====" << endl;
    int id = getValidatedInput<int>("Enter the ID of the wallet you want to edit: ");

    auto wallet = walletDAO->findById(id);
    if (!wallet)
    {
        cout << RED << "\nWallet not found!\n" << RESET;
        interactiveWait(2);
        return;
    }

    Menu menu;
    menu.setTitle("Editing Wallet: " + wallet->getHolder());
    menu.addOption("Change Holder Name");
    menu.addOption("Change Broker");
    menu.addOption("Back");

    while (true)
    {
        int option = menu.getOption();
        bool updated = false;

        switch (option)
        {
            case 1: 
            {
                cout << "Enter the new holder name: ";
                string holder;
                getline(cin >> ws, holder);
                wallet->setHolder(holder);
                updated = true;
                break;
            }
            case 2: 
            {
                cout << "Enter the new broker: ";
                string broker;
                getline(cin >> ws, broker);
                wallet->setBroker(broker);
                updated = true;
                break;
            }
            case 3: 
                return;
        }

        if (updated)
        {
            walletDAO->update(*wallet);
            cout << GREEN << "\nWallet updated successfully!\n" << RESET;
            menu.setTitle("Editing Wallet: " + wallet->getHolder());
            interactiveWait(2);
        }
    }
}

void Controller::deleteWallet()
{
    int id = getValidatedInput<int>("\nEnter the wallet ID to delete: ");

    if (!walletDAO->findById(id)) {
        cout << RED << "\nWallet with ID " << id << " not found!\n" << RESET;
        interactiveWait(2);
        return;
    }

    try {
        transactionDAO->removeByWalletId(id);
        walletDAO->remove(id);
        cout << GREEN << "Wallet and its transaction history have been successfully deleted!\n" << RESET;
    } catch (const runtime_error& e) {
        cerr << RED << "Error: " << e.what() << RESET << endl;
    }
    interactiveWait(3);
}

void Controller::listWallets()
{
    auto wallets = walletDAO->findAll();
    if (wallets.empty()) {
        cout << RED << "\nNo wallets found." << RESET << endl;
        interactiveWait(3);
        return;
    }
    cout << CYAN << BOLD << "\n--- Wallet List ---\n" << RESET;
    cout << "--------------------------------------------------\n";
    cout << left << setw(5) << "ID" << setw(25) << "Holder" << "Broker" << endl;
    cout << "--------------------------------------------------\n";

    stringstream ss;
    for (const auto& wallet : wallets) {
        ss.str(""); // Clear the stringstream
        ss << left << setw(5) << wallet->getId() << setw(25) << wallet->getHolder() << wallet->getBroker();
        printSlowly(ss.str(), 5);
    }
    cout << "--------------------------------------------------\n";
    interactiveWait(5);
}

void Controller::newPurchase()
{
    cout << "\n===== New Purchase =====" << endl;
    int walletId = getValidatedInput<int>("Enter the wallet ID for the purchase: ");
    if (!walletDAO->findById(walletId)) {
        cout << RED << "\nWallet with ID " << walletId << " not found!\n" << RESET;
        interactiveWait(2);
        return;
    }
    double quantity = getValidatedInput<double>("Enter the quantity purchased: ");
    string date = getValidatedDate("Enter the purchase date (DD-MM-YYYY): ");

    getQuote(date);

    Transaction transaction(walletId, "C", quantity, date);
    transactionDAO->insert(transaction);

    cout << GREEN << "\nPurchase registered successfully!\n" << RESET;
    interactiveWait(2);
}

void Controller::newSale()
{
    int walletId = getValidatedInput<int>("\nEnter the wallet ID for the sale: ");
    if (!walletDAO->findById(walletId)) {
        cout << RED << "\nWallet with ID " << walletId << " not found!\n" << RESET;
        interactiveWait(2);
        return;
    }
    double quantity = getValidatedInput<double>("Enter the quantity sold: ");
    string date = getValidatedDate("Enter the sale date (DD-MM-YYYY): ");

    getQuote(date);

    Transaction transaction(walletId, "V", quantity, date);
    transactionDAO->insert(transaction);

    cout << GREEN << "\nSale registered successfully!\n" << RESET;
    interactiveWait(2);
}

void Controller::reportGlobalBalance()
{
    Menu menu;
    menu.setTitle("Global Wallet Report");
    menu.addOption("Sort by Total Balance");
    menu.addOption("Sort by Coin Quantity");
    menu.addOption("Sort by Holder Name");
    menu.addOption("Sort by ID");
    menu.addOption("Back");

    while (true)
    {
        int option = menu.getOption();
        if (option == 5) break;

        auto wallets = walletDAO->findAll();
        if (wallets.empty())
        {
            cout << RED << "\nNo wallets found." << RESET << endl;
            return;
        }

        struct WalletReportInfo
        {
            int id;
            string holder;
            double coinQuantity;
            double totalBalance;
        };

        vector<WalletReportInfo> reportData;
        for (const auto& wallet : wallets)
        {
            double coinQuantity = getWalletCoinQuantity(wallet->getId());
            auto cashFlowDetails = getWalletCashFlowDetails(wallet->getId());
            auto transactions = transactionDAO->findByWalletId(wallet->getId());

            double coinAssets = 0.0;
            if (!transactions.empty())
            {
                // Use last transaction's date for current asset value.
                auto it_max = max_element(transactions.cbegin(), transactions.cend(), 
                    [](const unique_ptr<Transaction>& a, const unique_ptr<Transaction>& b) {
                        return a->getDate() < b->getDate();
                    });
                string lastTransactionDate = (*it_max)->getDate();
                coinAssets = coinQuantity * getQuote(lastTransactionDate);
            }

            double totalBalance = coinAssets + cashFlowDetails.netFlow;

            reportData.push_back({
                wallet->getId(),
                wallet->getHolder(),
                coinQuantity,
                totalBalance
            });
        }

        switch (option)
        {
            case 1: 
                sort(reportData.begin(), reportData.end(), [](const WalletReportInfo& a, const WalletReportInfo& b) {
                    return a.totalBalance > b.totalBalance;
                });
                break;
            case 2: 
                sort(reportData.begin(), reportData.end(), [](const WalletReportInfo& a, const WalletReportInfo& b) {
                    return a.coinQuantity > b.coinQuantity;
                });
                break;
            case 3: 
                sort(reportData.begin(), reportData.end(), [](const WalletReportInfo& a, const WalletReportInfo& b) {
                    return a.holder < b.holder;
                });
                break;
            case 4: 
                sort(reportData.begin(), reportData.end(), [](const WalletReportInfo& a, const WalletReportInfo& b) {
                    return a.id < b.id;
                });
                break;
        }

        cout << "\n------------------------------------------------------------------------\n";
        cout << left << setw(5) << "ID" << setw(20) << "Holder" << setw(20) << "Coin Qty" << "Total Balance (R$)" << endl;
        cout << "------------------------------------------------------------------------\n";

        stringstream ss;
        for (const auto& info : reportData)
        {
            ss.str(""); // Clear stream
            ss << left << setw(5) << info.id << setw(20) << info.holder 
               << setw(20) << fixed << setprecision(8) << info.coinQuantity;
            
            if (info.totalBalance >= 0)
            {
                ss << GREEN;
            }
            else
            {
                ss << RED;
            }
            ss << fixed << setprecision(2) << info.totalBalance << RESET;
            printSlowly(ss.str(), 5);
        }
        cout << "------------------------------------------------------------------------\n";
        interactiveWait(5);
    }
}

void Controller::reportHistory()
{
    Menu menu;
    menu.setTitle("General Transaction History");
    menu.addOption("Sort by Date (Newest)");
    menu.addOption("Sort by Date (Oldest)");
    menu.addOption("Sort by Wallet ID");
    menu.addOption("Sort by Transaction ID");
    menu.addOption("Back");

    while (true)
    {
        int option = menu.getOption();
        if (option == 5) break; // Back

        auto transactions = transactionDAO->findAll();
        if (transactions.empty())
        {
            cout << RED << "\nNo transactions found." << RESET << endl;
            interactiveWait(3);
            return;
        }

        switch (option)
        {
            case 1: 
                sort(transactions.begin(), transactions.end(), [](const unique_ptr<Transaction>& a, const unique_ptr<Transaction>& b) {
                    return a->getDate() > b->getDate();
                });
                break;
            case 2: 
                sort(transactions.begin(), transactions.end(), [](const unique_ptr<Transaction>& a, const unique_ptr<Transaction>& b) {
                    return a->getDate() < b->getDate();
                });
                break;
            case 3: 
                sort(transactions.begin(), transactions.end(), [](const unique_ptr<Transaction>& a, const unique_ptr<Transaction>& b) {
                    if (a->getWalletId() != b->getWalletId()) {
                        return a->getWalletId() < b->getWalletId();
                    }
                    return a->getId() < b->getId(); // Tie-breaker
                });
                break;
            case 4: 
                sort(transactions.begin(), transactions.end(), [](const unique_ptr<Transaction>& a, const unique_ptr<Transaction>& b) {
                    return a->getId() < b->getId();
                });
                break;
        }
        printTransactions(transactions);
        interactiveWait(5);
    }
}

void Controller::reportGainsAndLosses()
{
    auto wallets = walletDAO->findAll();
    if (wallets.empty())
    {
        cout << RED << "\nNo wallets found to calculate gains/losses." << RESET << endl;
        interactiveWait(3);
        return;
    }

    cout << CYAN << BOLD << "\n--- Global Gains and Losses Report ---" << RESET << endl;
    cout << "--------------------------------------------------------\n";
    cout << left << setw(25) << "Wallet Holder" << "Balance (R$)" << endl;
    cout << "--------------------------------------------------------\n";

    double globalTotalBalance = 0.0;
    stringstream ss;

    for (const auto& wallet : wallets)
    {
        double coinQuantity = getWalletCoinQuantity(wallet->getId());
        auto cashFlowDetails = getWalletCashFlowDetails(wallet->getId());
        auto transactions = transactionDAO->findByWalletId(wallet->getId());

        double coinAssets = 0.0;
        if (!transactions.empty())
        {
            auto it_max = max_element(transactions.cbegin(), transactions.cend(),
                [](const unique_ptr<Transaction>& a, const unique_ptr<Transaction>& b) {
                    return a->getDate() < b->getDate();
                });
            string lastTransactionDate = (*it_max)->getDate();
            coinAssets = coinQuantity * getQuote(lastTransactionDate);
        }

        double walletBalance = coinAssets + cashFlowDetails.netFlow;
        globalTotalBalance += walletBalance;

        ss.str("");
        ss << left << setw(25) << wallet->getHolder();
        
        if (walletBalance >= 0) {
            ss << GREEN;
        } else {
            ss << RED;
        }
        ss << fixed << setprecision(2) << walletBalance << RESET;
        printSlowly(ss.str(), 5);
    }

    cout << "--------------------------------------------------------\n";
    ss.str("");
    ss << BOLD << left << setw(25) << "Global Gains and Losses";
    if (globalTotalBalance >= 0) {
        ss << GREEN;
    } else {
        ss << RED;
    }
    ss << fixed << setprecision(2) << globalTotalBalance << RESET;
    printSlowly(ss.str());
    cout << "--------------------------------------------------------\n";

    interactiveWait(5);
}

void Controller::specificWalletReportActions()
{
    vector<string> menuItems = {
        "Gains and Losses Report",
        "Transactions and Coins Report",
        "General Report",
        "Back"};
    vector<void (Controller::*)()> functions = {
        &Controller::reportWalletGainsAndLosses,
        &Controller::reportWalletTransactionsAndCoins,
        &Controller::reportWallet};
    launchActions("Specific Wallet Report", menuItems, functions);
}

void Controller::reportWalletGainsAndLosses()
{
    int walletId = getValidatedInput<int>("\nEnter the wallet ID: ");
    auto wallet = walletDAO->findById(walletId);
    if (!wallet)
    {
        cout << RED << "\nWallet not found!\n" << RESET;
        interactiveWait(2);
        return;
    }

    cout << CYAN << BOLD << "\n------------------------------------------------\n";
    cout << "  Gains and Losses: " << wallet->getHolder() << "\n";
    cout << "------------------------------------------------\n\n" << RESET;

    double coinQuantity = getWalletCoinQuantity(walletId);
    auto cashFlowDetails = getWalletCashFlowDetails(walletId);
    auto transactions = transactionDAO->findByWalletId(walletId);

    double coinAssets = 0;
    if (!transactions.empty())
    {
        auto it_max = max_element(transactions.cbegin(), transactions.cend(),
            [](const unique_ptr<Transaction>& a, const unique_ptr<Transaction>& b) {
                return a->getDate() < b->getDate();
            });
        string lastTransactionDate = (*it_max)->getDate();
        coinAssets = coinQuantity * getQuote(lastTransactionDate);
    }

    double totalResult = coinAssets + cashFlowDetails.netFlow;

    stringstream ss;

    ss.str("");
    ss << YELLOW << "Total Spent on Purchases (R$): " << RESET << RED << fixed << setprecision(2) << cashFlowDetails.totalPurchases << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << YELLOW << "Total Received from Sales (R$): " << RESET << GREEN << fixed << setprecision(2) << cashFlowDetails.totalSales << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << YELLOW << "\nNet Financial Balance (R$): " << RESET;
    if (cashFlowDetails.netFlow >= 0) ss << GREEN; else ss << RED;
    ss << fixed << setprecision(2) << cashFlowDetails.netFlow << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << YELLOW << "Coin Assets (R$): " << RESET;
    if (coinAssets >= 0) ss << GREEN; else ss << RED;
    ss << fixed << setprecision(2) << coinAssets << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << YELLOW << BOLD << "\nTotal Result (Gains/Losses) (R$): " << RESET;
    if (totalResult >= 0) ss << GREEN; else ss << RED;
    ss << fixed << setprecision(2) << totalResult << RESET;
    printSlowly(ss.str());
    
    interactiveWait(5);
}

void Controller::reportWalletTransactionsAndCoins()
{
    int walletId = getValidatedInput<int>("\nEnter the wallet ID: ");
    auto wallet = walletDAO->findById(walletId);
    if (!wallet)
    {
        cout << RED << "\nWallet not found!\n" << RESET;
        interactiveWait(2);
        return;
    }

    cout << CYAN << BOLD << "\n------------------------------------------------\n";
    cout << "  Transactions and Coins: " << wallet->getHolder() << "\n";
    cout << "------------------------------------------------\n\n" << RESET;

    auto transactions = transactionDAO->findByWalletId(walletId);
    
    vector<unique_ptr<Transaction>> purchases;
    vector<unique_ptr<Transaction>> sales;
    
    int purchaseOpsCount = 0;
    int saleOpsCount = 0;
    double totalCoinsPurchased = 0;
    double totalCoinsSold = 0;

    for (auto& mov : transactions)
    {
        if (mov->getOperationType() == "C")
        {
            totalCoinsPurchased += mov->getQuantity();
            purchaseOpsCount++;
            purchases.push_back(unique_ptr<Transaction>(new Transaction(*mov)));
        }
        else
        { 
            totalCoinsSold += mov->getQuantity();
            saleOpsCount++;
            sales.push_back(unique_ptr<Transaction>(new Transaction(*mov)));
        }
    }

    cout << CYAN << "\n--- Purchase Transactions ---\n" << RESET;
    if (purchases.empty())
    {
        cout << RED << "No purchase operations registered." << RESET << endl;
    }
    else
    {
        printTransactions(purchases);
    }

    cout << CYAN << "\n--- Sale Transactions ---\n" << RESET;
    if (sales.empty())
    {
        cout << RED << "No sale operations registered." << RESET << endl;
    }
    else
    {
        printTransactions(sales);
    }
    
    double currentCoinBalance = getWalletCoinQuantity(walletId);

    cout << CYAN << "\n--- Coin Summary ---\n" << RESET;
    stringstream ss;
    
    ss << YELLOW << "\nTotal Coins Purchased: " << RESET << fixed << setprecision(8) << totalCoinsPurchased;
    printSlowly(ss.str());

    ss.str("");
    ss << YELLOW << "Number of Purchase Operations: " << RESET << GREEN << purchaseOpsCount << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << YELLOW << "\nTotal Coins Sold: " << RESET << fixed << setprecision(8) << totalCoinsSold;
    printSlowly(ss.str());

    ss.str("");
    ss << YELLOW << "Number of Sale Operations: " << RESET << GREEN << saleOpsCount << RESET;
    printSlowly(ss.str());
    
    printSlowly("\n");

    ss.str("");
    ss << YELLOW << "Current Coin Balance: " << RESET << fixed << setprecision(8) << currentCoinBalance;
    printSlowly(ss.str());

    interactiveWait(5);
}

void Controller::reportWallet()
{
    int walletId = getValidatedInput<int>("\nEnter the wallet ID: ");

    auto wallet = walletDAO->findById(walletId);
    if (!wallet)
    {
        cout << RED << "\nWallet not found!\n" << RESET;
        interactiveWait(2);
        return;
    }

    cout << CYAN << BOLD << "\n------------------------------------------------\n";
    cout << "      Wallet Report: " << wallet->getHolder() << "\n";
    cout << "------------------------------------------------\n\n" << RESET;

    double coinQuantity = getWalletCoinQuantity(walletId);
    auto cashFlowDetails = getWalletCashFlowDetails(walletId);
    auto transactions = transactionDAO->findByWalletId(walletId);

    double coinAssets = 0;
    if (!transactions.empty())
    {
        auto it_max = max_element(transactions.cbegin(), transactions.cend(),
            [](const unique_ptr<Transaction>& a, const unique_ptr<Transaction>& b) {
                return a->getDate() < b->getDate();
            });
        string lastTransactionDate = (*it_max)->getDate();
        coinAssets = coinQuantity * getQuote(lastTransactionDate);
    }

    double totalResult = coinAssets + cashFlowDetails.netFlow;

    stringstream ss;
    ss << YELLOW << "Coin Balance: " << RESET << fixed << setprecision(8) << coinQuantity;
    printSlowly(ss.str());

    ss.str("");
    ss << YELLOW << "Total Spent on Purchases (R$): " << RESET << RED << fixed << setprecision(2) << cashFlowDetails.totalPurchases << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << YELLOW << "Total Received from Sales (R$): " << RESET << GREEN << fixed << setprecision(2) << cashFlowDetails.totalSales << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << YELLOW << "\nNet Financial Balance (R$): " << RESET;
    if (cashFlowDetails.netFlow >= 0) ss << GREEN; else ss << RED;
    ss << fixed << setprecision(2) << cashFlowDetails.netFlow << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << YELLOW << "Coin Assets (R$): " << RESET;
    if (coinAssets >= 0) ss << GREEN; else ss << RED;
    ss << fixed << setprecision(2) << coinAssets << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << YELLOW << BOLD << "\nTotal Result (Gains/Losses) (R$): " << RESET;
    if (totalResult >= 0) ss << GREEN; else ss << RED;
    ss << fixed << setprecision(2) << totalResult << RESET;
    printSlowly(ss.str());

    if (transactions.empty())
    {
        cout << RED << "\nNo transactions registered for this wallet." << RESET << endl;
    }
    else
    {
        cout << CYAN << "\n--- Transaction History ---\n" << RESET;
    printTransactions(transactions);
    }
    interactiveWait(5);
}

void Controller::showSystemHelp()
{
    cout << CYAN << BOLD << "\n--- FT COIN SYSTEM HELP ---\n\n" << RESET;
    
    auto printBlock = [this](const vector<string>& block) {
        for(const auto& line : block) {
            printSlowly(line, 9);
        }
    };

    vector<string> main_menu_text = {
        string(YELLOW) + "MAIN MENU:" + RESET,
        "1. Manage Wallets - Create, edit, delete, and list your coin wallets.",
        "2. Buy & Sell Coins - Register your purchase and sale operations.",
        "3. Reports - Generate global reports, by wallet, or about quotes.",
        "4. Help - Displays this help menu and system credits.",
        "5. Exit - Closes the program."
    };
    printBlock(main_menu_text);
    wait(1);

    vector<string> features_text = {
        string(YELLOW) + "\nFEATURES:" + RESET,
        "- Wallet reports allow for a detailed analysis of gains, losses, and transactions.",
        "- The quote oracle fetches and stores coin values to ensure consistency in calculations."
    };
    printBlock(features_text);
    wait(1);

    vector<string> commands_text = {
        string(YELLOW) + "\nCOMMANDS:" + RESET,
        "- Use the corresponding numbers to navigate through the menus.",
        "- Follow the instructions displayed on each screen to enter data.",
        "- Dates must be entered in the DD-MM-YYYY format."
    };
    printBlock(commands_text);
    wait(1);
    
    vector<string> more_info_text = {
        string(YELLOW) + "\nFOR MORE INFORMATION:" + RESET,
        "Consult the project documentation or contact the development team."
    };
    printBlock(more_info_text);

    interactiveWait(5);
}

void Controller::showCredits()
{
    vector<string> credits_art = {
        "   _____ _____  ______ _____ _____ _______ _____ ",
        "  / ____|  __ \\|  ____|  __ \\_   _|__   __/ ____|",
        " | |    | |__) | |__  | |  | || |    | | | (___  ",
        " | |    |  _  /|  __| | |  | || |    | |  \\___ \\ ",
        " | |____| | \\ \\| |____| |__| || |_   | |  ____) |",
        "  \\_____|_|  \\_\\______|_____/_____|  |_| |_____/ \n",
    };

    cout << CYAN << BOLD;
    for(const auto& line : credits_art) {
        printSlowly(line, 8);
    }
    cout << RESET;

    auto printBlock = [this](const vector<string>& block) {
        for(const auto& line : block) {
            printSlowly(line, 9);
        }
    };

    vector<string> info_text = {
        "\nFT COIN SYSTEM",
        "Version: 1.2.0",
        "Release Date: June 2025"
    };
    printBlock(info_text);
    wait(1);

    vector<string> dev_text = {
        string(YELLOW) + "\nDEVELOPED BY:" + RESET,
        "Gustavo Soares Almeida\nHans William Hamann\nMarcelo Dos Santos Da Boa Morte\nNicolas Lourenço Mafei\nRenan Felipe Rodrigues"
    };
    printBlock(dev_text);
    wait(1);

    vector<string> rights_text = {
        string(YELLOW) + "\nRIGHTS:" + RESET,
        "© 2025 FT-Coin. All rights reserved."
    };
    printBlock(rights_text);
    wait(1);

    vector<string> uni_text = {
        "\nUNICAMP - University of Campinas",
        "Course: SI300 - Object-Oriented Programming I",
        "1st Semester of 2025"
    };
    printBlock(uni_text);

    interactiveWait(5);
}

void Controller::wait(int seconds)
{
    this_thread::sleep_for(chrono::seconds(seconds));
}

void Controller::interactiveWait(int seconds)
{
    // Manipulates terminal settings for non-blocking input to allow skipping wait time.
    struct termios oldt, newt;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    cout << CYAN << "\n\nPress ENTER to skip or wait to return to the menu... " << RESET << endl;

    for (int i = seconds; i > 0; --i)
    {
        cout << CYAN << "Returning in " << i << "s..." << flush;
        
        this_thread::sleep_for(chrono::seconds(1));

        int ch = getchar();
        if (ch == '\n')
        {
            break;
        }
        cout << "\r"; 
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    cout << "\r                                                     \r" << flush; 
}

void Controller::printSlowly(const string& text, int delay_ms) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delay_ms));
    }
    cout << endl;
}

void Controller::oracleReportActions()
{
    vector<string> menuItems = {
        "List Quote History",
        "List Quote by Specific Date",
        "General Quote Report",
        "Back"};
    vector<void (Controller::*)()> functions = {
        &Controller::listQuotesHistory,
        &Controller::listQuoteByDate,
        &Controller::generalQuoteReport
        };
    launchActions("Oracle Quote Report", menuItems, functions);
}

void Controller::listQuotesHistory()
{
    if (type == DataBaseSelector::DATABASE)
    {
        try
        {
            unique_ptr<sql::Statement> stmnt(dbConnection->getConnection()->createStatement());
            sql::ResultSet* res = stmnt->executeQuery("SELECT DATE_FORMAT(Data, '%d-%m-%Y') as FormattedDate, Cotacao FROM ORACULO ORDER BY Data ASC");
            
            cout << CYAN << BOLD << "\n--- Oracle Quote History (DB) ---\n" << RESET;
            cout << "---------------------------------------\n";
            cout << left << setw(15) << "Date" << "Quote (R$)" << endl;
            cout << "---------------------------------------\n";
            
            while (res->next())
            {
                cout << left << setw(15) << res->getString("FormattedDate") << fixed << setprecision(2) << res->getDouble("Cotacao") << endl;
            }
            cout << "---------------------------------------\n";
            delete res;
        }
        catch (sql::SQLException& e)
        {
            cerr << RED << "Error querying the database: " << e.what() << RESET << endl;
        }
    }
    else
    { // MEMORY
        if (oracleMem.empty())
        {
            cout << RED << "\nNo quotes registered in the oracle." << RESET << endl;
            interactiveWait(3);
            return;
        }

        vector<pair<string, double>> quotes;
        for(const auto& p : oracleMem)
        {
            quotes.push_back(p);
        }

        sort(quotes.begin(), quotes.end(), [](const pair<string, double>& a, const pair<string, double>& b) {
            return a.first < b.first; // Dates are YYYY-MM-DD, so direct comparison works
        });

        cout << CYAN << BOLD << "\n--- Oracle Quote History (Memory) ---\n" << RESET;
        cout << "---------------------------------------\n";
        cout << left << setw(15) << "Date" << "Quote (R$)" << endl;
        cout << "---------------------------------------\n";
        
        for (const auto& quote : quotes)
        {
            string date_db = quote.first; // YYYY-MM-DD
            string date_display = date_db.substr(8, 2) + "-" + date_db.substr(5, 2) + "-" + date_db.substr(0, 4);
            cout << left << setw(15) << date_display << fixed << setprecision(2) << quote.second << endl;
        }
        cout << "---------------------------------------\n";
    }
    interactiveWait(5);
}

void Controller::generalQuoteReport()
{
    if (type == DataBaseSelector::DATABASE)
    {
        try
        {
            unique_ptr<sql::Statement> stmnt(dbConnection->getConnection()->createStatement());
            
            double maxQuote = 0.0, minQuote = 0.0, avgQuote = 0.0;
            string maxDate, minDate;
            bool hasQuotes = false;

            // Query for MAX quote, read result, then close.
            sql::ResultSet* resMax = stmnt->executeQuery("SELECT Cotacao, DATE_FORMAT(Data, '%d-%m-%Y') as FormattedDate FROM ORACULO ORDER BY Cotacao DESC LIMIT 1");
            if (resMax->next()) {
                maxQuote = static_cast<double>(resMax->getDouble("Cotacao"));
                maxDate = resMax->getString("FormattedDate");
                hasQuotes = true;
            }
            delete resMax;

            if (hasQuotes) {
                // Query for MIN quote
                sql::ResultSet* resMin = stmnt->executeQuery("SELECT Cotacao, DATE_FORMAT(Data, '%d-%m-%Y') as FormattedDate FROM ORACULO ORDER BY Cotacao ASC LIMIT 1");
                if (resMin->next()) {
                    minQuote = static_cast<double>(resMin->getDouble("Cotacao"));
                    minDate = resMin->getString("FormattedDate");
                }
                delete resMin;

                // Query for AVG quote
                sql::ResultSet* resAvg = stmnt->executeQuery("SELECT AVG(Cotacao) as Average FROM ORACULO");
                if (resAvg->next()) {
                    avgQuote = static_cast<double>(resAvg->getDouble("Average"));
                }
                delete resAvg;

                cout << CYAN << BOLD << "\n--- General Quote Report (DB) ---\n\n" << RESET;
                stringstream ss;

                ss << YELLOW << "Max Quote: " << RESET << GREEN << fixed << setprecision(2) << maxQuote << " (on " << maxDate << ")" << RESET;
                printSlowly(ss.str());

                ss.str("");
                ss << YELLOW << "Min Quote: " << RESET << RED << fixed << setprecision(2) << minQuote << " (on " << minDate << ")" << RESET;
                printSlowly(ss.str());

                ss.str("");
                ss << YELLOW << "Average Quote:  " << RESET << fixed << setprecision(2) << avgQuote;
                printSlowly(ss.str());
            }
            else
            {
                 cout << RED << "\nNo quotes registered in the oracle." << RESET << endl;
            }
        }
        catch (sql::SQLException& e)
        {
            cerr << RED << "Error querying the database: " << e.what() << RESET << endl;
        }

    }
    else
    { // MEMORY
        if (oracleMem.empty())
        {
            cout << RED << "\nNo quotes registered in the oracle." << RESET << endl;
            interactiveWait(3);
            return;
        }

        double maxVal = -1.0;
        string maxDate;
        double minVal = numeric_limits<double>::max();
        string minDate;
        double sum = 0.0;

        for (const auto& pair : oracleMem)
        {
            if (pair.second > maxVal)
            {
                maxVal = pair.second;
                maxDate = pair.first;
            }
            if (pair.second < minVal)
            {
                minVal = pair.second;
                minDate = pair.first;
            }
            sum += pair.second;
        }

        double average = sum / static_cast<double>(oracleMem.size());

        string maxDate_display = maxDate.substr(8, 2) + "-" + maxDate.substr(5, 2) + "-" + maxDate.substr(0, 4);
        string minDate_display = minDate.substr(8, 2) + "-" + minDate.substr(5, 2) + "-" + minDate.substr(0, 4);

        cout << CYAN << BOLD << "\n--- General Quote Report (Memory) ---\n\n" << RESET;
        stringstream ss;

        ss << YELLOW << "Max Quote: " << RESET << GREEN << fixed << setprecision(2) << maxVal << " (on " << maxDate_display << ")" << RESET;
        printSlowly(ss.str());

        ss.str("");
        ss << YELLOW << "Min Quote: " << RESET << RED << fixed << setprecision(2) << minVal << " (on " << minDate_display << ")" << RESET;
        printSlowly(ss.str());

        ss.str("");
        ss << YELLOW << "Average Quote:  " << RESET << fixed << setprecision(2) << average;
        printSlowly(ss.str());
    }
    interactiveWait(5);
}

void Controller::listQuoteByDate()
{
    string date_db = getValidatedDate("Enter the date for the query (DD-MM-YYYY): ");
    
    double quote = getQuote(date_db);

    string date_display = date_db.substr(8,2) + "-" + date_db.substr(5,2) + "-" + date_db.substr(0,4);

    cout << "\n";
    stringstream ss;
    ss << YELLOW << "The quote for the date " << RESET << date_display << YELLOW << " is: " << RESET << GREEN << "R$ " << fixed << setprecision(2) << quote << RESET;
    printSlowly(ss.str());
    interactiveWait(3);
}

void Controller::printTransactions(const vector<unique_ptr<Transaction>>& transactions)
{
    cout << "\n----------------------------------------------------------------------------------\n";
    cout << left << setw(5) << "ID" << setw(15) << "Wallet ID" << setw(20) << "Type" << setw(25) << "Date" << "Quantity" << endl;
    cout << "----------------------------------------------------------------------------------\n";

    if (transactions.empty())
    {
        cout << RED << "No transactions found." << RESET << endl;
    }
    else
    {
        stringstream ss;
        for (const auto& mov : transactions)
        {
            ss.str(""); 
            
            string type_str;
            if (mov->getOperationType() == "C")
            {
                type_str = GREEN + string("Purchase") + RESET;
            }
            else
            {
                type_str = RED + string("Sale") + RESET;
            }

            // Displays date in DD-MM-YYYY format for readability.
            string date_db = mov->getDate(); 
            string date_display = date_db.substr(8, 2) + "-" + date_db.substr(5, 2) + "-" + date_db.substr(0, 4);

            ss << left << setw(5) << mov->getId() << setw(15) << mov->getWalletId() << setw(20) << type_str
               << setw(25) << date_display << fixed << setprecision(8) << mov->getQuantity();
            printSlowly(ss.str(), 5);
        }
    }
    cout << "----------------------------------------------------------------------------------\n";
}

void Controller::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

bool Controller::isValidDate(int d, int m, int y)
{
    if (y < 1900 || y > 2100 || m < 1 || m > 12 || d < 1 || d > 31)
    {
        return false;
    }

    if (m == 2)
    {
        bool isLeap = (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
        if (isLeap)
        {
            return d <= 29;
        }
        else
        {
            return d <= 28;
        }
    }

    if (m == 4 || m == 6 || m == 9 || m == 11)
    {
        return d <= 30;
    }

    return true;
}

string Controller::getValidatedDate(const string& prompt)
{
    string date_str;
    regex date_regex(R"(^(\d{2})-(\d{2})-(\d{4})$)");
    smatch matches;

    while (true)
    {
        cout << prompt;
        getline(cin, date_str);
        if (regex_match(date_str, matches, date_regex))
        {
            int d = stoi(matches[1].str());
            int m = stoi(matches[2].str());
            int y = stoi(matches[3].str());

            if (isValidDate(d, m, y))
            {
                // Returns date in YYYY-MM-DD for internal consistency.
                return matches[3].str() + "-" + matches[2].str() + "-" + matches[1].str();
            }
            else
            {
                cout << RED << "Invalid date. The day or month is out of range for the year." << RESET << endl;
            }
        }
        else
        {
            cout << RED << "Invalid date format. Use DD-MM-YYYY. Please try again." << RESET << endl;
        }
    }
}

void Controller::findWalletById()
{
    cout << "\n===== Find Wallet by ID =====" << endl;
    int id = getValidatedInput<int>("Enter wallet ID: ");

    unique_ptr<Wallet> wallet = walletDAO->findById(id);

    if (wallet) {
        cout << GREEN << "\nWallet found!" << RESET << endl;
        cout << "----------------------------------------" << endl;
        cout << YELLOW << "ID: " << RESET << wallet->getId() << endl;
        cout << YELLOW << "Holder: " << RESET << wallet->getHolder() << endl;
        cout << YELLOW << "Broker: " << RESET << wallet->getBroker() << endl;
        cout << "----------------------------------------" << endl;
    } else {
        cout << RED << "\nWallet with ID " << id << " not found." << RESET << endl;
    }
    interactiveWait(5);
}

double Controller::getWalletCoinQuantity(int walletId)
{
    auto transactions = transactionDAO->findByWalletId(walletId);
    double quantity = 0;
    for (const auto& mov : transactions)
    {
        if (mov->getOperationType() == "C")
        {
            quantity += mov->getQuantity();
        }
        else
        {
            quantity -= mov->getQuantity();
        }
    }
    return quantity;
}

WalletCashFlowDetails Controller::getWalletCashFlowDetails(int walletId)
{
    auto transactions = transactionDAO->findByWalletId(walletId);
    double totalPurchases = 0;
    double totalSales = 0;
    for (const auto& mov : transactions)
    {
        double transactionValue = getQuote(mov->getDate()) * mov->getQuantity();
        if (mov->getOperationType() == "C")
        {
            totalPurchases += transactionValue;
        }
        else
        {
            totalSales += transactionValue;
        }
    }
    return {totalPurchases, totalSales, totalSales - totalPurchases};
}

void Controller::createQuote(const string& date, double quote)
{
    if (type == DataBaseSelector::MEMORY)
    {
        oracleMem[date] = quote;
    }
    else
    {
        try
        {
            unique_ptr<sql::PreparedStatement> pstmt(dbConnection->getConnection()->prepareStatement("INSERT INTO ORACULO (Data, Cotacao) VALUES (?, ?)"));
            pstmt->setString(1, date);
            pstmt->setDouble(2, quote);
            pstmt->executeUpdate();
            cout << "New quote of R$" << fixed << setprecision(2) << quote << " created for date " << date.substr(8, 2) + "-" + date.substr(5, 2) + "-" + date.substr(0, 4) << endl;
        }
        catch (sql::SQLException &e)
        {
            // Ignores duplicate entry errors (code 1062).
            if (e.getErrorCode() != 1062) 
                cerr << "Error creating new quote: " << e.what() << endl;
        }
    }
}

double Controller::getQuote(const string& date)
{
    if (type == DataBaseSelector::MEMORY)
    {
        if (oracleMem.find(date) != oracleMem.end())
        {
            return oracleMem[date];
        }
        else
        {
            // Fallback: If no quote exists, create a random one.
            cout << YELLOW << "No previous quote found for the date. Generating a random one..." << RESET << endl;
            random_device rd;
            mt19937 gen(rd());
            uniform_real_distribution<> distr(2.0, 7.0); // Range from 2.0 to 7.0
            double newQuote = distr(gen);
            oracleMem[date] = newQuote; // Save the new quote
            return newQuote;
        }
    }
    else
    { // DATABASE
        double quote = 0.0;
        try
        {
            unique_ptr<sql::PreparedStatement> pstmt(dbConnection->getConnection()->prepareStatement("SELECT Cotacao FROM ORACULO WHERE Data = ?"));
            pstmt->setString(1, date);
            sql::ResultSet* res = pstmt->executeQuery();

            if (res->next())
            {
                quote = static_cast<double>(res->getDouble("Cotacao"));
            }
            else
            {
                // Fallback: If no quote exists, create a random one.
                cout << YELLOW << "No previous quote found for the date. Generating a random one..." << RESET << endl;
                random_device rd;
                mt19937 gen(rd());
                uniform_real_distribution<> distr(2.0, 7.0); // Range from 2.0 to 7.0
                quote = distr(gen);
                createQuote(date, quote); // Save the new quote to DB
            }
            delete res;
        }
        catch (sql::SQLException &e)
        {
            cerr << RED << "Error getting current quote: " << e.what() << RESET << endl;
            // Fallback in case of DB error
            cout << YELLOW << "Database error. Generating a temporary random quote..." << RESET << endl;
            random_device rd;
            mt19937 gen(rd());
            uniform_real_distribution<> distr(2.0, 7.0);
            return distr(gen);
        }
        return quote;
    }
}