#ifndef MOVIMENTACAO_HPP
#define MOVIMENTACAO_HPP

#include <string>
#include <iostream>
#include <random>
#include <chrono>
#include "cores.h"


using namespace std;

class movimentacao{
    private:
    int entry_id;
    int id;
    string date;
    char operation;
    float quantidade;
    public:
    movimentacao(int entry_id, int id, string data, char operation, float quantidade);
    //getters
    int getEntry_id()const;
    int getID()const;
    string getDate()const;
    char getOperation()const;
    float getQuantidade()const;
    
};
void criar_registro(char operation, int idEntrada);
float Calcular_Saldo();
float Oraculo();
void ExibirRegistro(string titular, int id);
void ExcluirRegistrosDaCarteira(int entry_ID);
void Calcular_Ganho_Perda(int entry_ID, string titular);

#endif