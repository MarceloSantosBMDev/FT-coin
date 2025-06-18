#ifndef MOVIMENTACAO_H
#define MOVIMENTACAO_H

#include <string>

using namespace std;

class Movimentacao {
public:
    Movimentacao();
    Movimentacao(int carteiraId, const string& tipoOperacao, double quantidade, const string& data);
    Movimentacao(int id, int carteiraId, const string& tipoOperacao, double quantidade, const string& data);
    Movimentacao(const Movimentacao& other);
    Movimentacao& operator=(const Movimentacao& other);
    ~Movimentacao() = default;

    int getId() const;
    void setId(int id);
    int getCarteiraId() const;
    void setCarteiraId(int carteiraId);
    const string& getTipoOperacao() const;
    void setTipoOperacao(const string& tipoOperacao);
    double getQuantidade() const;
    void setQuantidade(double quantidade);
    const string& getData() const;
    void setData(const string& data);

private:
    int _id;
    int _carteiraId;
    string _tipoOperacao;
    double _quantidade;
    string _data;
};

#endif /* MOVIMENTACAO_H_ */ 