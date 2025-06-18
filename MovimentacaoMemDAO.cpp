#include "MovimentacaoMemDAO.h"
#include <algorithm>

using namespace std;

MovimentacaoMemDAO::MovimentacaoMemDAO() : _nextId(1) {}

void MovimentacaoMemDAO::insert(Movimentacao& movimentacao) {
    movimentacao.setId(_nextId++);
    _movimentacoes.push_back(make_unique<Movimentacao>(movimentacao));
}

void MovimentacaoMemDAO::update(Movimentacao& movimentacao) {
    for (auto& mov : _movimentacoes) {
        if (mov && mov->getId() == movimentacao.getId()) {
            *mov = movimentacao;
            return;
        }
    }
}

void MovimentacaoMemDAO::remove(int id) {
    _movimentacoes.erase(
        remove_if(_movimentacoes.begin(), _movimentacoes.end(),
                       [id](const unique_ptr<Movimentacao>& mov) {
                           return mov->getId() == id;
                       }),
        _movimentacoes.end());
}

void MovimentacaoMemDAO::removeByCarteiraId(int carteiraId) {
    _movimentacoes.erase(
        remove_if(_movimentacoes.begin(), _movimentacoes.end(),
                       [carteiraId](const unique_ptr<Movimentacao>& mov) {
                           return mov->getCarteiraId() == carteiraId;
                       }),
        _movimentacoes.end());
}

unique_ptr<Movimentacao> MovimentacaoMemDAO::findById(int id) {
    for (const auto& mov : _movimentacoes) {
        if (mov && mov->getId() == id) {
            return make_unique<Movimentacao>(*mov);
        }
    }
    return nullptr;
}

vector<unique_ptr<Movimentacao>> MovimentacaoMemDAO::findByCarteiraId(int carteiraId) {
    vector<unique_ptr<Movimentacao>> result;
    for (const auto& mov : _movimentacoes) {
        if (mov && mov->getCarteiraId() == carteiraId) {
            result.push_back(make_unique<Movimentacao>(*mov));
        }
    }
    return result;
}

vector<unique_ptr<Movimentacao>> MovimentacaoMemDAO::findAll() {
    vector<unique_ptr<Movimentacao>> result;
    for (const auto& mov : _movimentacoes) {
        if (mov) {
            result.push_back(make_unique<Movimentacao>(*mov));
        }
    }
    return result;
} 