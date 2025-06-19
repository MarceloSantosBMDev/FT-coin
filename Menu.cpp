#include "Menu.h"
#include "cores.h"
#include <iomanip>

using namespace std;

Menu::Menu() {
}

Menu::~Menu() {
}

void Menu::setTitle(const string& title) {
    this->title = title;
}

void Menu::addOption(const string& option) {
    options.push_back(option);
}

int Menu::getOption() const {
    while (true) {
        cout << "\n" << CIANO NEGRITO;
        cout << "╔════════════════════════════════════════════════════════════╗\n";
        cout << "║ " << left << setw(58) << title << " ║\n";
        cout << "╚════════════════════════════════════════════════════════════╝\n" << RESET;
        
        for (size_t i = 0; i < options.size(); i++) {
            cout << "   " << VERDE NEGRITO << (i + 1) << ". " << RESET << options[i] << "\n";
        }
        cout << AMARELO << "\n   Opção: " << RESET;

        int opcao;
        while (!(cin >> opcao) || opcao < 1 || opcao > static_cast<int>(options.size())) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << VERMELHO << "Opção inválida! Digite um número entre 1 e " << options.size() << ": " << RESET;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return opcao;
    }
} 