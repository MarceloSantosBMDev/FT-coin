# FT-Coin - Sistema de Controle de Carteiras

Este é um sistema de controle de carteiras de criptomoedas desenvolvido em C++.

## Requisitos

- CMake 3.10 ou superior
- Compilador C++ com suporte a C++17
- SQLite3

## Execução

g++ -std=c++17 -o FT-Coin.exe Main.cpp Menu.cpp Controller.cpp Carteira.cpp Movimentacao.cpp ConnectDB.cpp CarteiraMemDAO.cpp MovimentacaoMemDAO.cpp CarteiraBDDAO.cpp MovimentacaoBDDAO.cpp -I/usr/include/mariadb -lmariadbcpp -lmariadb -lpthread -ldl

## Funcionalidades

- Gerenciamento de carteiras
  - Criar nova carteira
  - Editar carteira existente
  - Excluir carteira
  - Listar carteiras

- Movimentações
  - Registrar compras
  - Registrar vendas

- Relatórios
  - Carteiras por ID
  - Carteiras por nome
  - Saldo das carteiras
  - Histórico de movimentações
  - Ganhos e perdas

## Licensa

Programa criado e produzido pelo grupo A-10, composto por Gustavo Soares Almeida, Hans William Hamann, Marcelo Dos Santos Da Boa Morte, Nicolas Lourenço Mafei e Renan Felipe Rodrigues
