#include "menu.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <limits>

Menu::Menu() {}

void Menu::mostrarTitulo() {
    std::cout << AZUL << R"(
-----------------------------------------------
  ______ _______   _____   _____  _____  _   _ 
 |  ____|__   __| / ____| /  __ \|_   _|| \ | |
 | |__     | |   | |     | |  | |  | |  |  \| |
 |  __|    | |   | |     | |  | |  | |  | . ` |
 | |       | |   | |____ | |__| | _| |_ | |\  |
 |_|       |_|    \_____| \____/ |_____||_| \_|
-----------------------------------------------
    )" << RESET << std::endl;
}

void Menu::mostrarMenu() {
    mostrarTitulo();
    int opcao;
    do {
        std::cout << CIANO << "==== MENU PRINCIPAL ====\n" << RESET << std::endl;
        std::cout << "1. Carteira\n" << std::endl;
        std::cout << "2. Movimentação\n" << std::endl;
        std::cout << "3. Relatórios\n" << std::endl;  
        std::cout << "4. Ajuda\n" << std::endl;      
        std::cout << "5. Sair\n" << std::endl;
        std::cout << "Escolha uma opção: ";
        
        while (!(std::cin >> opcao)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << VERMELHO << "Entrada inválida! Digite um número: " << RESET;
        }
        
        switch(opcao) {
            case 1:
                menuCarteira();
                break;
            case 2:
                menuMovimentacao();
                break;
            case 3: 
                menuRelatorio();
                break;
            case 4:
                exibirMenuAjuda();
                break;
            case 5:
                std::cout << "Saindo..." << std::endl;
                break;
            default:
                std::cout << VERMELHO << "Opção inválida! Tente novamente." << RESET << std::endl;
        }   
    } while(opcao != 5);
}

void Menu::menuCarteira() {
    int opcao;
    do {
        std::cout << CIANO << "\n==== MENU CARTEIRA ====\n" << RESET << std::endl;
        std::cout << "1. Incluir um novo registro de carteira\n" << std::endl;
        std::cout << "2. Recuperar um registro de carteira por identificador\n" << std::endl;
        std::cout << "3. Editar um registro de carteira existente\n" << std::endl;
        std::cout << "4. Excluir um registro de carteira\n" << std::endl;
        std::cout << "5. Retornar ao Menu Principal\n"<< std::endl;
        std::cout << "Escolha uma opção: ";
        
        while (!(std::cin >> opcao)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << VERMELHO << "Entrada inválida! Digite um número: " << RESET;
        }
        
        switch(opcao) {
            case 1: {  
                carteira nova;
                nova.IncluirCarteira();
                ColocarCarteiraNoVetor(nova);
                std::cout << VERDE << "Carteira adicionada com sucesso!\n" << RESET << std::endl;
                break;
            }
            case 2: {
                if(carteiras.empty()) {
                    std::cout << VERMELHO << "Nenhuma carteira cadastrada!\n" << RESET << std::endl;
                    break;
                }
                carteira* encontrada = BuscarCarteiraPorID();
                if(encontrada != nullptr){
                    std::cout << VERDE << "Carteira encontrada com Sucesso!" << RESET << std::endl;
                    encontrada->ExibirDados();
                }
                break;
            }
            case 3: {
                if(carteiras.empty()) {
                    std::cout << VERMELHO << "Nenhuma carteira cadastrada!\n" << RESET << std::endl;
                    break;
                }
                EditarCarteira();
                break;
            }
            case 4: {
                if(carteiras.empty()) {
                    std::cout << VERMELHO << "Nenhuma carteira cadastrada!\n" << RESET << std::endl;
                    break;
                }
                ExcluirCarteira();
                break;
            }
            case 5:
                std::cout << "Retornando ao menu principal...\n";
                break;
            default:
                std::cout << VERMELHO << "Opção inválida! Tente novamente." << RESET << std::endl;
        }
    } while (opcao != 5);
}

void Menu::menuMovimentacao() {
    int opcao;
    do {
        std::cout << CIANO << "==== MENU MOVIMENTAÇÃO ====" << RESET << std::endl;
        std::cout << "1. Registrar compra de moeda virtual\n" << std::endl;
        std::cout << "2. Registrar venda de moeda virtual\n" << std::endl;
        std::cout << "3. Voltar ao Menu Principal\n" << std::endl;
        std::cout << "Escolha uma opção: ";
        
        while (!(std::cin >> opcao)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << VERMELHO << "Entrada inválida! Digite um número: " << RESET;
        }
        
        switch(opcao) {
            case 1: {
                if(carteiras.empty()) {
                    std::cout << VERMELHO << "Nenhuma carteira cadastrada!\n" << RESET << std::endl;
                    break;
                }
                carteira* encontrada = BuscarCarteiraPorID();
                if(encontrada != nullptr){
                    criar_registro('C', encontrada->getID());
                    break;

                }
                else{
                    cout<<VERMELHO<<"Carteira não encontrada"<<RESET<<endl;
                }
                break;
            }
            case 2: {
                if(carteiras.empty()) {
                    std::cout << VERMELHO << "Nenhuma carteira cadastrada!\n" << RESET << std::endl;
                    break;
                }
                carteira* encontrada = BuscarCarteiraPorID();
                if(encontrada != nullptr){
                    criar_registro('V', encontrada->getID());
                    break;

                }
                else{
                    cout<<VERMELHO<<"Carteira não encontrada"<<RESET<<endl;
                }
                break;
            }
            case 3: {
                std::cout << "Retornando ao menu principal..." << std::endl;
                break;
            }
            default:
                std::cout << VERMELHO << "Opção inválida! Tente novamente." << RESET << std::endl;
        }
    } while(opcao != 3);
}

void Menu::menuRelatorio() {
    int opcao;
    do {
        std::cout << CIANO << "\n==== MENU RELATÓRIOS ====\n" << RESET << std::endl;
        std::cout << "1. Listar carteiras ordenadas por identificador\n" << std::endl;
        std::cout << "2. Listar carteiras ordenadas por nome do titular\n" << std::endl;
        std::cout << "3. Exibir saldo atual de uma carteira específica\n" << std::endl;
        std::cout << "4. Exibir histórico de movimentação de uma carteira específica\n" << std::endl;
        std::cout << "5. Apresentar ganho ou perda total de cada carteira\n" << std::endl;
        std::cout << "6. Voltar ao Menu Principal\n" << std::endl;
        std::cout << "Escolha uma opção: ";
        
        while (!(std::cin >> opcao)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << VERMELHO << "Entrada inválida! Digite um número: " << RESET;
        }

        switch(opcao) {
            case 1: {
                if(carteiras.empty()) {
                    std::cout << VERMELHO << "Nenhuma carteira cadastrada!\n" << RESET << std::endl;
                    break;
                }
                ExibirTodasAsCarteirasID();
                break;
            }
            case 2: {
                if(carteiras.empty()) {
                    std::cout << VERMELHO << "Nenhuma carteira cadastrada!\n" << RESET << std::endl;
                    break;
                }
                ExibirTodasAsCarteirasNome();
                break;
            }
            case 3: {
                if(carteiras.empty()) {
                    std::cout << VERMELHO << "Nenhuma carteira cadastrada!\n" << RESET << std::endl;
                    break;
                }
                
                float saldo = Calcular_Saldo();
                std::cout << "Saldo atual: " << saldo << std::endl;
                break;
            }
            case 4: {
                if(carteiras.empty()) {
                    std::cout << VERMELHO << "Nenhuma carteira cadastrada!\n" << RESET << std::endl;
                    break;
                }
                carteira* encontrada = BuscarCarteiraPorID();
                if(encontrada != nullptr){
                    ExibirRegistro(encontrada->getTitular(), encontrada->getID());
                    break;

                }
                break;
            }
            case 5: {
                if(carteiras.empty()) {
                    std::cout << VERMELHO << "Nenhuma carteira cadastrada!\n" << RESET << std::endl;
                    break;
                }
                ExibirSaldoTodasAsCarteiras();
            }
            case 6: 
                std::cout << "Retornando ao menu principal..." << std::endl;
                break;
            default:
                std::cout << VERMELHO << "Opção inválida! Tente novamente." << RESET << std::endl;
        }
    } while(opcao != 6);
}

void Menu::exibirMenuAjuda() {
    Ajuda ajuda;
    int opcao;
    
    do {
        std::cout << CIANO << "\n=== MENU AJUDA ===" << RESET << std::endl;
        std::cout << "1. Mostrar Ajuda do Sistema\n" << std::endl;
        std::cout << "2. Mostrar Créditos\n" << std::endl;
        std::cout << "3. Voltar ao Menu Principal\n" << std::endl;
        std::cout << "Escolha uma opção: ";
        
        while (!(std::cin >> opcao)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << VERMELHO << "Entrada inválida! Digite um número: " << RESET;
        }
        
        switch(opcao) {
            case 1:
                ajuda.mostrarAjuda();
                break;
            case 2:
                ajuda.mostrarCreditos();
                break;
            case 3:
                std::cout << "Retornando ao menu principal..." << std::endl;
                break;
            default:
                std::cout << VERMELHO << "Opção inválida! Tente novamente." << RESET << std::endl;
        }
    } while(opcao != 3);
}


