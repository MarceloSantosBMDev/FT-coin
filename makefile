# Nome do executável
TARGET = FT-Coin.exe

# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Diretórios de include
INCLUDES = -I/usr/include/mariadb

# Bibliotecas externas
LIBS = -lmariadbcpp -lmariadb -lpthread -ldl

# Fontes do projeto
SRCS = Main.cpp Menu.cpp Controller.cpp Wallet.cpp Transaction.cpp ConnectDB.cpp \
       WalletMemDAO.cpp TransactionMemDAO.cpp WalletDBDAO.cpp TransactionDBDAO.cpp

# Regra principal
all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(INCLUDES) $(LIBS)

# Limpa apenas o executável
clean:
	rm -f $(TARGET)