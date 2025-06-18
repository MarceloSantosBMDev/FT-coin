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
	void reportGanhoPerda();

	// Outros
	void showHelp();
	double getCotacao(const string& data);
	double getCarteiraQuantidadeMoedas(int carteiraId);
	double getCarteiraFluxoCaixa(int carteiraId);
	void createCotacao(const string& data, double cotacao);
};

#endif // CONTROLLER_H