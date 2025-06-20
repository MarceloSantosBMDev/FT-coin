# FT-Coin - Wallet Management System

This is a coin wallet management system developed in C++.

## Requirements

- CMake 3.10 or higher
- C++ compiler with C++17 support
- MariaDB Connector

## Execution

g++ -std=c++17 -o FT-Coin.exe Main.cpp Menu.cpp Controller.cpp Wallet.cpp Transaction.cpp ConnectDB.cpp WalletMemDAO.cpp TransactionMemDAO.cpp WalletDBDAO.cpp TransactionDBDAO.cpp -I/usr/include/mariadb -lmariadbcpp -lmariadb -lpthread -ldl

## Features

- Wallet Management
  - Create new wallet
  - Edit existing wallet
  - Delete wallet
  - List wallets

- Transactions
  - Register purchases
  - Register sales

- Reports
  - Wallets by ID
  - Wallets by holder name
  - Wallet balances
  - Transaction history
  - Gains and losses

## License

Program created and produced by group A-10, composed of Gustavo Soares Almeida, Hans William Hamann, Marcelo Dos Santos Da Boa Morte, Nicolas Lourenço Mafei, and Renan Felipe Rodrigues.
