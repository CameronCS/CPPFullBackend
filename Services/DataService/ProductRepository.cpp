#include "ProductRepository.h"
#include "ProductRepository.h"
#include "DBUtils/DBUtils.h"
#include <nanodbc/nanodbc.h>
#include <Entities.h>
namespace db = nanodbc;

static std::string connectionString = "Driver={ODBC Driver 18 for SQL Server};Server=localhost;Database=CPPBackend;Trusted_Connection=yes;TrustServerCertificate=yes;";

bool DataService::ProductRepository::WriteProductToDB(Entities::PRDProduct product) {
	db::connection conn(connectionString);
	db::statement stmt(conn);
	db::prepare(stmt, NANODBC_TEXT("INSERT INTO PRD_Products (Name, Price) VALUES (?, ?)"));
	stmt.bind(0, product.Name.c_str());
	stmt.bind(1, &product.Price);
	db::result res = db::execute(stmt);
	if (res.affected_rows() == 0) {
		return false;
	}
	return true;
}

std::optional<Entities::PRDProduct> DataService::ProductRepository::GetProductById(int id) {
	db::connection conn(connectionString);
	db::statement stmt(conn);
	db::prepare(stmt, NANODBC_TEXT("SELECT * FROM PRD_Products WHERE ID = ?"));
	stmt.bind(0, &id);
	db::result res = db::execute(stmt);

	if (!res.next()) {
		return std::nullopt;
	}

	std::optional<Entities::PRDProduct> resultProduct = DataService::Utils::CreateProductFromResult(res);
	return resultProduct;
}

std::vector<Entities::PRDProduct> DataService::ProductRepository::GetAllProducts() {
	db::connection conn(connectionString);
	db::statement stmt(conn);
	db::prepare(stmt, NANODBC_TEXT("SELECT * FROM PRD_Products"));
	db::result res = db::execute(stmt);

	std::vector<Entities::PRDProduct> resultVector;
	while (res.next()) {
		std::optional<Entities::PRDProduct> currentObject = DataService::Utils::CreateProductFromResult(res);
		if (currentObject.has_value()) {
			resultVector.push_back(currentObject.value());
		}
	}
	return resultVector;
}

bool DataService::ProductRepository::DeleteProduct(int id) {
	db::connection conn(connectionString);
	db::statement stmt(conn);
	db::prepare(stmt, NANODBC_TEXT("DELETE FROM PRD_Products WHERE ID = ?"));
	stmt.bind(0, &id);

	db::result res = db::execute(stmt);
	if (res.affected_rows() != 1) {
		return false;
	}
	return true;
}

bool DataService::ProductRepository::UpdateProduct(Entities::PRDProduct product) {
	db::connection conn(connectionString);
	db::statement stmt(conn);
	db::prepare(stmt, NANODBC_TEXT("UPDATE PRD_Products SET Name = ?, Price = ? WHERE ID = ?"));
	stmt.bind(0, product.Name.c_str());
	stmt.bind(1, &product.Price);
	stmt.bind(2, &product.ID);
	db::result res = db::execute(stmt);
	if (res.affected_rows() != 1) {
		return false;
	}
	return true;
}