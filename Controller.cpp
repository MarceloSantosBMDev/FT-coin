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
        printSlowly(line, 10);
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
        "Relatorio Global de Carteiras",
        "Relatorio de Carteira Especifica",
        "Relatorio de Cotacao do Oraculo",
        "Historico Geral de Movimentacoes",
        "Historico Geral de Ganhos e Perdas",
        "Voltar"};
    vector<void (Controller::*)()> functions = {
        &Controller::reportSaldo,
        &Controller::actionRelatorioCarteiraEspecifica,
        &Controller::actionRelatorioOraculo,
        &Controller::reportHistorico,
        &Controller::reportGanhoPerda
        };
    launchActions("Relatorios", menuItens, functions);
}

void Controller::actionAjuda()
{
    vector<string> menuItens = {"Ajuda do Sistema", "Creditos", "Voltar"};
    vector<void (Controller::*)()> functions = {
        &Controller::showSystemHelp,
        &Controller::showCredits
        };
    launchActions("Ajuda", menuItens, functions);
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
            esperarInterativo(2);
        }
    }
}

void Controller::deleteCarteira()
{
    int id = getValidatedInput<int>("\nID da carteira: ");

    if (!_carteiraDAO->findById(id)) {
        cout << VERMELHO << "\nCarteira com ID " << id << " nao encontrada!\n" << RESET;
        return;
    }

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
    esperarInterativo(5);
}

void Controller::newCompra()
{
    int carteiraId = getValidatedInput<int>("\nDigite o ID da carteira para a compra: ");
    if (!_carteiraDAO->findById(carteiraId)) {
        cout << VERMELHO << "\nCarteira com ID " << carteiraId << " nao encontrada!\n" << RESET;
        return;
    }
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
    if (!_carteiraDAO->findById(carteiraId)) {
        cout << VERMELHO << "\nCarteira com ID " << carteiraId << " nao encontrada!\n" << RESET;
        return;
    }
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
    menu.setTitle("Relatorio Global de Carteiras");
    menu.addOption("Ordenar por Saldo Total");
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
            double saldoTotal;
        };

        vector<CarteiraReportInfo> reportData;
        for (const auto& carteira : carteiras) {
            double quantidadeMoedas = getCarteiraQuantidadeMoedas(carteira->getId());
            auto fluxoDetalhado = getCarteiraFluxoCaixaDetalhado(carteira->getId());
            auto movimentacoes = _movimentacaoDAO->findByCarteiraId(carteira->getId());

            double patrimonioMoedas = 0.0;
            if (!movimentacoes.empty()) {
                auto it_max = max_element(movimentacoes.cbegin(), movimentacoes.cend(), 
                    [](const unique_ptr<Movimentacao>& a, const unique_ptr<Movimentacao>& b) {
                        return a->getData() < b->getData();
                    });
                string dataUltimaMov = (*it_max)->getData();
                patrimonioMoedas = quantidadeMoedas * getCotacao(dataUltimaMov);
            }

            double saldoTotal = patrimonioMoedas + fluxoDetalhado.fluxoLiquido;

            reportData.push_back({
                carteira->getId(),
                carteira->getTitular(),
                quantidadeMoedas,
                saldoTotal
            });
        }

        switch (opcao) {
            case 1: // Saldo
                sort(reportData.begin(), reportData.end(), [](const CarteiraReportInfo& a, const CarteiraReportInfo& b) {
                    return a.saldoTotal > b.saldoTotal;
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
        cout << left << setw(5) << "ID" << setw(20) << "Titular" << setw(20) << "Qtd. Moedas" << "Saldo Total (R$)" << endl;
        cout << "------------------------------------------------------------------------\n";

        stringstream ss;
        for (const auto& info : reportData) {
            ss.str(""); // Limpa o stream
            ss << left << setw(5) << info.id << setw(20) << info.titular 
               << setw(20) << fixed << setprecision(8) << info.quantidadeMoedas;
            
            if (info.saldoTotal >= 0) {
                ss << VERDE;
            } else {
                ss << VERMELHO;
            }
            ss << fixed << setprecision(2) << info.saldoTotal << RESET;
            printSlowly(ss.str(), 5);
        }
        cout << "------------------------------------------------------------------------\n";
        esperarInterativo(5);
    }
}

void Controller::reportHistorico()
{
    Menu menu;
    menu.setTitle("Historico Geral de Movimentacoes");
    menu.addOption("Ordenar por Data (Mais Recentes)");
    menu.addOption("Ordenar por Data (Mais Antigas)");
    menu.addOption("Ordenar por ID da Carteira");
    menu.addOption("Ordenar por ID da Movimentacao");
    menu.addOption("Voltar");

    while (true) {
        int opcao = menu.getOption();
        if (opcao == 5) break; // Voltar

        auto movimentacoes = _movimentacaoDAO->findAll();
        if (movimentacoes.empty()) {
            cout << "\nNenhuma movimentacao encontrada.\n";
            esperarInterativo(3);
            return;
        }

        switch (opcao) {
            case 1: // Mais recentes
                sort(movimentacoes.begin(), movimentacoes.end(), [](const unique_ptr<Movimentacao>& a, const unique_ptr<Movimentacao>& b) {
                    return a->getData() > b->getData();
                });
                break;
            case 2: // Mais antigas
                sort(movimentacoes.begin(), movimentacoes.end(), [](const unique_ptr<Movimentacao>& a, const unique_ptr<Movimentacao>& b) {
                    return a->getData() < b->getData();
                });
                break;
            case 3: // ID da Carteira
                sort(movimentacoes.begin(), movimentacoes.end(), [](const unique_ptr<Movimentacao>& a, const unique_ptr<Movimentacao>& b) {
                    if (a->getCarteiraId() != b->getCarteiraId()) {
                        return a->getCarteiraId() < b->getCarteiraId();
                    }
                    return a->getId() < b->getId(); // Critério de desempate
                });
                break;
            case 4: // ID da Movimentacao
                sort(movimentacoes.begin(), movimentacoes.end(), [](const unique_ptr<Movimentacao>& a, const unique_ptr<Movimentacao>& b) {
                    return a->getId() < b->getId();
                });
                break;
        }
        printMovimentacoes(movimentacoes);
        esperarInterativo(5);
    }
}

void Controller::reportGanhoPerda()
{
    double totalGanhoPerda = 0;
    auto movimentacoes = _movimentacaoDAO->findAll();
    if (movimentacoes.empty()) {
        cout << "\nNenhuma movimentacao encontrada para calcular ganhos/perdas.\n";
        esperarInterativo(3);
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

    esperarInterativo(5);
}

void Controller::actionRelatorioCarteiraEspecifica()
{
    vector<string> menuItens = {
        "Relatorio de Ganhos e Perdas",
        "Relatorio de Movimentacoes e Moedas",
        "Relatorio Geral",
        "Voltar"};
    vector<void (Controller::*)()> functions = {
        &Controller::reportCarteiraGanhosPerdas,
        &Controller::reportCarteiraMovimentacoesEMoedas,
        &Controller::reportCarteira};
    launchActions("Relatorio de Carteira Especifica", menuItens, functions);
}

void Controller::reportCarteiraGanhosPerdas()
{
    int carteiraId = getValidatedInput<int>("\nDigite o ID da carteira: ");
    auto carteira = _carteiraDAO->findById(carteiraId);
    if (!carteira) {
        cout << VERMELHO << "\nCarteira nao encontrada!\n" << RESET;
        return;
    }

    cout << CIANO NEGRITO << "\n------------------------------------------------\n";
    cout << "  Ganhos e Perdas: " << carteira->getTitular() << "\n";
    cout << "------------------------------------------------\n\n" << RESET;

    double quantidadeMoedas = getCarteiraQuantidadeMoedas(carteiraId);
    auto fluxoDetalhado = getCarteiraFluxoCaixaDetalhado(carteiraId);
    auto movimentacoes = _movimentacaoDAO->findByCarteiraId(carteiraId);

    double patrimonioMoedas = 0;
    if (!movimentacoes.empty()) {
        auto it_max = max_element(movimentacoes.cbegin(), movimentacoes.cend(),
            [](const unique_ptr<Movimentacao>& a, const unique_ptr<Movimentacao>& b) {
                return a->getData() < b->getData();
            });
        string dataUltimaMov = (*it_max)->getData();
        patrimonioMoedas = quantidadeMoedas * getCotacao(dataUltimaMov);
    }

    double resultadoTotal = patrimonioMoedas + fluxoDetalhado.fluxoLiquido;

    stringstream ss;

    ss.str("");
    ss << AMARELO << "Total Gasto em Compras (R$): " << RESET << VERMELHO << fixed << setprecision(2) << fluxoDetalhado.totalCompras << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << AMARELO << "Total Recebido em Vendas (R$): " << RESET << VERDE << fixed << setprecision(2) << fluxoDetalhado.totalVendas << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << AMARELO << "\nBalanço Financeiro (R$): " << RESET;
    if (fluxoDetalhado.fluxoLiquido >= 0) ss << VERDE; else ss << VERMELHO;
    ss << fixed << setprecision(2) << fluxoDetalhado.fluxoLiquido << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << AMARELO << "Patrimônio em Moedas (R$): " << RESET;
    if (patrimonioMoedas >= 0) ss << VERDE; else ss << VERMELHO;
    ss << fixed << setprecision(2) << patrimonioMoedas << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << AMARELO NEGRITO << "\nResultado Total (Ganhos/Perdas) (R$): " << RESET;
    if (resultadoTotal >= 0) ss << VERDE; else ss << VERMELHO;
    ss << fixed << setprecision(2) << resultadoTotal << RESET;
    printSlowly(ss.str());
    
    esperarInterativo(5);
}

void Controller::reportCarteiraMovimentacoesEMoedas()
{
    int carteiraId = getValidatedInput<int>("\nDigite o ID da carteira: ");
    auto carteira = _carteiraDAO->findById(carteiraId);
    if (!carteira) {
        cout << VERMELHO << "\nCarteira nao encontrada!\n" << RESET;
        return;
    }

    cout << CIANO NEGRITO << "\n------------------------------------------------\n";
    cout << "  Movimentacoes e Moedas: " << carteira->getTitular() << "\n";
    cout << "------------------------------------------------\n\n" << RESET;

    auto movimentacoes = _movimentacaoDAO->findByCarteiraId(carteiraId);
    
    vector<unique_ptr<Movimentacao>> compras;
    vector<unique_ptr<Movimentacao>> vendas;
    
    int qntOperacoesCompra = 0;
    int qntOperacoesVenda = 0;
    double totalMoedasCompradas = 0;
    double totalMoedasVendidas = 0;

    for (auto& mov : movimentacoes) {
        if (mov->getTipoOperacao() == "C") {
            totalMoedasCompradas += mov->getQuantidade();
            qntOperacoesCompra++;
            compras.push_back(make_unique<Movimentacao>(*mov));
        } else { // "V"
            totalMoedasVendidas += mov->getQuantidade();
            qntOperacoesVenda++;
            vendas.push_back(make_unique<Movimentacao>(*mov));
        }
    }

    cout << CIANO << "\n--- Movimentacoes Compra ---\n" << RESET;
    if (compras.empty()) {
        cout << AMARELO << "Nenhuma operacao de compra registrada." << RESET << endl;
    } else {
        printMovimentacoes(compras);
    }

    cout << CIANO << "\n--- Movimentacoes Venda ---\n" << RESET;
    if (vendas.empty()) {
        cout << AMARELO << "Nenhuma operacao de venda registrada." << RESET << endl;
    } else {
        printMovimentacoes(vendas);
    }
    
    double saldoAtualMoedas = getCarteiraQuantidadeMoedas(carteiraId);

    cout << CIANO << "\n--- Resumo de Moedas ---\n" << RESET;
    stringstream ss;
    
    ss << AMARELO << "\nTotal de Moedas Compradas: " << RESET << fixed << setprecision(8) << totalMoedasCompradas;
    printSlowly(ss.str());

    ss.str("");
    ss << AMARELO << "Quantidade de Operacoes de Compra: " << RESET << VERDE << qntOperacoesCompra << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << AMARELO << "\nTotal de Moedas Vendidas: " << RESET << fixed << setprecision(8) << totalMoedasVendidas;
    printSlowly(ss.str());

    ss.str("");
    ss << AMARELO << "Quantidade de Operacoes de Venda: " << RESET << VERDE << qntOperacoesVenda << RESET;
    printSlowly(ss.str());
    
    printSlowly("\n");

    ss.str("");
    ss << AMARELO << "Saldo Atual de Moedas: " << RESET << fixed << setprecision(8) << saldoAtualMoedas;
    printSlowly(ss.str());

    esperarInterativo(5);
}

void Controller::reportCarteira()
{
    int carteiraId = getValidatedInput<int>("\nDigite o ID da carteira: ");

    auto carteira = _carteiraDAO->findById(carteiraId);
    if (!carteira)
    {
        cout << VERMELHO << "\nCarteira nao encontrada!\n" << RESET;
        return;
    }

    cout << CIANO NEGRITO << "\n------------------------------------------------\n";
    cout << "      Relatorio da Carteira " << carteira->getTitular() << "\n";
    cout << "------------------------------------------------\n\n" << RESET;

    double quantidadeMoedas = getCarteiraQuantidadeMoedas(carteiraId);
    auto fluxoDetalhado = getCarteiraFluxoCaixaDetalhado(carteiraId);
    auto movimentacoes = _movimentacaoDAO->findByCarteiraId(carteiraId);

    double patrimonioMoedas = 0;
    if (!movimentacoes.empty()) {
        auto it_max = max_element(movimentacoes.cbegin(), movimentacoes.cend(),
            [](const unique_ptr<Movimentacao>& a, const unique_ptr<Movimentacao>& b) {
                return a->getData() < b->getData();
            });
        string dataUltimaMov = (*it_max)->getData();
        patrimonioMoedas = quantidadeMoedas * getCotacao(dataUltimaMov);
    }

    double resultadoTotal = patrimonioMoedas + fluxoDetalhado.fluxoLiquido;

    stringstream ss;
    ss << AMARELO << "Saldo de Moedas: " << RESET << fixed << setprecision(8) << quantidadeMoedas;
    printSlowly(ss.str());

    ss.str("");
    ss << AMARELO << "Total Gasto em Compras (R$): " << RESET << VERMELHO << fixed << setprecision(2) << fluxoDetalhado.totalCompras << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << AMARELO << "Total Recebido em Vendas (R$): " << RESET << VERDE << fixed << setprecision(2) << fluxoDetalhado.totalVendas << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << AMARELO << "\nBalanço Financeiro (R$): " << RESET;
    if (fluxoDetalhado.fluxoLiquido >= 0) ss << VERDE; else ss << VERMELHO;
    ss << fixed << setprecision(2) << fluxoDetalhado.fluxoLiquido << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << AMARELO << "Patrimônio em Moedas (R$): " << RESET;
    if (patrimonioMoedas >= 0) ss << VERDE; else ss << VERMELHO;
    ss << fixed << setprecision(2) << patrimonioMoedas << RESET;
    printSlowly(ss.str());

    ss.str("");
    ss << AMARELO NEGRITO << "\nResultado Total (Ganhos/Perdas) (R$): " << RESET;
    if (resultadoTotal >= 0) ss << VERDE; else ss << VERMELHO;
    ss << fixed << setprecision(2) << resultadoTotal << RESET;
    printSlowly(ss.str());

    if (movimentacoes.empty())
    {
        cout << AMARELO << "\nNenhuma movimentacao registrada para esta carteira." << RESET << endl;
    }
    else
    {
        cout << CIANO << "\n--- Historico de Movimentacoes ---\n" << RESET;
    printMovimentacoes(movimentacoes);
    }
    esperarInterativo(5);
}

void Controller::showSystemHelp()
{
    cout << CIANO NEGRITO << "\n--- AJUDA DO SISTEMA FT COIN ---\n\n" << RESET;
    
    auto printBlock = [this](const vector<string>& block) {
        for(const auto& line : block) {
            printSlowly(line, 9);
        }
    };

    vector<string> menu_principal_text = {
        string(AMARELO) + "MENU PRINCIPAL:" + RESET,
        "1. Gerenciar Carteiras - Crie, edite, exclua e liste suas carteiras de moedas.",
        "2. Compra & Venda de Moedas - Registre suas operacoes de compra e venda.",
        "3. Relatorios - Gere relatorios globais, por carteira ou sobre as cotacoes.",
        "4. Ajuda - Exibe este menu de ajuda e os creditos do sistema.",
        "5. Sair - Encerra o programa."
    };
    printBlock(menu_principal_text);
    esperar(1);

    vector<string> funcionalidades_text = {
        string(AMARELO) + "\nFUNCIONALIDADES:" + RESET,
        "- Os relatorios de carteira permitem uma analise detalhada de ganhos, perdas e movimentacoes.",
        "- O oraculo de cotacao busca e armazena os valores da moeda para garantir consistencia nos calculos."
    };
    printBlock(funcionalidades_text);
    esperar(1);

    vector<string> comandos_text = {
        string(AMARELO) + "\nCOMANDOS:" + RESET,
        "- Utilize os numeros correspondentes para navegar pelos menus.",
        "- Siga as instrucoes exibidas em cada tela para inserir dados.",
        "- Datas devem ser inseridas no formato DD-MM-AAAA."
    };
    printBlock(comandos_text);
    esperar(1);
    
    vector<string> mais_info_text = {
        string(AMARELO) + "\nPARA MAIS INFORMACOES:" + RESET,
        "Consulte a documentacao do projeto ou entre em contato com a equipe de desenvolvimento."
    };
    printBlock(mais_info_text);

    esperarInterativo(5);
}

void Controller::showCredits()
{
    vector<string> credits_art = {
        "   _____ _____  ______ _____ _____ _______ _____ ",
        "  / ____|  __ \\|  ____|  __ \\_   _|__   __/ ____|",
        " | |    | |__) | |__  | |  | || |    | | | (___  ",
        " | |    |  _  /|  __| | |  | || |    | |  \\___ \\ ",
        " | |____| | \\ \\| |____| |__| || |_   | |  ____) |",
        "  \\_____|_|  \\_\\______|_____/_____|  |_| |_____/ ",
        "                                                 ",
        "                                                 "
    };

    cout << CIANO NEGRITO;
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
        "\nSISTEMA FT COIN",
        "Versao: 1.0.0",
        "Data de Lancamento: Julho de 2025"
    };
    printBlock(info_text);
    esperar(1);

    vector<string> dev_text = {
        string(AMARELO) + "\nDESENVOLVIDO POR:" + RESET,
        "Gustavo Soares Almeida\nHans William Hamann\nMarcelo Dos Santos Da Boa Morte\nNicolas Lourenço Mafei\nRenan Felipe Rodrigues"
    };
    printBlock(dev_text);
    esperar(1);

    vector<string> rights_text = {
        string(AMARELO) + "\nDIREITOS:" + RESET,
        "© 2025 FT-Coin. Todos os direitos reservados."
    };
    printBlock(rights_text);
    esperar(1);

    vector<string> uni_text = {
        "\nUNICAMP - Universidade Estadual de Campinas",
        "Disciplina: SI300 - Programação Orientada a Objetos I",
        "2º Semestre de 2025"
    };
    printBlock(uni_text);

    esperarInterativo(5);
}

void Controller::esperar(int seconds)
{
    sleep(seconds);
}

void Controller::esperarInterativo(int seconds)
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

void Controller::actionRelatorioOraculo()
{
    vector<string> menuItens = {
        "Listar Historico Cotacao",
        "Listar Cotacao por Data Especifica",
        "Relatorio Geral de Cotacao",
        "Voltar"};
    vector<void (Controller::*)()> functions = {
        &Controller::listarHistoricoCotacao,
        &Controller::listarCotacaoPorData,
        &Controller::relatorioGeralCotacao
        };
    launchActions("Relatorio de Cotacao do Oraculo", menuItens, functions);
}

void Controller::listarHistoricoCotacao()
{
    if (_type == DataBaseSelector::DATABASE) {
        try {
            unique_ptr<sql::Statement> stmnt(_dbConnection->getConnection()->createStatement());
            sql::ResultSet* res = stmnt->executeQuery("SELECT DATE_FORMAT(Data, '%d-%m-%Y') as DataFormatada, Cotacao FROM ORACULO ORDER BY Data ASC");
            
            cout << CIANO NEGRITO << "\n--- Historico de Cotacoes do Oraculo (BD) ---\n" << RESET;
            cout << "---------------------------------------\n";
            cout << left << setw(15) << "Data" << "Cotacao (R$)" << endl;
            cout << "---------------------------------------\n";
            
            while (res->next()) {
                cout << left << setw(15) << res->getString("DataFormatada") << fixed << setprecision(2) << res->getDouble("Cotacao") << endl;
            }
            cout << "---------------------------------------\n";
            delete res;
        } catch (sql::SQLException& e) {
            cerr << VERMELHO << "Erro ao consultar o banco de dados: " << e.what() << RESET << endl;
        }
    } else { // MEMORY
        if (_oracle_mem.empty()) {
            cout << AMARELO << "\nNenhuma cotacao registrada no oraculo." << RESET << endl;
            esperarInterativo(3);
            return;
        }

        vector<pair<string, double>> cotacoes(_oracle_mem.begin(), _oracle_mem.end());
        sort(cotacoes.begin(), cotacoes.end(), [](const pair<string, double>& a, const pair<string, double>& b) {
            string dataA = a.first.substr(6, 4) + a.first.substr(3, 2) + a.first.substr(0, 2);
            string dataB = b.first.substr(6, 4) + b.first.substr(3, 2) + b.first.substr(0, 2);
            return dataA < dataB;
        });

        cout << CIANO NEGRITO << "\n--- Historico de Cotacoes do Oraculo (Memoria) ---\n" << RESET;
        cout << "---------------------------------------\n";
        cout << left << setw(15) << "Data" << "Cotacao (R$)" << endl;
        cout << "---------------------------------------\n";
        
        for (const auto& cotacao : cotacoes) {
            cout << left << setw(15) << cotacao.first << fixed << setprecision(2) << cotacao.second << endl;
        }
        cout << "---------------------------------------\n";
    }
    esperarInterativo(5);
}

void Controller::relatorioGeralCotacao()
{
    if (_type == DataBaseSelector::DATABASE) {
        try {
            unique_ptr<sql::Statement> stmnt(_dbConnection->getConnection()->createStatement());
            sql::ResultSet* res = stmnt->executeQuery(
                "SELECT "
                "(SELECT Cotacao FROM ORACULO ORDER BY Cotacao DESC LIMIT 1) as maxVal, "
                "(SELECT DATE_FORMAT(Data, '%d-%m-%Y') FROM ORACULO ORDER BY Cotacao DESC LIMIT 1) as maxDate, "
                "(SELECT Cotacao FROM ORACULO ORDER BY Cotacao ASC LIMIT 1) as minVal, "
                "(SELECT DATE_FORMAT(Data, '%d-%m-%Y') FROM ORACULO ORDER BY Cotacao ASC LIMIT 1) as minDate, "
                "AVG(Cotacao) as media FROM ORACULO"
            );

            if (res->next()) {
                cout << CIANO NEGRITO << "\n--- Relatorio Geral de Cotacoes (BD) ---\n\n" << RESET;
                stringstream ss;

                ss << AMARELO << "Cotacao Maxima: " << RESET << VERDE << fixed << setprecision(2) << res->getDouble("maxVal") << " (em " << res->getString("maxDate") << ")" << RESET;
                printSlowly(ss.str());

                ss.str("");
                ss << AMARELO << "Cotacao Minima: " << RESET << VERMELHO << fixed << setprecision(2) << res->getDouble("minVal") << " (em " << res->getString("minDate") << ")" << RESET;
                printSlowly(ss.str());

                ss.str("");
                ss << AMARELO << "Cotacao Media:  " << RESET << fixed << setprecision(2) << res->getDouble("media");
                printSlowly(ss.str());
            } else {
                 cout << AMARELO << "\nNenhuma cotacao registrada no oraculo." << RESET << endl;
            }
            delete res;

        } catch (sql::SQLException& e) {
            cerr << VERMELHO << "Erro ao consultar o banco de dados: " << e.what() << RESET << endl;
        }

    } else { // MEMORY
        if (_oracle_mem.empty()) {
            cout << AMARELO << "\nNenhuma cotacao registrada no oraculo." << RESET << endl;
            esperarInterativo(3);
            return;
        }

        double maxVal = -1.0;
        string maxDate;
        double minVal = numeric_limits<double>::max();
        string minDate;
        double sum = 0.0;

        for (const auto& par : _oracle_mem) {
            if (par.second > maxVal) {
                maxVal = par.second;
                maxDate = par.first;
            }
            if (par.second < minVal) {
                minVal = par.second;
                minDate = par.first;
            }
            sum += par.second;
        }

        double media = sum / _oracle_mem.size();

        cout << CIANO NEGRITO << "\n--- Relatorio Geral de Cotacoes (Memoria) ---\n\n" << RESET;
        stringstream ss;

        ss << AMARELO << "Cotacao Maxima: " << RESET << VERDE << fixed << setprecision(2) << maxVal << " (em " << maxDate << ")" << RESET;
        printSlowly(ss.str());

        ss.str("");
        ss << AMARELO << "Cotacao Minima: " << RESET << VERMELHO << fixed << setprecision(2) << minVal << " (em " << minDate << ")" << RESET;
        printSlowly(ss.str());

        ss.str("");
        ss << AMARELO << "Cotacao Media:  " << RESET << fixed << setprecision(2) << media;
        printSlowly(ss.str());
    }
    esperarInterativo(5);
}

void Controller::listarCotacaoPorData()
{
    string data = getValidatedDate("Digite a data para a consulta (DD-MM-AAAA): ");
    
    // A função getCotacao já lida com BD e memória, então não precisa de lógica dupla aqui.
    // Ela já insere no _oracle_mem se não encontrar, e também no BD se for o caso.
    double cotacao = getCotacao(data);

    cout << "\n";
    stringstream ss;
    ss << AMARELO << "Cotacao para a data " << RESET << data << AMARELO << " e de: " << RESET << VERDE << "R$ " << fixed << setprecision(2) << cotacao << RESET;
    printSlowly(ss.str());
    esperarInterativo(3);
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

bool Controller::isValidDate(int d, int m, int y)
{
    if (y < 1900 || y > 2100 || m < 1 || m > 12 || d < 1 || d > 31) {
        return false;
    }

    if (m == 2) {
        bool isLeap = (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
        if (isLeap) {
            return d <= 29;
        } else {
            return d <= 28;
        }
    }

    if (m == 4 || m == 6 || m == 9 || m == 11) {
        return d <= 30;
    }

    return true;
}

string Controller::getValidatedDate(const string& prompt) {
    string date_str;
    regex date_regex(R"(^(\d{2})-(\d{2})-(\d{4})$)");
    smatch matches;

    while (true) {
        cout << prompt;
        getline(cin, date_str);
        if (regex_match(date_str, matches, date_regex)) {
            int d = stoi(matches[1].str());
            int m = stoi(matches[2].str());
            int y = stoi(matches[3].str());

            if (isValidDate(d, m, y)) {
                return matches[3].str() + "-" + matches[2].str() + "-" + matches[1].str();
            } else {
                cout << VERMELHO << "Data invalida. O dia ou mes estao fora do intervalo para o ano." << RESET << endl;
            }
        } else {
            cout << VERMELHO << "Formato de data invalido. Use DD-MM-AAAA. Por favor, tente novamente." << RESET << endl;
        }
    }
}

template int Controller::getValidatedInput<int>(const string& prompt);
template double Controller::getValidatedInput<double>(const string& prompt);

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

CarteiraFluxoCaixaDetalhado Controller::getCarteiraFluxoCaixaDetalhado(int carteiraId)
{
    auto movimentacoes = _movimentacaoDAO->findByCarteiraId(carteiraId);
    double totalCompras = 0;
    double totalVendas = 0;
    for (const auto& mov : movimentacoes) {
        double valorOperacao = getCotacao(mov->getData()) * mov->getQuantidade();
        if (mov->getTipoOperacao() == "C") {
            totalCompras += valorOperacao;
        } else {
            totalVendas += valorOperacao;
        }
    }
    return {totalCompras, totalVendas, totalVendas - totalCompras};
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