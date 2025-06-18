#include "MovimentacaoBDDAO.h"
#include <stdexcept>
#include <mariadb/conncpp.hpp>

using namespace std;

MovimentacaoBDDAO::MovimentacaoBDDAO(ConnectDB* connection) : _connection(connection) {}

void MovimentacaoBDDAO::insert(Movimentacao& movimentacao) {
    try {
        unique_ptr<sql::PreparedStatement> stmnt(_connection->getConnection()->prepareStatement("INSERT INTO MOVIMENTACAO (IdCarteira, Data, TipoOperacao, Quantidade) VALUES (?, ?, ?, ?)"));
        stmnt->setInt(1, movimentacao.getCarteiraId());
        stmnt->setString(2, movimentacao.getData());
        stmnt->setString(3, movimentacao.getTipoOperacao());
        stmnt->setDouble(4, movimentacao.getQuantidade());
        stmnt->executeUpdate();
    } catch (sql::SQLException& e) {
        throw runtime_error("Error inserting movimentacao: " + string(e.what()));
    }
}

void MovimentacaoBDDAO::update(Movimentacao& movimentacao) {
    try {
        unique_ptr<sql::PreparedStatement> stmnt(_connection->getConnection()->prepareStatement("UPDATE MOVIMENTACAO SET IdCarteira = ?, Data = ?, TipoOperacao = ?, Quantidade = ? WHERE IdMovimento = ?"));
        stmnt->setInt(1, movimentacao.getCarteiraId());
        stmnt->setString(2, movimentacao.getData());
        stmnt->setString(3, movimentacao.getTipoOperacao());
        stmnt->setDouble(4, movimentacao.getQuantidade());
        stmnt->setInt(5, movimentacao.getId());
        stmnt->executeUpdate();
    } catch (sql::SQLException& e) {
        throw runtime_error("Error updating movimentacao: " + string(e.what()));
    }
}

void MovimentacaoBDDAO::remove(int id) {
    try {
        unique_ptr<sql::PreparedStatement> stmnt(_connection->getConnection()->prepareStatement("DELETE FROM MOVIMENTACAO WHERE IdMovimento = ?"));
        stmnt->setInt(1, id);
        stmnt->executeUpdate();
    } catch (sql::SQLException& e) {
        throw runtime_error("Error deleting movimentacao: " + string(e.what()));
    }
}

void MovimentacaoBDDAO::removeByCarteiraId(int carteiraId) {
    try {
        unique_ptr<sql::PreparedStatement> stmnt(_connection->getConnection()->prepareStatement("DELETE FROM MOVIMENTACAO WHERE IdCarteira = ?"));
        stmnt->setInt(1, carteiraId);
        stmnt->executeUpdate();
    } catch (sql::SQLException& e) {
        throw runtime_error("Error deleting movimentacoes by carteira_id: " + string(e.what()));
    }
}

unique_ptr<Movimentacao> MovimentacaoBDDAO::findById(int id) {
    try {
        unique_ptr<sql::PreparedStatement> stmnt(_connection->getConnection()->prepareStatement("SELECT * FROM MOVIMENTACAO WHERE IdMovimento = ?"));
        stmnt->setInt(1, id);
        sql::ResultSet* res = stmnt->executeQuery();
        if (res->next()) {
            auto mov = make_unique<Movimentacao>(res->getInt("IdMovimento"), res->getInt("IdCarteira"), string(res->getString("TipoOperacao")), res->getDouble("Quantidade"), string(res->getString("Data")));
            delete res;
            return mov;
        }
        delete res;
    } catch (sql::SQLException& e) {
        throw runtime_error("Error finding movimentacao: " + string(e.what()));
    }
    return nullptr;
}

vector<unique_ptr<Movimentacao>> MovimentacaoBDDAO::findByCarteiraId(int carteiraId) {
    vector<unique_ptr<Movimentacao>> movimentacoes;
    try {
        unique_ptr<sql::PreparedStatement> stmnt(_connection->getConnection()->prepareStatement("SELECT * FROM MOVIMENTACAO WHERE IdCarteira = ?"));
        stmnt->setInt(1, carteiraId);
        sql::ResultSet* res = stmnt->executeQuery();
        while (res->next()) {
            movimentacoes.push_back(make_unique<Movimentacao>(res->getInt("IdMovimento"), res->getInt("IdCarteira"), string(res->getString("TipoOperacao")), res->getDouble("Quantidade"), string(res->getString("Data"))));
        }
        delete res;
    } catch (sql::SQLException& e) {
        throw runtime_error("Error finding movimentacoes by carteira_id: " + string(e.what()));
    }
    return movimentacoes;
}

vector<unique_ptr<Movimentacao>> MovimentacaoBDDAO::findAll() {
    vector<unique_ptr<Movimentacao>> movimentacoes;
    try {
        unique_ptr<sql::Statement> stmnt(_connection->getConnection()->createStatement());
        sql::ResultSet* res = stmnt->executeQuery("SELECT * FROM MOVIMENTACAO");
        while (res->next()) {
            movimentacoes.push_back(make_unique<Movimentacao>(res->getInt("IdMovimento"), res->getInt("IdCarteira"), string(res->getString("TipoOperacao")), res->getDouble("Quantidade"), string(res->getString("Data"))));
        }
        delete res;
    } catch (sql::SQLException& e) {
        throw runtime_error("Error finding all movimentacoes: " + string(e.what()));
    }
    return movimentacoes;
} 