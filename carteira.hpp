#ifndef CARTEIRA_H
#define CARTEIRA_H
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

using namespace std;

class carteira {
public:
    carteira(); 
    //funcoespropriasdacarteira
    void IncluirCarteira();
    void ExibirDados() const;
    //funcoesset
    void setTitular();
    void setCorretora();
    //funcoesget
    int getID()const ;
    string getCorretora()const;
    string getTitular()const ;

    
private:
    int id;
    string titular;
    string corretora;
};
extern vector<carteira> carteiras;
void ColocarCarteiraNoVetor(carteira nova);
carteira* BuscarCarteiraPorID();
void EditarCarteira();
void ExcluirCarteira();
void ExibirTodasAsCarteirasID();
void ExibirTodasAsCarteirasNome();
void ExibirSaldoTodasAsCarteiras();

#endif