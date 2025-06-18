#ifndef MOVIMENTACAO_BD_DAO_H
#define MOVIMENTACAO_BD_DAO_H

#include "AbstractMovimentacaoDAO.h"
#include "ConnectDB.h"
#include <vector>
#include <memory>

using namespace std;

class MovimentacaoBDDAO : public AbstractMovimentacaoDAO {
public:
    MovimentacaoBDDAO(ConnectDB* connection);
    void insert(Movimentacao& movimentacao) override;
    void update(Movimentacao& movimentacao) override;
    void remove(int id) override;
    void removeByCarteiraId(int carteiraId) override;
    unique_ptr<Movimentacao> findById(int id) override;
    vector<unique_ptr<Movimentacao>> findByCarteiraId(int carteiraId) override;
    vector<unique_ptr<Movimentacao>> findAll() override;

private:
    ConnectDB* _connection;
};

#endif // MOVIMENTACAO_BD_DAO_H 