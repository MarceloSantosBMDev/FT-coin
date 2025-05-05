#ifndef AJUDA_H
#define AJUDA_H

#include <iostream>
#include <string>

class Ajuda {
public:
    Ajuda();

    void mostrarAjuda() const;

    void mostrarCreditos() const;

private:
    const std::string versao = "1.0.0";
    
    const std::string dataLancamento = "Junho 2025";
    
    const std::string autores = "Equipe A-10: Marcelo Dos Santos Da Boa Morte, Nicolas Mafei, Renan Felipe Rodrigues, Gustavo Almeida";
    
    const std::string copyright = "Copyright © 2025 - FT Coin System - Todos os Direitos reservados";
};

#endif 