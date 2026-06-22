#ifndef __PDS_H__
#define __PDS_H__

#include <iostream>
#include <vector>
#include <optional>
#include <Entities.h>
#include <IProductRepository.h>
#include <nanodbc/nanodbc.h>

namespace DataService {
	class ProductRepository : public DataService::Interface::IProductRepository {
	public:
		ProductRepository(const std::string& connectionString, SystemFramework::Logging::ILogger* logger)
			: IProductRepository(connectionString, logger), _connection(connectionString) {}
		bool WriteProductToDB(const Entities::PRDProduct&) override;
		std::optional<Entities::PRDProduct> GetProductById(int id) override;
		std::vector<Entities::PRDProduct> GetAllProducts() override;
		bool DeleteProduct(int productId) override;
		bool UpdateProduct(const Entities::PRDProduct& product) override;
	private:
		nanodbc::connection _connection;
	};
}

#endif