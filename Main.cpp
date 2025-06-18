#include <iostream>
#include <limits>
#include "Controller.h"
#include "cores.h"

using namespace std;

int main() {
    try {
        cout << VERDE << "\n=== FT-Coin - Sistema de Controle de Carteiras ===\n" << RESET;
        cout << "\nSelecione o tipo de execução:\n";
        cout << "1. Memória\n";
        cout << "2. Banco de Dados\n\n";
        cout << "Opção: ";

        int opcao;
        while (!(cin >> opcao) || (opcao != 1 && opcao != 2)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << VERMELHO << "Opção inválida! Digite 1 ou 2: " << RESET;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        DataBaseSelector dbType = (opcao == 1) ? DataBaseSelector::MEMORY : DataBaseSelector::DATABASE;
        Controller controller(dbType);
        controller.start();
    } catch (const exception& e) {
        cerr << VERMELHO << "\nErro: " << e.what() << RESET << endl;
        return 1;
    } catch (...) {
        cerr << VERMELHO << "\nErro desconhecido!" << RESET << endl;
        return 1;
    }
    return 0;
}