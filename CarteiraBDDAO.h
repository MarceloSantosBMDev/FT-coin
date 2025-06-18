#ifndef CARTEIRA_BD_DAO_H
#define CARTEIRA_BD_DAO_H

#include "AbstractCarteiraDAO.h"
#include "ConnectDB.h"
#include <vector>
#include <memory>
#include <string>

using namespace std;

class CarteiraBDDAO : public AbstractCarteiraDAO {
public:
    CarteiraBDDAO(ConnectDB* connection);
    void insert(Carteira& carteira) override;
    void update(Carteira& carteira) override;
    void remove(int id) override;
    unique_ptr<Carteira> findById(int id) override;
    vector<unique_ptr<Carteira>> findAll() override;
    unique_ptr<Carteira> findByTitular(const string& titular) override;

private:
    ConnectDB* _connection;
};

#endif // CARTEIRA_BD_DAO_H 