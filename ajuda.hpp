#ifndef AJUDA_H
#define AJUDA_H

#include <iostream>
#include <string>
using namespace std;
class Ajuda {
public:
    Ajuda();

    void mostrarAjuda() const;

    void mostrarCreditos() const;

private:
    const string versao = "1.0.0";
    
    const string dataLancamento = "Junho 2025";
    
    const string autores = "Equipe A-10: Marcelo Dos Santos Da Boa Morte, Nicolas Mafei, Renan Felipe Rodrigues, Gustavo Almeida";
    
    const string copyright = "Copyright © 2025 - FT Coin System - Todos os Direitos reservados";
};

#endif 