#include <iostream>
#include "menu.hpp"
#include "ajuda.hpp"
#include "connectDB.h" // Inclua o cabeçalho do banco de dados

using namespace std;

int main() {
    // Instancia e testa a conexão com o banco de dados
    try {
        ServerDBConnection dbConnection;
        // Se chegou aqui, a conexão foi bem-sucedida
        cout << "Conexão com o banco de dados estabelecida com sucesso!" << endl;
    } catch (...) {
        cout << "Falha ao conectar ao banco de dados." << endl;
        return 1; // Retorna código de erro
    }

    Menu menu;
    menu.mostrarMenu();
    return 0;
}