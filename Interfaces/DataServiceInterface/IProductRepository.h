#ifndef _____I_PRODUCT_DATA_REPOSITORY_H_____
#define _____I_PRODUCT_DATA_REPOSITORY_H_____

#include <iostream>
#include <vector>
#include <optional>
#include <Entities.h>

namespace DataService::Interface {
	class IProductRepository {
	public:
		virtual bool WriteProductToDB(Entities::PRDProduct) = 0;
		virtual std::optional<Entities::PRDProduct> GetProductById(int id) = 0;
		virtual std::vector<Entities::PRDProduct> GetAllProducts() = 0;
		virtual bool DeleteProduct(int productId) = 0;
		virtual bool UpdateProduct(Entities::PRDProduct product) = 0;
	};
}

#endif