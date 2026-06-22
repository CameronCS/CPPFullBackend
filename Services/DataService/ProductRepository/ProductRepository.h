#pragma once

#include <vector>
#include <optional>
#include <Entities.h>
#include <IProductRepository.h>
#include <Logging/ILogger.h>
#include <nanodbc/nanodbc.h>

namespace DataService {
	class ProductRepository : public DataService::Interface::IProductRepository {
	public:
		ProductRepository(const std::string& connectionString, SystemFramework::Logging::ILogger* logger)
			: _logger(logger), _connection(connectionString) {
			_logger->SetServiceLevel("Data", "Product Repository");
		}
		bool WriteProductToDB(const Entities::PRDProduct&) override;
		std::optional<Entities::PRDProduct> GetProductById(int id) override;
		std::vector<Entities::PRDProduct> GetAllProducts() override;
		bool DeleteProduct(int productId) override;
		bool UpdateProduct(const Entities::PRDProduct& product) override;
	private:
		SystemFramework::Logging::ILogger* _logger;
		nanodbc::connection _connection;
	};
}
