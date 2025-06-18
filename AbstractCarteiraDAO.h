#ifndef ABSTRACT_CARTEIRA_DAO_H
#define ABSTRACT_CARTEIRA_DAO_H

#include "Carteira.h"
#include <vector>
#include <string>
#include <memory>

using namespace std;

class AbstractCarteiraDAO {
public:
    virtual ~AbstractCarteiraDAO() = default;

    virtual void insert(Carteira& carteira) = 0;
    virtual void update(Carteira& carteira) = 0;
    virtual void remove(int id) = 0;
    virtual unique_ptr<Carteira> findById(int id) = 0;
    virtual vector<unique_ptr<Carteira>> findAll() = 0;
    virtual unique_ptr<Carteira> findByTitular(const string& titular) = 0;
};

#endif // ABSTRACT_CARTEIRA_DAO_H 