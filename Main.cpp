#include <iostream>
#include "menu.hpp"
#include "ajuda.hpp"
using namespace std;

void exibirMenuAjuda() {
    Ajuda ajuda;
    int opcao;
    
    do {
        cout << "\n=== MENU AJUDA ===\n";
        cout << "1. Mostrar Ajuda do Sistema\n";
        cout << "2. Mostrar Créditos\n";
        cout << "3. Voltar ao Menu Principal\n";
        cout << "Escolha uma opção: ";
        cin >> opcao;
        
        switch(opcao) {
            case 1:
                ajuda.mostrarAjuda();
                break;
            case 2:
                ajuda.mostrarCreditos();
                break;
            case 3:
                cout << "Retornando ao menu principal...\n";
                break;
            default:
                cout << "Opção inválida! Tente novamente.\n";
        }
    } while(opcao != 3);
}


int main() {
    Menu menu;
    int opcao;
    do {
        menu.mostrarMenu();

        cout << "Escolha uma opção: ";
        cin >> opcao;

        switch (opcao) {
            case 1:
                cout << "Opção 1." << endl;
                break;
            case 2:
                cout << "Opção 2." << endl;
                break;
            case 3:
                cout << "Opção 3." << endl;
                break;
            case 4:
                exibirMenuAjuda();
                break;
            default:
                cout << "Opção inválida. Tente novamente." << endl;
        }

        cout << endl;
    } while (opcao != 5);

    return 0;
}
