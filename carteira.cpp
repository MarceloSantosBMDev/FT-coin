#include "carteira.hpp"
#include "movimentacao.hpp"
#include <iostream>
using namespace std;
static int total_carteiras = 0;
vector<carteira> carteiras;
carteira::carteira() : id(0) {}

void carteira::IncluirCarteira() {
    cout << "Digite o nome do titular: ";
    getline(cin >> ws, titular);  
    cout << "Digite o nome da Corretora: ";
    getline(cin >> ws, corretora);  
    id = ++total_carteiras;
    
    cout << "\nCarteira Criada com Sucesso!\n";
    ExibirDados();
}



void carteira::ExibirDados() const {
    cout << "ID: " << id << endl;
    cout << "Titular: " << titular << endl;
    cout << "Corretora: " << corretora<< endl;
}

//getters
string carteira::getTitular()const {
    return titular;
}
int carteira::getID () const {
    return id;
}
string carteira::getCorretora() const{
    return corretora;
}


//setters
void carteira::setTitular(){
    string nome;
    cin>>nome;
    titular = nome;
}

void carteira::setCorretora(){
    string Corret;
    getline(cin >> ws, Corret);
    corretora = Corret;
}


//funções que não são da carteira

void ColocarCarteiraNoVetor(carteira nova){
    carteiras.push_back(nova);
}

carteira* BuscarCarteiraPorID(){
    int idProcurado;
    cout<<"Digite o ID da carteira: ";
    cin>>idProcurado;
    for (auto& carteira : carteiras){
        if (carteira.getID() == idProcurado){
            return &carteira;
        }
    }
    cout<<"Carteira não encontrada"<<endl;
    return nullptr; //retornando nulo se nao encontrar
}

void EditarCarteira(){
    carteira* editada = BuscarCarteiraPorID();
    if (editada == nullptr) return; 

    string dataAlteracao;
    cout << "Digite a data de alteracao (XX/XX/XXXX): ";
    cin.ignore();
    getline(cin, dataAlteracao);
    
    int opcao;
    cout << "Escolha o que deve ser editado:" << endl;
    cout << "1. Nome do titular" << endl;
    cout << "2. Data de criação" << endl;
    cout << "3. Cancelar" << endl;
    cout << "Opção: ";
    cin >> opcao;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if(opcao == 1){
        cout << "Digite o nome do titular: ";
        editada->setTitular();
        
        cout <<VERDE<< "Nome do titular alterado com sucesso!" <<RESET<< endl;
    }
    else if(opcao == 2){
        cout << "Digite a nova corretora: ";
        editada->setCorretora();  
        cout <<VERDE<< "Data de Criação alterada com sucesso!" <<RESET<< endl;
    }
    else{
        cout <<VERMELHO << "Operação cancelada!" <<RESET<< endl;
    }
}

void ExcluirCarteira(){
    shared_ptr<carteira> CarteiraSelecionada(BuscarCarteiraPorID(), [](carteira*){}); 

    if (CarteiraSelecionada == nullptr) {
        return;
    }
    cout << "\nCarteira selecionada para exclusão:\n";
    CarteiraSelecionada->ExibirDados();
    int opcao;
    cout<<"Deseja mesmo excluir essa carteira? "<<endl;
    cout<<"1. Sim"<<endl;
    cout<<"2. Não"<<endl;
    cin>>opcao;
    if(opcao == 1){
    auto it = find_if(carteiras.begin(), carteiras.end(),
            [&CarteiraSelecionada](carteira& carteiravtr) {  
                return carteiravtr.getID() == CarteiraSelecionada->getID();
            });

        if (it != carteiras.end()) {
            carteiras.erase(it);
            ExcluirRegistrosDaCarteira(CarteiraSelecionada->getID());
            cout <<VERDE<< "Carteira excluída com sucesso!\n"<<RESET<<endl;
        }
    else{
        cout<<"Operação Cancelada!"<<endl;
    }
    }
}


//funcoes para o menu relatorio
void ExibirTodasAsCarteirasID(){
    cout << "\n=== CARTEIRAS ORDENADAS POR ID ===\n";
    for (auto& carteira : carteiras){
            cout<<"ID: " << carteira.getID() << " | Titular: " << carteira.getTitular()<<endl;
    }
    cout << "Total: " << carteiras.size() << " carteiras\n\n";

}

void ExibirTodasAsCarteirasNome() {
    sort(carteiras.begin(), carteiras.end(), 
        [](const carteira& a, const carteira& b) {
            return a.getTitular() < b.getTitular();
        });

    cout << "\n=== CARTEIRAS ORDENADAS POR NOME ===\n";
    for (const auto& c : carteiras) {
        cout << "ID: " << c.getID() << " | Titular: " << c.getTitular() << endl;
    }
    cout << "Total: " << carteiras.size() << " carteiras\n\n";
}


void ExibirSaldoTodasAsCarteiras(){
    cout << "\n=== CARTEIRAS ORDENADAS POR ID ===\n";
    for (auto& carteira : carteiras){
        Calcular_Ganho_Perda(carteira.getID(), carteira.getTitular());
        cout<<"\n\n"<<endl;
    }
    cout << "Total: " << carteiras.size() << " carteiras\n\n";
}


