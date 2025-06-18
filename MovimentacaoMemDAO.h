#ifndef MOVIMENTACAO_MEM_DAO_H
#define MOVIMENTACAO_MEM_DAO_H

#include "AbstractMovimentacaoDAO.h"
#include <vector>
#include <memory>
#include <algorithm>

using namespace std;

class MovimentacaoMemDAO : public AbstractMovimentacaoDAO {
public:
    MovimentacaoMemDAO();
    void insert(Movimentacao& movimentacao) override;
    void update(Movimentacao& movimentacao) override;
    void remove(int id) override;
    void removeByCarteiraId(int carteiraId) override;
    unique_ptr<Movimentacao> findById(int id) override;
    vector<unique_ptr<Movimentacao>> findByCarteiraId(int carteiraId) override;
    vector<unique_ptr<Movimentacao>> findAll() override;

private:
    vector<unique_ptr<Movimentacao>> _movimentacoes;
    int _nextId;
};

#endif /* MOVIMENTACAO_MEM_DAO_H */ 