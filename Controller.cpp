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
#include <regex>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sstream>

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
    vector<string> welcome_art = {
        "\n  ______ _______      _____ ____ _____ _   _ ",
        " |  ____|__   __|    / ____/ __ \\_   _| \\ | |",
        " |  __|    | |______| |   | |  | || | | . ` |",
        " | |       | |      | |___| |__| || |_| |\\  |",
        " |_|       |_|       \\_____\\____/_____|_| \\_|"
    };

    cout << VERDE NEGRITO;
    for(const auto& line : welcome_art) {
        printSlowly(line, 5);
    }
    cout << RESET;
    printSlowly(CIANO "\nSistema de Apuracao de Ganhos e Perdas em Carteira de Moedas" RESET, 15);
    cout << endl;

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
        "Saldo das Carteiras", "Historico de Movimentacoes", "Ganhos e Perdas (Global)", "Relatorio de Carteira Especifica", "Voltar"};
    vector<void (Controller::*)()> functions = {
        &Controller::reportSaldo, &Controller::reportHistorico, &Controller::reportGanhoPerda, &Controller::reportCarteira};
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
    int id = getValidatedInput<int>("\nID da carteira que deseja editar: ");

    auto carteira = _carteiraDAO->findById(id);
    if (!carteira)
    {
        cout << VERMELHO << "\nCarteira nao encontrada!\n" << RESET;
        return;
    }

    Menu menu;
    menu.setTitle("Editando Carteira: " + carteira->getTitular());
    menu.addOption("Alterar Nome do Titular");
    menu.addOption("Alterar Corretora");
    menu.addOption("Voltar");

    while (true)
    {
        int opcao = menu.getOption();
        bool updated = false;

        switch (opcao)
        {
            case 1: // Alterar nome
            {
                cout << "Digite o novo nome do titular: ";
                string titular;
                getline(cin >> ws, titular);
                carteira->setTitular(titular);
                updated = true;
                break;
            }
            case 2: // Alterar corretora
            {
                cout << "Digite a nova corretora: ";
                string corretora;
                getline(cin >> ws, corretora);
                carteira->setCorretora(corretora);
                updated = true;
                break;
            }
            case 3: // Voltar
                return;
        }

        if (updated)
        {
            _carteiraDAO->update(*carteira);
            cout << VERDE << "\nCarteira atualizada com sucesso!\n" << RESET;
            menu.setTitle("Editando Carteira: " + carteira->getTitular());
            esperar(2);
        }
    }
}

void Controller::deleteCarteira()
{
    int id = getValidatedInput<int>("\nID da carteira: ");

    try {
        _movimentacaoDAO->removeByCarteiraId(id);
        _carteiraDAO->remove(id);
        cout << VERDE << "Carteira e seu historico de movimentacoes foram excluidos com sucesso!\n" << RESET;
    } catch (const runtime_error& e) {
        cerr << VERMELHO << "Erro: " << e.what() << RESET << endl;
    }
}

void Controller::listCarteira()
{
    listAll();
    esperar(5);
}

void Controller::newCompra()
{
    int carteiraId = getValidatedInput<int>("\nDigite o ID da carteira para a compra: ");
    double quantidade = getValidatedInput<double>("Digite a quantidade comprada: ");
    string data = getValidatedDate("Digite a data da compra (DD-MM-AAAA): ");

    getCotacao(data);

    Movimentacao mov(carteiraId, "C", quantidade, data);
    _movimentacaoDAO->insert(mov);

    cout << VERDE << "\nCompra registrada com sucesso!\n" << RESET;
}

void Controller::newVenda()
{
    int carteiraId = getValidatedInput<int>("\nDigite o ID da carteira para a venda: ");
    double quantidade = getValidatedInput<double>("Digite a quantidade vendida: ");
    string data = getValidatedDate("Digite a data da venda (DD-MM-AAAA): ");

    getCotacao(data);

    Movimentacao mov(carteiraId, "V", quantidade, data);
    _movimentacaoDAO->insert(mov);

    cout << VERDE << "\nVenda registrada com sucesso!\n" << RESET;
}

void Controller::reportSaldo()
{
    Menu menu;
    menu.setTitle("Relatório das Carteiras");
    menu.addOption("Ordenar por Saldo (Fluxo de Caixa)");
    menu.addOption("Ordenar por Quantidade de Moedas");
    menu.addOption("Ordenar por Nome do Titular");
    menu.addOption("Ordenar por ID");
    menu.addOption("Voltar");

    while (true) {
        int opcao = menu.getOption();
        if (opcao == 5) break;

        auto carteiras = _carteiraDAO->findAll();
        if (carteiras.empty()) {
            cout << "\nNenhuma carteira encontrada.\n";
            return;
        }

        struct CarteiraReportInfo {
            int id;
            string titular;
            double quantidadeMoedas;
            double fluxoCaixa;
        };

        vector<CarteiraReportInfo> reportData;
        for (const auto& carteira : carteiras) {
            reportData.push_back({
                carteira->getId(),
                carteira->getTitular(),
                getCarteiraQuantidadeMoedas(carteira->getId()),
                getCarteiraFluxoCaixa(carteira->getId())
            });
        }

        switch (opcao) {
            case 1: // Saldo
                sort(reportData.begin(), reportData.end(), [](const CarteiraReportInfo& a, const CarteiraReportInfo& b) {
                    return a.fluxoCaixa > b.fluxoCaixa;
                });
                break;
            case 2: // Moedas
                sort(reportData.begin(), reportData.end(), [](const CarteiraReportInfo& a, const CarteiraReportInfo& b) {
                    return a.quantidadeMoedas > b.quantidadeMoedas;
                });
                break;
            case 3: // Nome
                sort(reportData.begin(), reportData.end(), [](const CarteiraReportInfo& a, const CarteiraReportInfo& b) {
                    return a.titular < b.titular;
                });
                break;
            case 4: // ID
                sort(reportData.begin(), reportData.end(), [](const CarteiraReportInfo& a, const CarteiraReportInfo& b) {
                    return a.id < b.id;
                });
                break;
        }

        cout << "\n------------------------------------------------------------------------\n";
        cout << left << setw(5) << "ID" << setw(20) << "Titular" << setw(20) << "Qtd. Moedas" << "Fluxo de Caixa (R$)" << endl;
        cout << "------------------------------------------------------------------------\n";

        stringstream ss;
        for (const auto& info : reportData) {
            ss.str(""); // Limpa o stream
            ss << left << setw(5) << info.id << setw(20) << info.titular 
               << setw(20) << fixed << setprecision(8) << info.quantidadeMoedas;
            
            if (info.fluxoCaixa >= 0) {
                ss << VERDE;
            } else {
                ss << VERMELHO;
            }
            ss << fixed << setprecision(2) << info.fluxoCaixa << RESET;
            printSlowly(ss.str(), 5);
        }
        cout << "------------------------------------------------------------------------\n";
        esperar(5);
    }
}

void Controller::reportHistorico()
{
    Menu menu;
    menu.setTitle("Historico de Movimentacoes");
    menu.addOption("Listar Todas as Movimentacoes");
    menu.addOption("Filtrar por ID da Carteira");
    menu.addOption("Ordenar por Data (Mais Recentes)");
    menu.addOption("Ordenar por Data (Mais Antigas)");
    menu.addOption("Voltar");

    while (true) {
        int opcao = menu.getOption();
        if (opcao == 5) break;

        auto movimentacoes = _movimentacaoDAO->findAll();
        if (movimentacoes.empty()) {
            cout << "\nNenhuma movimentacao encontrada.\n";
            esperar(3);
            return;
        }

        switch (opcao) {
            case 1: 
                // Nenhuma ordenação necessária
                break;
            case 2:
                reportHistoricoPorCarteira(); 
                continue; // reportHistoricoPorCarteira já tem sua própria espera
            case 3: // Mais recentes
                sort(movimentacoes.begin(), movimentacoes.end(), [](const unique_ptr<Movimentacao>& a, const unique_ptr<Movimentacao>& b) {
                    return a->getData() > b->getData();
                });
                break;
            case 4: // Mais antigas
                sort(movimentacoes.begin(), movimentacoes.end(), [](const unique_ptr<Movimentacao>& a, const unique_ptr<Movimentacao>& b) {
                    return a->getData() < b->getData();
                });
                break;
        }
        printMovimentacoes(movimentacoes);
        esperar(5);
    }
}

void Controller::reportGanhoPerda()
{
    auto movimentacoes = _movimentacaoDAO->findAll();
    if (movimentacoes.empty()) {
        cout << "\nNenhuma movimentacao encontrada para calcular ganhos/perdas.\n";
        esperar(3);
        return;
    }

    double totalCompras = 0;
    double totalVendas = 0;

    for (const auto& mov : movimentacoes) {
        double valor = getCotacao(mov->getData()) * mov->getQuantidade();
        if (mov->getTipoOperacao() == "C") {
            totalCompras += valor;
        } else {
            totalVendas += valor;
        }
    }
    
    cout << CIANO NEGRITO << "\n--- Relatorio de Ganhos e Perdas (Global) ---" << RESET << endl;

    stringstream ss;
    ss << "Total de Compras: R$ " << fixed << setprecision(2) << totalCompras;
    printSlowly(ss.str());

    ss.str("");
    ss << "Total de Vendas: R$ " << fixed << setprecision(2) << totalVendas;
    printSlowly(ss.str());

    cout << "----------------------------------------\n";

    double resultado = totalVendas - totalCompras;

    ss.str("");
    ss << "Resultado (Ganhos/Perdas): ";
    if (resultado >= 0) {
        ss << VERDE;
    } else {
        ss << VERMELHO;
    }
    ss << "R$ " << fixed << setprecision(2) << resultado << RESET;
    printSlowly(ss.str());
    cout << "----------------------------------------\n";

    esperar(5);
}

void Controller::reportCarteira()
{
    int id = getValidatedInput<int>("\nDigite o ID da carteira para o relatorio: ");

    auto carteira = _carteiraDAO->findById(id);
    if (!carteira) {
        cout << VERMELHO << "\nCarteira nao encontrada!\n" << RESET;
        return;
    }

    cout << CIANO NEGRITO << "\n--- Relatorio da Carteira " << id << " ---" << RESET << endl;

    stringstream ss;
    ss << "Titular: " << carteira->getTitular();
    printSlowly(ss.str());

    ss.str("");
    ss << "Corretora: " << carteira->getCorretora();
    printSlowly(ss.str());

    double quantidadeMoedas = getCarteiraQuantidadeMoedas(id);
    double fluxoCaixa = getCarteiraFluxoCaixa(id);
    ss.str("");
    ss << "Saldo de Moedas: " << fixed << setprecision(8) << quantidadeMoedas;
    printSlowly(ss.str());
    
    ss.str("");
    ss << "Fluxo de Caixa: ";
    if (fluxoCaixa >= 0) ss << VERDE; else ss << VERMELHO;
    ss << "R$ " << fixed << setprecision(2) << fluxoCaixa << RESET;
    printSlowly(ss.str());

    double ganhoPerda = 0;
    auto movimentacoes = _movimentacaoDAO->findByCarteiraId(id);
    vector<unique_ptr<Movimentacao>> vendas;
    for(const auto& mov : movimentacoes) {
        if(mov->getTipoOperacao() == "V") {
            vendas.push_back(make_unique<Movimentacao>(*mov));
        }
    }

    if (!vendas.empty()) {
        for (const auto& venda : vendas) {
            double custoMedio = 0;
            double totalComprado = 0;
            double moedasCompradas = 0;

            for (const auto& mov : movimentacoes) {
                if (mov->getTipoOperacao() == "C" && mov->getData() <= venda->getData()) {
                    totalComprado += getCotacao(mov->getData()) * mov->getQuantidade();
                    moedasCompradas += mov->getQuantidade();
                }
            }
            custoMedio = (moedasCompradas > 0) ? totalComprado / moedasCompradas : 0;
            
            double valorVenda = getCotacao(venda->getData()) * venda->getQuantidade();
            double custoDaVenda = custoMedio * venda->getQuantidade();
            ganhoPerda += (valorVenda - custoDaVenda);
        }
    }

    ss.str("");
    ss << "Ganhos/Perdas da Carteira: ";
    if (ganhoPerda >= 0) ss << VERDE; else ss << VERMELHO;
    ss << "R$ " << fixed << setprecision(2) << ganhoPerda << RESET;
    printSlowly(ss.str());

    cout << "\n--- Historico de Movimentacoes da Carteira ---\n";
    printMovimentacoes(movimentacoes);
    esperar(5);
}

void Controller::showHelp()
{
    cout << CIANO NEGRITO << "\n--- Ajuda: FT-Coin ---" << RESET << endl;
    cout << "Preciso Fazer\n";

    esperar(5);
}

void Controller::listAll()
{
    auto carteiras = _carteiraDAO->findAll();
    if (carteiras.empty())
    {
        cout << "\nNenhuma carteira encontrada.\n";
        return;
    }
    cout << "\n--- Lista de Carteiras ---\n";
    cout << "--------------------------------------------------\n";
    cout << left << setw(5) << "ID" << setw(25) << "Titular" << "Corretora" << endl;
    cout << "--------------------------------------------------\n";
    
    stringstream ss;
    for (const auto& carteira : carteiras)
    {
        ss.str("");
        ss << left << setw(5) << carteira->getId() << setw(25) << carteira->getTitular() << carteira->getCorretora();
        printSlowly(ss.str(), 5);
    }
    cout << "--------------------------------------------------\n";
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
            if (e.getErrorCode() != 1062)
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
            cerr << AMARELO << "\nAviso (Memoria): Nenhuma cotacao encontrada para a data " << data << ". Criando uma nova." << RESET << endl;
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

            cerr << AMARELO << "\nAviso: Nenhuma cotacao encontrada para a data " << data << ". Criando uma nova." << RESET << endl;
            random_device rd;
            mt19937 gen(rd());
            uniform_real_distribution<> distr(4.50, 5.90);
            double novaCotacao = distr(gen);
            createCotacao(data, novaCotacao);
            return novaCotacao;
        }
        catch (sql::SQLException &e)
        {
            cerr << "\nErro ao buscar cotacao: " << e.what() << endl;
            return 5.0;
        }
    }
}

void Controller::reportHistoricoPorCarteira()
{
    int carteiraId = getValidatedInput<int>("\nDigite o ID da carteira: ");
    
    auto movimentacoes = _movimentacaoDAO->findByCarteiraId(carteiraId);
    printMovimentacoes(movimentacoes);
}

void Controller::printMovimentacoes(const vector<unique_ptr<Movimentacao>>& movimentacoes)
{
    cout << "\n----------------------------------------------------------------------------------\n";
    cout << left << setw(5) << "ID" << setw(15) << "ID Carteira" << setw(20) << "Tipo" << setw(25) << "Data" << "Quantidade" << endl;
    cout << "----------------------------------------------------------------------------------\n";

    if (movimentacoes.empty()) {
        cout << "Nenhuma movimentacao encontrada.\n";
    } else {
        stringstream ss;
        for (const auto& mov : movimentacoes) {
            ss.str(""); // Limpa o stream
            
            string tipo_str;
            if (mov->getTipoOperacao() == "C") {
                tipo_str = VERDE + string("Compra") + RESET;
            } else {
                tipo_str = VERMELHO + string("Venda") + RESET;
            }

            ss << left << setw(5) << mov->getId() << setw(15) << mov->getCarteiraId() << setw(20) << tipo_str
               << setw(25) << mov->getData() << fixed << setprecision(8) << mov->getQuantidade();
            printSlowly(ss.str(), 5);
        }
    }
    cout << "----------------------------------------------------------------------------------\n";
}

template<typename T>
T Controller::getValidatedInput(const string& prompt) {
    T value;
    cout << prompt;
    while (!(cin >> value)) {
        cout << VERMELHO << "\nEntrada inválida. Por favor, digite um valor numérico." << RESET << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << prompt;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

string Controller::getValidatedDate(const string& prompt) {
    string date;
    regex date_regex(R"(^\d{2}-\d{2}-\d{4}$)");
    while (true) {
        cout << prompt;
        getline(cin, date);
        if (regex_match(date, date_regex)) {
            string dd = date.substr(0, 2);
            string mm = date.substr(3, 2);
            string yyyy = date.substr(6, 4);
            return yyyy + "-" + mm + "-" + dd;
        }
        cout << VERMELHO << "Formato de data invalido. Use DD-MM-AAAA. Por favor, tente novamente." << RESET << endl;
    }
}

template int Controller::getValidatedInput<int>(const string& prompt);
template double Controller::getValidatedInput<double>(const string& prompt);

void Controller::esperar(int seconds)
{
    // Salva as configurações atuais do terminal
    struct termios oldt, newt;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Desativa o modo canônico e o eco para ler as teclas imediatamente
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    cout << CIANO << "\n\nPressione ENTER para pular ou aguarde o retorno ao menu... " << RESET << endl;

    for (int i = seconds; i > 0; --i) {
        cout << CIANO << "Retornando em " << i << "s..." << flush;
        
        // Pausa por 1 segundo
        this_thread::sleep_for(chrono::seconds(1));

        // Verifica se 'Enter' foi pressionado
        char ch = getchar();
        if (ch == '\n') {
            break;
        }
        cout << "\r"; // Move o cursor para o início da linha
    }

    // Restaura as configurações originais do terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    cout << "\r                                                     \r" << flush; // Limpa a linha
}

void Controller::printSlowly(const string& text, int delay_ms)
{
    for (const char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delay_ms));
    }
    cout << endl;
}