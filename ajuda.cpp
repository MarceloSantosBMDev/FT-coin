#include "ajuda.hpp"
#include <iostream>

using namespace std;

Ajuda::Ajuda() {
}

void Ajuda::mostrarAjuda() const {
    cout << "\n=== AJUDA DO SISTEMA FT COIN ===\n\n";
    
    cout << "MENU PRINCIPAL:\n";
    cout << "1. Carteira - Gerencia carteiras de moedas virtuais\n";
    cout << "2. Movimentação - Registra compras e vendas de moedas\n";
    cout << "3. Relatórios - Gera diversos relatórios do sistema\n";
    cout << "4. Ajuda - Exibe esta tela de ajuda\n";
    cout << "5. Sair - Encerra o programa\n\n";
    
    cout << "FUNCIONALIDADES:\n";
    cout << "- Carteira: Incluir, recuperar, editar e excluir carteiras\n";
    cout << "- Movimentação: Registrar compras e vendas de moedas virtuais\n";
    cout << "- Relatórios: Listar carteiras, exibir saldos e históricos\n";
    cout << "- Ajuda: Exibe informações sobre como usar o sistema\n\n";
    
    cout << "COMANDOS:\n";
    cout << "- Utilize os números correspondentes para navegar nos menus\n";
    cout << "- Siga as instruções exibidas em cada tela\n";
    cout << "- Pressione Enter para confirmar as entradas\n\n";
    
    cout << "PARA MAIS INFORMAÇÕES:\n";
    cout << "Consulte a documentação do projeto ou entre em contato com a equipe de desenvolvimento.\n";
}

void Ajuda::mostrarCreditos() const {
    cout << "\n=== CRÉDITOS ===\n\n";
    cout << "SISTEMA FT COIN\n";
    cout << "Versão: " << versao << "\n";
    cout << "Data: " << dataLancamento << "\n\n";
    
    cout << "DESENVOLVIDO POR:\n";
    cout << autores << "\n\n";
    
    cout << "DIREITOS:\n";
    cout << copyright << "\n\n";
    
    cout << "UNICAMP - Universidade Estadual de Campinas\n";
    cout << "Disciplina: SI300 - Programação Orientada a Objetos I\n";
    cout << "1º Semestre de 2025\n";
}