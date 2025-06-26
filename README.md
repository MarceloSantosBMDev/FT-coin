# FT-Coin - Wallet Management System

This is a coin wallet management system developed in C++.

## Requirements

- C++ compiler with C++17 support (e.g., g++)
- MariaDB Connector/C and Connector/C++ installed on Linux
- Development libraries for:
  - mariadbcpp (`<mariadb/conncpp.hpp>`)
  - POSIX system headers (already available on Linux systems)

On Debian/Ubuntu systems, you can install the required packages for MariaDB with the following command:

bash
sudo apt install g++ libmariadb-dev libmariadbcpp-dev

## Compilation Script
You can compile the program using one of the two options below:

### Option 1 – Using Makefile

Run the following command to build the executable:

`make`

To remove the generated executable:

`make clean`

### Option 2 – Using direct compilation command

Run the following command on the bash terminal:

`g++ -std=c++17 -o FT-Coin.exe Main.cpp Menu.cpp Controller.cpp Wallet.cpp Transaction.cpp ConnectDB.cpp WalletMemDAO.cpp TransactionMemDAO.cpp WalletDBDAO.cpp TransactionDBDAO.cpp -I/usr/include/mariadb -lmariadbcpp -lmariadb -lpthread -ldl`

After running either option, on the terminal, execute the following line:

`./FT-Coin.exe`
## Features

### Wallet Management
- Create new wallet
- Edit existing wallet
- Delete wallet
- List wallets
- Transactions

### Coin transaction
- Register purchases
- Register sales
- Reports

### Data Reports
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

Developed in University of Campinas - UNICAMP, 1st semester of 2025  Add commentMore actions
© 2025 FT-Coin. All rights reserved.
