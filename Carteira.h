#ifndef CARTEIRA_H
#define CARTEIRA_H

#include <string>
#include <ostream>

using namespace std;

class Carteira {
public:
    Carteira();
    Carteira(const string& titular, const string& corretora);
    Carteira(int id, const string& titular, const string& corretora);
    Carteira(const Carteira& other);
    Carteira& operator=(const Carteira& other);
    ~Carteira() = default;

    int getId() const;
    void setId(int id);
    const string& getTitular() const;
    void setTitular(const string& titular);
    const string& getCorretora() const;
    void setCorretora(const string& corretora);

private:
    int _id;
    string _titular;
    string _corretora;
};

ostream& operator<<(ostream& os, const Carteira& carteira);

#endif /* CARTEIRA_H_ */ 