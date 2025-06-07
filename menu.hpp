#ifndef MENU_HPP
#define MENU_HPP

#include <string>
#include "carteira.hpp"
#include "movimentacao.hpp"
#include "ajuda.hpp"
#include "cores.h"
#include <vector>

using namespace std;

class Menu {
private:

    void mostrarTitulo();
    void menuCarteira();
    void menuMovimentacao();
    void menuRelatorio();
    void exibirMenuAjuda();
    

public:
    Menu();
    void mostrarMenu();
};

#endif