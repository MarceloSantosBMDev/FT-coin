#include "movimentacao.hpp"
#include <sstream>
#include <iomanip>
#include <vector>
vector<movimentacao> movimentacoes;
int registros;
float Oraculo() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_real_distribution<float> dis(5.0f, 7.0f);
    return dis(gen);
}

movimentacao::movimentacao(int entry_id, int id, std::string date, char operation, float quantidade)
    : entry_id(entry_id), id(id), date(date), operation(operation), quantidade(quantidade) {}

int movimentacao::getEntry_id() const { return entry_id; }
int movimentacao::getID() const { return id; }
std::string movimentacao::getDate() const { return date; }
char movimentacao::getOperation() const { return operation; }
float movimentacao::getQuantidade() const { return quantidade; }


void criar_registro(char operation, int idEntrada){
    string data;
    float quantidadeMovimentada;

    cout<<"Digite a data da compra(DD/MM/AAAA): "<<endl;
    cin>>data;

    cout<<"Digite a quantidade movimentada: "<<endl;
    cin>>quantidadeMovimentada;

    registros+=1;
    movimentacao nova(idEntrada, registros, data, operation, quantidadeMovimentada);

    movimentacoes.emplace_back(idEntrada, registros, data, operation, quantidadeMovimentada);

}



float Calcular_Saldo(){
    int* ID_do_Cliente = new int;
    float Saldo = 0;
    cout<<"Digite o ID do cliente: "<<endl;
    cin>>*ID_do_Cliente;
    for (auto& movimentacao : movimentacoes){
        if(movimentacao.getEntry_id() == *ID_do_Cliente){
            if(movimentacao.getOperation() == 'C'){
                Saldo+=movimentacao.getQuantidade();
            }
            else{
                Saldo -=movimentacao.getQuantidade();
            }
        }
    }
    delete ID_do_Cliente;
    return Saldo;
}

void ExibirRegistro(string titular, int id){
    cout<<CIANO<<"Registro do "<<titular<<RESET<<endl;
    for (auto& movimentacao : movimentacoes){
        if(movimentacao.getEntry_id() == id){
           if(movimentacao.getOperation()=='C'){
                cout<<"ID: " << movimentacao.getID() <<" Operação: Compra"<<" Data: "<<movimentacao.getDate()<<" Valor: "<<movimentacao.getQuantidade()<<endl;
           }
           else{
                cout<<"ID: " << movimentacao.getID() <<" Operação: Venda"<<" Data: "<<movimentacao.getDate()<<" Valor: "<<movimentacao.getQuantidade()<<endl;
           }
        }
    }

}


void ExcluirRegistrosDaCarteira(int entry_ID) {
    if (movimentacoes.empty()) {
        cout << VERMELHO << "Nenhuma movimentação cadastrada no sistema!" << RESET << endl;
        return;
    }
        
    for (auto it = movimentacoes.begin(); it != movimentacoes.end(); ) {
        if (it->getEntry_id() == entry_ID) {
            it = movimentacoes.erase(it);
        } else {
            ++it;
        }
    }
    
}


void Calcular_Ganho_Perda(int entry_ID, string titular){
    float GanhoPerda = 0;
    float valor_do_dolar = 0;
    for (auto& movimentacao : movimentacoes){
        if(movimentacao.getEntry_id() == entry_ID){
            if(movimentacao.getOperation() == 'C'){
                valor_do_dolar = Oraculo();
                GanhoPerda-=valor_do_dolar*movimentacao.getQuantidade();
                cout<<"Compra na quantida de: "<<movimentacao.getQuantidade()<<" no dia: "<<movimentacao.getDate()<<" (Dolar no valor de "<<valor_do_dolar<<")"<<endl;
            }
            else{
                valor_do_dolar = Oraculo();
                GanhoPerda+=valor_do_dolar*movimentacao.getQuantidade();
                cout<<"Venda na quantida de: "<<movimentacao.getQuantidade()<<" no dia: "<<movimentacao.getDate()<<" (Dolar no valor de "<<valor_do_dolar<<")"<<endl;

            }
        }
    }
    if(GanhoPerda>0){
        cout<<"O cliente "<<titular<<" possui "<<GanhoPerda<<" de ganho"<<endl;
    }
    else if(GanhoPerda<0){
        cout<<"O cliente "<<titular<<" possui "<<GanhoPerda<<" de perda"<<endl;
    }
    else{
        cout<<CIANO<<"\nO cliente"<<titular<<" não possui perdas nem ganhos"<<RESET<<endl;
    }
}




