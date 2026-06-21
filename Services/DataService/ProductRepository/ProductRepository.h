#ifndef __PDS_H__
#define __PDS_H__

#include <iostream>
#include <vector>
#include <optional>
#include <Entities.h>
#include <IProductRepository.h>

namespace DataService {
	class ProductRepository : public DataService::Interface::IProductRepository {
	public:
		ProductRepository(std::string connectionString, SystemFramework::Logging::ILogger* logger)
			: IProductRepository(connectionString, logger) {}
		bool WriteProductToDB(Entities::PRDProduct) override;
		std::optional<Entities::PRDProduct> GetProductById(int id) override;
		std::vector<Entities::PRDProduct> GetAllProducts() override;
		bool DeleteProduct(int productId) override;
		bool UpdateProduct(Entities::PRDProduct product) override;
	};
}

#endif