#include "CarteiraBDDAO.h"
#include <stdexcept>
#include <mariadb/conncpp.hpp>

using namespace std;

CarteiraBDDAO::CarteiraBDDAO(ConnectDB* connection) : _connection(connection) {}

void CarteiraBDDAO::insert(Carteira& carteira) {
    try {
        unique_ptr<sql::PreparedStatement> stmnt(_connection->getConnection()->prepareStatement("INSERT INTO CARTEIRA (Titular, Corretora) VALUES (?, ?)"));
        stmnt->setString(1, carteira.getTitular());
        stmnt->setString(2, carteira.getCorretora());
        stmnt->executeUpdate();
    } catch (sql::SQLException& e) {
        throw runtime_error("Error inserting carteira: " + string(e.what()));
    }
}

void CarteiraBDDAO::update(Carteira& carteira) {
    try {
        unique_ptr<sql::PreparedStatement> stmnt(_connection->getConnection()->prepareStatement("UPDATE CARTEIRA SET Titular = ?, Corretora = ? WHERE IdCarteira = ?"));
        stmnt->setString(1, carteira.getTitular());
        stmnt->setString(2, carteira.getCorretora());
        stmnt->setInt(3, carteira.getId());
        stmnt->executeUpdate();
    } catch (sql::SQLException& e) {
        throw runtime_error("Error updating carteira: " + string(e.what()));
    }
}

void CarteiraBDDAO::remove(int id) {
    try {
        unique_ptr<sql::PreparedStatement> stmnt(_connection->getConnection()->prepareStatement("DELETE FROM CARTEIRA WHERE IdCarteira = ?"));
        stmnt->setInt(1, id);
        stmnt->executeUpdate();
    } catch (sql::SQLException& e) {
        throw runtime_error("Error deleting carteira: " + string(e.what()));
    }
}

unique_ptr<Carteira> CarteiraBDDAO::findById(int id) {
    try {
        unique_ptr<sql::PreparedStatement> stmnt(_connection->getConnection()->prepareStatement("SELECT * FROM CARTEIRA WHERE IdCarteira = ?"));
        stmnt->setInt(1, id);
        sql::ResultSet* res = stmnt->executeQuery();
        if (res->next()) {
            auto carteira = make_unique<Carteira>(res->getInt("IdCarteira"), string(res->getString("Titular")), string(res->getString("Corretora")));
            delete res;
            return carteira;
        }
        delete res;
    } catch (sql::SQLException& e) {
        throw runtime_error("Error finding carteira: " + string(e.what()));
    }
    return nullptr;
}

vector<unique_ptr<Carteira>> CarteiraBDDAO::findAll() {
    vector<unique_ptr<Carteira>> carteiras;
    try {
        unique_ptr<sql::Statement> stmnt(_connection->getConnection()->createStatement());
        sql::ResultSet* res = stmnt->executeQuery("SELECT * FROM CARTEIRA");
        while (res->next()) {
            carteiras.push_back(make_unique<Carteira>(res->getInt("IdCarteira"), string(res->getString("Titular")), string(res->getString("Corretora"))));
        }
        delete res;
    } catch (sql::SQLException& e) {
        throw runtime_error("Error finding all carteiras: " + string(e.what()));
    }
    return carteiras;
}

unique_ptr<Carteira> CarteiraBDDAO::findByTitular(const string& titular) {
    try {
        unique_ptr<sql::PreparedStatement> stmnt(_connection->getConnection()->prepareStatement("SELECT * FROM CARTEIRA WHERE Titular = ?"));
        stmnt->setString(1, titular);
        sql::ResultSet* res = stmnt->executeQuery();
        if (res->next()) {
            auto carteira = make_unique<Carteira>(res->getInt("IdCarteira"), string(res->getString("Titular")), string(res->getString("Corretora")));
            delete res;
            return carteira;
        }
        delete res;
    } catch (sql::SQLException& e) {
        throw runtime_error("Error finding carteira by titular: " + string(e.what()));
    }
    return nullptr;
} 