# FT-Coin - Wallet Management System

This is a coin wallet management system developed in C++.

## Requirements

- C++ compiler with C++17 support
- MariaDB Connector

## Execution

Compilation:
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
  - Wallets global reports
  - Specific wallet report
  - Quote history
  - Transaction history
  - Global gains and losses

## License

Program created and produced by group A-10, composed of:
  - Gustavo Soares Almeida; 
  - Hans William Hamann;
  - Marcelo Dos Santos Da Boa Morte; 
  - Nicolas Lourenço Mafei;
  - Renan Felipe Rodrigues.

Developed in University of Campinas - UNICAMP, 1st semester of 2025  
© 2025 FT-Coin. All rights reserved.
