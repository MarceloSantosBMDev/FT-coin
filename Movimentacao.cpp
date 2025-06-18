#include "Movimentacao.h"

using namespace std;

Movimentacao::Movimentacao() : _id(0), _carteiraId(0), _tipoOperacao(""), _quantidade(0.0), _data("") {}

Movimentacao::Movimentacao(int carteiraId, const string& tipoOperacao, double quantidade, const string& data)
    : _id(0), _carteiraId(carteiraId), _tipoOperacao(tipoOperacao), _quantidade(quantidade), _data(data) {}

Movimentacao::Movimentacao(int id, int carteiraId, const string& tipoOperacao, double quantidade, const string& data)
    : _id(id), _carteiraId(carteiraId), _tipoOperacao(tipoOperacao), _quantidade(quantidade), _data(data) {}

Movimentacao::Movimentacao(const Movimentacao& other)
    : _id(other._id),
      _carteiraId(other._carteiraId),
      _tipoOperacao(other._tipoOperacao),
      _quantidade(other._quantidade),
      _data(other._data) {}

Movimentacao& Movimentacao::operator=(const Movimentacao& other) {
    if (this != &other) {
        _id = other._id;
        _carteiraId = other._carteiraId;
        _tipoOperacao = other._tipoOperacao;
        _quantidade = other._quantidade;
        _data = other._data;
    }
    return *this;
}

int Movimentacao::getId() const {
    return _id;
}

void Movimentacao::setId(int id) {
    _id = id;
}

int Movimentacao::getCarteiraId() const {
    return _carteiraId;
}

void Movimentacao::setCarteiraId(int carteiraId) {
    _carteiraId = carteiraId;
}

const string& Movimentacao::getTipoOperacao() const {
    return _tipoOperacao;
}

void Movimentacao::setTipoOperacao(const string& tipoOperacao) {
    _tipoOperacao = tipoOperacao;
}

double Movimentacao::getQuantidade() const {
    return _quantidade;
}

void Movimentacao::setQuantidade(double quantidade) {
    _quantidade = quantidade;
}

const string& Movimentacao::getData() const {
    return _data;
}

void Movimentacao::setData(const string& data) {
    _data = data;
} 