#include "Carteira.h"

using namespace std;

Carteira::Carteira() : _id(0), _titular(""), _corretora("") {}

Carteira::Carteira(const string& titular, const string& corretora) : _id(0), _titular(titular), _corretora(corretora) {}

Carteira::Carteira(int id, const string& titular, const string& corretora) : _id(id), _titular(titular), _corretora(corretora) {}

Carteira::Carteira(const Carteira& other) : _id(other._id), _titular(other._titular), _corretora(other._corretora) {}

Carteira& Carteira::operator=(const Carteira& other) {
    if (this != &other) {
        _id = other._id;
        _titular = other._titular;
        _corretora = other._corretora;
    }
    return *this;
}

int Carteira::getId() const {
    return _id;
}

void Carteira::setId(int id) {
    _id = id;
}

const string& Carteira::getTitular() const {
    return _titular;
}

void Carteira::setTitular(const string& titular) {
    _titular = titular;
}

const string& Carteira::getCorretora() const {
    return _corretora;
}

void Carteira::setCorretora(const string& corretora) {
    _corretora = corretora;
}

ostream& operator<<(ostream& os, const Carteira& carteira) {
    os << "ID: " << carteira.getId() << ", Titular: " << carteira.getTitular() << ", Corretora: " << carteira.getCorretora();
    return os;
} 