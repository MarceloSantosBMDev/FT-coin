#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "AbstractCarteiraDAO.h"
#include "AbstractMovimentacaoDAO.h"
#include "DataBaseSelector.h"
#include <string>
#include <vector>
#include <memory>
#include "ConnectDB.h"
#include <map>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <termios.h>

using namespace std;

class Controller {
public:
	Controller(DataBaseSelector type);
	~Controller();
	void start();

private:
	unique_ptr<ConnectDB> _dbConnection;
	unique_ptr<AbstractCarteiraDAO> _carteiraDAO;
	unique_ptr<AbstractMovimentacaoDAO> _movimentacaoDAO;
	DataBaseSelector _type;
	map<string, double> _oracle_mem;

	// menus
	void actionCarteira();
	void actionMovimentacao();
	void actionRelatorios();
	void actionAjuda();
	void launchActions(string title, vector<string> menuItens, vector<void (Controller::*)()> functions);

	// CRUD Carteira
	void newCarteira();
	void listCarteira();
	void editCarteira();
	void deleteCarteira();
	void findByNome();
	void listAll();

	// CRUD Movimentacao
	void newCompra();
	void newVenda();

	// Relatorios
	void reportSaldo();
	void reportHistorico();
	void reportHistoricoPorCarteira();
	void reportGanhoPerda();
	void reportCarteira();

	// Outros
	void showHelp();
	void esperar(int seconds);
	double getCotacao(const string& data);
	double getCarteiraQuantidadeMoedas(int carteiraId);
	double getCarteiraFluxoCaixa(int carteiraId);
	void createCotacao(const string& data, double cotacao);
	void printMovimentacoes(const vector<unique_ptr<Movimentacao>>& movimentacoes);
	void printSlowly(const string& text, int delay_ms = 30);

	// Input validation
	template<typename T>
	T getValidatedInput(const string& prompt);
	string getValidatedDate(const string& prompt);
};

#endif // CONTROLLER_H