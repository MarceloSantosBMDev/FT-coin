#include "menu.hpp"
#include <iostream>

Menu::Menu() {
    // Construtor vazio
}

void Menu::mostrarTitulo() {
    std::cout << R"(
-----------------------------------------------
  ______ _______   _____   _____  _____  _   _ 
 |  ____|__   __| / ____| /  __ \|_   _|| \ | |
 | |__     | |   | |     | |  | |  | |  |  \| |
 |  __|    | |   | |     | |  | |  | |  | . ` |
 | |       | |   | |____ | |__| | _| |_ | |\  |
 |_|       |_|    \_____| \____/ |_____||_| \_|
-----------------------------------------------
    )" << std::endl;
}

void Menu::mostrarMenu() {
    mostrarTitulo();
    std::cout << "==== MENU PRINCIPAL ====" << std::endl;
    std::cout << "1. Carteira" << std::endl;
    std::cout << "2. Movimentação" << std::endl;
    std::cout << "3. Relatórios" << std::endl;  // Corrigido o acento
    std::cout << "4. Ajuda" << std::endl;       // Corrigido o número
    std::cout << "5. Sair" << std::endl;        // Corrigido o número
}