#ifndef ABSTRACT_MOVIMENTACAO_DAO_H
#define ABSTRACT_MOVIMENTACAO_DAO_H

#include "Movimentacao.h"
#include <vector>
#include <string>
#include <memory>

using namespace std;

class AbstractMovimentacaoDAO {
public:
    virtual ~AbstractMovimentacaoDAO() = default;

    virtual void insert(Movimentacao& movimentacao) = 0;
    virtual void update(Movimentacao& movimentacao) = 0;
    virtual void remove(int id) = 0;
    virtual void removeByCarteiraId(int carteiraId) = 0;
    virtual unique_ptr<Movimentacao> findById(int id) = 0;
    virtual vector<unique_ptr<Movimentacao>> findByCarteiraId(int carteiraId) = 0;
    virtual vector<unique_ptr<Movimentacao>> findAll() = 0;
};

#endif // ABSTRACT_MOVIMENTACAO_DAO_H 