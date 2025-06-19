#include "Controller.h"
#include "cores.h"
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>

using namespace std;

void chooseDatabase(unique_ptr<Controller>& controller) {
    cout << AMARELO << "Selecione o modo de banco de dados:\n" << RESET;
    cout << VERDE NEGRITO << "1." << RESET << " Memoria\n" << RESET;
    cout << VERDE NEGRITO << "2." << RESET << " Banco de Dados (MariaDB)\n\n" << RESET;
    cout << AMARELO << "Opção: " << RESET;

    int opcao;
    while (!(cin >> opcao) || (opcao != 1 && opcao != 2)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << VERMELHO << "Opcao invalida! Digite 1 ou 2: " << RESET;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    DataBaseSelector dbType = (opcao == 1) ? DataBaseSelector::MEMORY : DataBaseSelector::DATABASE;
    controller = make_unique<Controller>(dbType);
}

int main() {
    system("clear");
    
    try {
        unique_ptr<Controller> mainController;
        chooseDatabase(mainController);
        mainController->start();
    } catch (const exception& e) {
        cerr << VERMELHO << "\nErro: " << e.what() << RESET << endl;
        return 1;
    }

    return 0;
}