#include "ProductRepository.h"
#include "DBUtils/DBUtils.h"
#include <nanodbc/nanodbc.h>
#include <Entities.h>
namespace db = nanodbc;

bool DataService::ProductRepository::WriteProductToDB(const Entities::PRDProduct& product) {
	try {
		db::statement stmt(_connection);
		db::prepare(stmt, NANODBC_TEXT("INSERT INTO PRD_Products (Name, Price) VALUES (?, ?)"));
		stmt.bind(0, product.Name.c_str());
		stmt.bind(1, &product.Price);
		db::result res = db::execute(stmt);
		if (res.affected_rows() == 0) {
			_logger->LogWarning("WriteProductToDB - No rows affected for product: " + product.Name);
			return false;
		}
		return true;
	}
	catch (const db::database_error& e) {
		_logger->LogError("WriteProductToDB - Database error: " + std::string(e.what()));
		return false;
	}
	catch (const std::exception& e) {
		_logger->LogError("WriteProductToDB - Unexpected error: " + std::string(e.what()));
		return false;
	}
}

std::optional<Entities::PRDProduct> DataService::ProductRepository::GetProductById(int id) {
	try {
		db::statement stmt(_connection);
		db::prepare(stmt, NANODBC_TEXT("SELECT ID, Name, Price FROM PRD_Products WHERE ID = ?"));
		stmt.bind(0, &id);
		db::result res = db::execute(stmt);

		if (!res.next()) {
			return std::nullopt;
		}

		std::optional<Entities::PRDProduct> resultProduct = DataService::Utils::CreateProductFromResult(res);
		return resultProduct;
	}
	catch (const db::database_error& e) {
		_logger->LogError("GetProductById - Database error for id " + std::to_string(id) + ": " + std::string(e.what()));
		return std::nullopt;
	}
	catch (const std::exception& e) {
		_logger->LogError("GetProductById - Unexpected error for id " + std::to_string(id) + ": " + std::string(e.what()));
		return std::nullopt;
	}
}

std::vector<Entities::PRDProduct> DataService::ProductRepository::GetAllProducts() {
	try {
		db::statement stmt(_connection);
		db::prepare(stmt, NANODBC_TEXT("SELECT ID, Name, Price FROM PRD_Products"));
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
	catch (const db::database_error& e) {
		_logger->LogError("GetAllProducts - Database error: " + std::string(e.what()));
		return {};
	}
	catch (const std::exception& e) {
		_logger->LogError("GetAllProducts - Unexpected error: " + std::string(e.what()));
		return {};
	}
}

bool DataService::ProductRepository::DeleteProduct(int id) {
	try {
		db::statement stmt(_connection);
		db::prepare(stmt, NANODBC_TEXT("DELETE FROM PRD_Products WHERE ID = ?"));
		stmt.bind(0, &id);

		db::result res = db::execute(stmt);
		if (res.affected_rows() != 1) {
			_logger->LogWarning("DeleteProduct - No rows affected for id: " + std::to_string(id));
			return false;
		}
		return true;
	}
	catch (const db::database_error& e) {
		_logger->LogError("DeleteProduct - Database error for id " + std::to_string(id) + ": " + std::string(e.what()));
		return false;
	}
	catch (const std::exception& e) {
		_logger->LogError("DeleteProduct - Unexpected error for id " + std::to_string(id) + ": " + std::string(e.what()));
		return false;
	}
}

bool DataService::ProductRepository::UpdateProduct(const Entities::PRDProduct& product) {
	try {
		db::statement stmt(_connection);
		db::prepare(stmt, NANODBC_TEXT("UPDATE PRD_Products SET Name = ?, Price = ? WHERE ID = ?"));
		stmt.bind(0, product.Name.c_str());
		stmt.bind(1, &product.Price);
		stmt.bind(2, &product.ID);
		db::result res = db::execute(stmt);
		if (res.affected_rows() != 1) {
			_logger->LogWarning("UpdateProduct - No rows affected for product id: " + std::to_string(product.ID));
			return false;
		}
		return true;
	}
	catch (const db::database_error& e) {
		_logger->LogError("UpdateProduct - Database error for id " + std::to_string(product.ID) + ": " + std::string(e.what()));
		return false;
	}
	catch (const std::exception& e) {
		_logger->LogError("UpdateProduct - Unexpected error for id " + std::to_string(product.ID) + ": " + std::string(e.what()));
		return false;
	}
}
