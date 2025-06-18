#ifndef CARTEIRA_MEM_DAO_H
#define CARTEIRA_MEM_DAO_H

#include "AbstractCarteiraDAO.h"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

using namespace std;

class CarteiraMemDAO : public AbstractCarteiraDAO {
public:
    CarteiraMemDAO();
    void insert(Carteira& carteira) override;
    void update(Carteira& carteira) override;
    void remove(int id) override;
    unique_ptr<Carteira> findById(int id) override;
    vector<unique_ptr<Carteira>> findAll() override;
    unique_ptr<Carteira> findByTitular(const string& titular) override;

private:
    vector<unique_ptr<Carteira>> _carteiras;
    int _nextId;
};

#endif 