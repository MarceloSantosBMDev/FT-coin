#include "Controller.h"
#include "Menu.h"
#include "CarteiraMemDAO.h"
#include "CarteiraBDDAO.h"
#include "MovimentacaoMemDAO.h"
#include "MovimentacaoBDDAO.h"
#include "ConnectDB.h"
#include "cores.h"
#include "Carteira.h"
#include "Movimentacao.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <mariadb/conncpp.hpp>
#include <cstdlib>
#include <ctime>
#include <random>
#include <limits>

using namespace std;

Controller::Controller(DataBaseSelector dbType) : _type(dbType)
{
    if (dbType == DataBaseSelector::MEMORY)
    {
        _carteiraDAO = make_unique<CarteiraMemDAO>();
        _movimentacaoDAO = make_unique<MovimentacaoMemDAO>();
    }
    else
    {
        _dbConnection = make_unique<ConnectDB>();
        _carteiraDAO = make_unique<CarteiraBDDAO>(_dbConnection.get());
        _movimentacaoDAO = make_unique<MovimentacaoBDDAO>(_dbConnection.get());
    }
}

Controller::~Controller() {}

void Controller::start()
{
    Menu menu;
    menu.setTitle("FT-Coin - Sistema de Controle de Carteiras");
    menu.addOption("Gerenciar Carteiras");
    menu.addOption("Compra & Venda de Moedas");
    menu.addOption("Relatorios");
    menu.addOption("Ajuda");
    menu.addOption("Sair");

    while (true)
    {
        int opcao = menu.getOption();
        switch (opcao)
        {
        case 1: actionCarteira(); break;
        case 2: actionMovimentacao(); break;
        case 3: actionRelatorios(); break;
        case 4: actionAjuda(); break;
        case 5: return;
        }
    }
}

void Controller::actionCarteira()
{
    vector<string> menuItens = {
        "Nova Carteira", "Editar Carteira", "Excluir Carteira", "Listar Carteiras", "Voltar"};

    vector<void (Controller::*)()> functions = {
        &Controller::newCarteira, &Controller::editCarteira, &Controller::deleteCarteira, &Controller::listCarteira};

    launchActions("Gerenciamento de Carteiras", menuItens, functions);
}

void Controller::actionMovimentacao()
{
    vector<string> menuItens = {"Registrar Compra", "Registrar Venda", "Voltar"};
    vector<void (Controller::*)()> functions = {&Controller::newCompra, &Controller::newVenda};
    launchActions("Movimentacoes", menuItens, functions);
}

void Controller::actionRelatorios()
{
    vector<string> menuItens = {
        "Saldo das Carteiras", "Historico de Movimentacoes", "Ganhos e Perdas", "Voltar"};
    vector<void (Controller::*)()> functions = {
        &Controller::reportSaldo, &Controller::reportHistorico, &Controller::reportGanhoPerda};
    launchActions("Relatorios", menuItens, functions);
}

void Controller::actionAjuda()
{
    showHelp();
}

void Controller::launchActions(string title, vector<string> menuItens, vector<void (Controller::*)()> functions)
{
    Menu menu;
    menu.setTitle(title);
    for (const string& item : menuItens)
    {
        menu.addOption(item);
    }

    while (true)
    {
        int opcao = menu.getOption();
        if (opcao > functions.size()) return;
        (this->*functions[opcao - 1])();
    }
}

void Controller::newCarteira()
{
    cout << "Digite o titular da nova carteira: ";
    string titular;
    getline(cin >> ws, titular);

    cout << "Digite a corretora: ";
    string corretora;
    getline(cin, corretora);

    Carteira carteira(titular, corretora);
    _carteiraDAO->insert(carteira);
    cout << VERDE << "\nCarteira criada com sucesso!\n" << RESET;
}

void Controller::editCarteira()
{
    int id;
    cout << "\nID da carteira: ";
    cin >> id;

    auto carteira = _carteiraDAO->findById(id);
    if (!carteira)
    {
        cout << VERMELHO << "\nCarteira nao encontrada!\n" << RESET;
        return;
    }

    string titular;
    cout << "Novo nome do titular: ";
    getline(cin >> ws, titular);

    carteira->setTitular(titular);
    _carteiraDAO->update(*carteira);
    cout << VERDE << "\nCarteira atualizada com sucesso!\n" << RESET;
}

void Controller::deleteCarteira()
{
    int id;
    cout << "\nID da carteira: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    try {
        // Primeiro, remove todas as movimentações associadas a esta carteira
        _movimentacaoDAO->removeByCarteiraId(id);
        // Depois, remove a carteira
        _carteiraDAO->remove(id);
        cout << VERDE << "Carteira e seu historico de movimentacoes foram excluidos com sucesso!\n" << RESET;
    } catch (const runtime_error& e) {
        cerr << VERMELHO << "Erro: " << e.what() << RESET << endl;
    }
}

void Controller::listCarteira()
{
    listAll();
}

void Controller::newCompra()
{
    cout << "\nDigite o ID da carteira para a compra: ";
    int carteiraId;
    cin >> carteiraId;

    cout << "Digite a quantidade comprada: ";
    double quantidade;
    cin >> quantidade;

    cout << "Digite a data da compra (AAAA-MM-DD): ";
    string data;
    cin >> data;

    getCotacao(data); // Garante que a cotação para esta data exista ou seja criada.

    Movimentacao mov(carteiraId, "C", quantidade, data);
    _movimentacaoDAO->insert(mov);

    cout << VERDE << "\nCompra registrada com sucesso!\n" << RESET;
}

void Controller::newVenda()
{
    cout << "\nDigite o ID da carteira para a venda: ";
    int carteiraId;
    cin >> carteiraId;

    cout << "Digite a quantidade vendida: ";
    double quantidade;
    cin >> quantidade;
    
    cout << "Digite a data da venda (AAAA-MM-DD): ";
    string data;
    cin >> data;

    getCotacao(data); // Garante que a cotação para esta data exista ou seja criada.

    Movimentacao mov(carteiraId, "V", quantidade, data);
    _movimentacaoDAO->insert(mov);

    cout << VERDE << "\nVenda registrada com sucesso!\n" << RESET;
}

void Controller::reportSaldo()
{
    auto carteiras = _carteiraDAO->findAll();
    if (carteiras.empty()) {
        cout << "\nNenhuma carteira encontrada.\n";
        return;
    }

    cout << "\n--- Posicao das Carteiras ---\n";
    cout << setw(5) << "ID" << setw(20) << "Titular" << setw(20) << "Qtd. Moedas" << setw(20) << "Fluxo Caixa (R$)" << endl;
    cout << string(65, '-') << endl;

    for (const auto& carteira : carteiras) {
        double quantidade = getCarteiraQuantidadeMoedas(carteira->getId());
        double fluxoCaixa = getCarteiraFluxoCaixa(carteira->getId());
        cout << setw(5) << carteira->getId() 
                  << setw(20) << carteira->getTitular() 
                  << setw(20) << fixed << setprecision(8) << quantidade
                  << setw(20) << fixed << setprecision(2) << fluxoCaixa << endl;
    }
}

void Controller::reportHistorico()
{
    auto movimentacoes = _movimentacaoDAO->findAll();
    if (movimentacoes.empty()) {
        cout << "\nNenhuma movimentacao encontrada.\n";
        return;
    }

    cout << "\n--- Historico de Movimentacoes ---\n";
    cout << setw(5) << "ID" 
              << setw(10) << "ID Cart."
              << setw(15) << "Tipo"
              << setw(15) << "Quantidade"
              << setw(15) << "Data" << endl;
    cout << string(60, '-') << endl;

    for (const auto& mov : movimentacoes) {
        cout << setw(5) << mov->getId() 
                  << setw(10) << mov->getCarteiraId() 
                  << setw(15) << (mov->getTipoOperacao() == "C" ? "Compra" : "Venda")
                  << setw(15) << mov->getQuantidade()
                  << setw(15) << mov->getData() << endl;
    }
}

void Controller::reportGanhoPerda()
{
    auto movimentacoes = _movimentacaoDAO->findAll();
    if (movimentacoes.empty()) {
        cout << "\nNenhuma movimentacao encontrada para calcular ganhos/perdas.\n";
        return;
    }

    double totalCompras = 0;
    double totalVendas = 0;

    for (const auto& mov : movimentacoes) {
        double cotacao = getCotacao(mov->getData());
        if (mov->getTipoOperacao() == "C") {
            totalCompras += mov->getQuantidade() * cotacao;
        } else {
            totalVendas += mov->getQuantidade() * cotacao;
        }
    }

    cout << "\n--- Relatorio de Ganhos e Perdas ---\n";
    cout << "Valor Total de Compras: R$ " << fixed << setprecision(2) << totalCompras << endl;
    cout << "Valor Total de Vendas:  R$ " << fixed << setprecision(2) << totalVendas << endl;
    cout << "Ganho/Perda:              R$ " << fixed << setprecision(2) << (totalVendas - totalCompras) << endl;
}

void Controller::showHelp()
{
    cout << "\n=== Ajuda ===\n";
    cout << "Este e o sistema de gerenciamento de carteiras de criptomoedas FT-Coin.\n";
    cout << "Use as opcoes do menu para navegar.\n";
}

void Controller::listAll()
{
    auto carteiras = _carteiraDAO->findAll();
    if (carteiras.empty()) {
        cout << "Nenhuma carteira cadastrada.\n";
        return;
    }

    cout << "\n--- Todas as Carteiras ---\n";
    cout << setw(5) << "ID" 
              << setw(20) << "Titular" 
              << setw(15) << "Corretora"
              << setw(20) << "Qtd. Moedas" 
              << setw(20) << "Fluxo Caixa (R$)" << endl;
    cout << string(80, '-') << endl;

    for (const auto& carteira : carteiras) {
        double quantidade = getCarteiraQuantidadeMoedas(carteira->getId());
        double fluxoCaixa = getCarteiraFluxoCaixa(carteira->getId());

        cout << setw(5) << carteira->getId() 
                  << setw(20) << carteira->getTitular() 
                  << setw(15) << carteira->getCorretora()
                  << setw(20) << fixed << setprecision(8) << quantidade
                  << setw(20) << fixed << setprecision(2) << fluxoCaixa << endl;
    }
}

void Controller::findByNome() {
    cout << "Digite o nome do titular a ser buscado: ";
    string nome;
    getline(cin >> ws, nome);

    auto carteira = _carteiraDAO->findByTitular(nome);
    if (carteira) {
        cout << "\nCarteira encontrada:\n";
        cout << *carteira << endl;
    } else {
        cout << "\nCarteira nao encontrada.\n";
    }
}

double Controller::getCarteiraQuantidadeMoedas(int carteiraId)
{
    auto movimentacoes = _movimentacaoDAO->findByCarteiraId(carteiraId);
    double quantidade = 0;
    for (const auto& mov : movimentacoes) {
        if (mov->getTipoOperacao() == "C") {
            quantidade += mov->getQuantidade();
        } else {
            quantidade -= mov->getQuantidade();
        }
    }
    return quantidade;
}

double Controller::getCarteiraFluxoCaixa(int carteiraId)
{
    auto movimentacoes = _movimentacaoDAO->findByCarteiraId(carteiraId);
    double fluxoCaixa = 0;
    for (const auto& mov : movimentacoes) {
        double cotacao = getCotacao(mov->getData());
        if (mov->getTipoOperacao() == "C") {
            fluxoCaixa -= mov->getQuantidade() * cotacao;
        } else {
            fluxoCaixa += mov->getQuantidade() * cotacao;
        }
    }
    return fluxoCaixa;
}

void Controller::createCotacao(const string& data, double cotacao)
{
    if (_type == DataBaseSelector::MEMORY)
    {
        _oracle_mem[data] = cotacao;
    }
    else
    {
        try
        {
            unique_ptr<sql::PreparedStatement> pstmt(_dbConnection->getConnection()->prepareStatement("INSERT INTO ORACULO (Data, Cotacao) VALUES (?, ?)"));
            pstmt->setString(1, data);
            pstmt->setDouble(2, cotacao);
            pstmt->executeUpdate();
            cout << "Nova cotacao de R$" << fixed << setprecision(2) << cotacao << " criada para a data " << data << endl;
        }
        catch (sql::SQLException &e)
        {
            // se cotacao ja existe para a data, apenas ignora
            if (e.getErrorCode() != 1062) // 1062 = a numero do erro para chave duplicada
                cerr << "Erro ao criar nova cotacao: " << e.what() << endl;
        }
    }
}

double Controller::getCotacao(const string& data)
{
    if (_type == DataBaseSelector::MEMORY)
    {
        if (_oracle_mem.find(data) == _oracle_mem.end())
        {
            cerr << AMARELO << "Aviso (Memoria): Nenhuma cotacao encontrada para a data " << data << ". Criando uma nova." << RESET << endl;
            random_device rd;
            mt19937 gen(rd());
            uniform_real_distribution<> distr(4.50, 5.90);
            double novaCotacao = distr(gen);
            _oracle_mem[data] = novaCotacao;
            return novaCotacao;
        }
        return _oracle_mem[data];
    }
    else
    {
        try
        {
            unique_ptr<sql::PreparedStatement> pstmt(_dbConnection->getConnection()->prepareStatement("SELECT Cotacao FROM ORACULO WHERE Data = ?"));
            pstmt->setString(1, data);
            sql::ResultSet* res = pstmt->executeQuery();
            if (res->next())
            {
                double cotacao = res->getDouble("Cotacao");
                delete res;
                return cotacao;
            }
            delete res;

            cerr << AMARELO << "Aviso: Nenhuma cotacao encontrada para a data " << data << ". Criando uma nova." << RESET << endl;
            random_device rd;
            mt19937 gen(rd());
            uniform_real_distribution<> distr(4.50, 5.90);
            double novaCotacao = distr(gen);
            createCotacao(data, novaCotacao);
            return novaCotacao;
        }
        catch (sql::SQLException &e)
        {
            cerr << "Erro ao buscar cotacao: " << e.what() << endl;
            return 5.0; // Retorna um valor padrao em caso de erro
        }
    }
}