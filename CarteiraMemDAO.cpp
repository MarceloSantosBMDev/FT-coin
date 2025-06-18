#include "CarteiraMemDAO.h"
#include <algorithm>

using namespace std;

CarteiraMemDAO::CarteiraMemDAO() : _nextId(1) {}

void CarteiraMemDAO::insert(Carteira& carteira) {
    carteira.setId(_nextId++);
    _carteiras.push_back(make_unique<Carteira>(carteira));
}

void CarteiraMemDAO::update(Carteira& carteira) {
    for (auto& c : _carteiras) {
        if (c && c->getId() == carteira.getId()) {
            *c = carteira;
            return;
        }
    }
}

void CarteiraMemDAO::remove(int id) {
    _carteiras.erase(
        remove_if(_carteiras.begin(), _carteiras.end(),
                       [id](const unique_ptr<Carteira>& c) {
                           return c && c->getId() == id;
                       }),
        _carteiras.end());
}

unique_ptr<Carteira> CarteiraMemDAO::findById(int id) {
    for (const auto& c : _carteiras) {
        if (c && c->getId() == id) {
            return make_unique<Carteira>(*c);
        }
    }
    return nullptr;
}

vector<unique_ptr<Carteira>> CarteiraMemDAO::findAll() {
    vector<unique_ptr<Carteira>> result;
    for (const auto& c : _carteiras) {
        if (c) {
            result.push_back(make_unique<Carteira>(*c));
        }
    }
    return result;
}

unique_ptr<Carteira> CarteiraMemDAO::findByTitular(const string& titular) {
    for (const auto& c : _carteiras) {
        if (c && c->getTitular() == titular) {
            return make_unique<Carteira>(*c);
        }
    }
    return nullptr;
} 