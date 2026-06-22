#ifndef _____I_PRODUCT_DATA_REPOSITORY_H_____
#define _____I_PRODUCT_DATA_REPOSITORY_H_____

#include <iostream>
#include <vector>
#include <optional>
#include <Entities.h>
#include <Logging/ILogger.h>

namespace DataService::Interface {
	class IProductRepository {
	public:
		IProductRepository(const std::string& connectionString, SystemFramework::Logging::ILogger* logger) {
			_connectionString = connectionString;
			_logger = logger;
			_logger->SetServiceLevel("Data", "Product Repository");
		}
		virtual ~IProductRepository() = default;
		virtual bool WriteProductToDB(const Entities::PRDProduct&) = 0;
		virtual std::optional<Entities::PRDProduct> GetProductById(int id) = 0;
		virtual std::vector<Entities::PRDProduct> GetAllProducts() = 0;
		virtual bool DeleteProduct(int productId) = 0;
		virtual bool UpdateProduct(const Entities::PRDProduct& product) = 0;
	protected:
		std::string _connectionString;
		SystemFramework::Logging::ILogger* _logger;
	};
}

#endif