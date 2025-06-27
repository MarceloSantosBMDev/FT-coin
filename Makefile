TARGET = FT-Coin.exe

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

INCLUDES = -I/usr/include/mariadb

LIBS = -lmariadbcpp -lmariadb -lpthread -ldl

SRCS = Main.cpp Menu.cpp Controller.cpp Wallet.cpp Transaction.cpp ConnectDB.cpp \
       WalletMemDAO.cpp TransactionMemDAO.cpp WalletDBDAO.cpp TransactionDBDAO.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(INCLUDES) $(LIBS)

clean:
	rm -f $(TARGET)